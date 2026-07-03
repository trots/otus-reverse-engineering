#include <windows.h>
#include <stdio.h>

BYTE* ReadFileToMemory(const char* path, DWORD& fileSize)
{
  HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    return nullptr;

  fileSize = GetFileSize(hFile, NULL);
  BYTE* buffer = new BYTE[fileSize];
  DWORD read;
  ReadFile(hFile, buffer, fileSize, &read, NULL);
  CloseHandle(hFile);
  return buffer;
}

bool LoadPE(const char* path)
{
  // Read file
  DWORD fileSize;
  BYTE* fileData = ReadFileToMemory(path, fileSize);
  if (!fileData)
  {
    printf("Cannot read file: %s\n", path);
    return false;
  }
  printf("File loaded: %u bytes\n", fileSize);

  // Check PE signatures
  IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)fileData;
  if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) // 'MZ'
  {
    printf("Not a PE file (no MZ signature)\n");
    delete[] fileData;
    return false;
  }

  IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(fileData + dosHeader->e_lfanew);
  if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) // 'PE\0\0'
  {
    printf("Invalid PE signature\n");
    delete[] fileData;
    return false;
  }

  // Extract PE parameters
  DWORD imageSize = ntHeaders->OptionalHeader.SizeOfImage;
  DWORD originalBase = ntHeaders->OptionalHeader.ImageBase;
  DWORD entryRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;

  printf("Original ImageBase: 0x%08X\n", originalBase);
  printf("SizeOfImage: 0x%X\n", imageSize);
  printf("EntryPoint RVA: 0x%X\n", entryRVA);

  // Alloc memory
  BYTE* newBase = (BYTE*)VirtualAlloc(NULL, imageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  if (!newBase)
  {
    printf("VirtualAlloc failed: %u\n", GetLastError());
    delete[] fileData;
    return false;
  }
  printf("Allocated at: 0x%p\n", newBase);

  DWORD delta = (DWORD)newBase - originalBase;
  printf("Relocation delta: 0x%X\n", delta);

  // Copy headers
  memcpy(newBase, fileData, ntHeaders->OptionalHeader.SizeOfHeaders);

  // Copy sections
  IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(ntHeaders);
  for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i)
  {
    if (section[i].SizeOfRawData == 0)
      continue;
    BYTE* dest = newBase + section[i].VirtualAddress;
    BYTE* src  = fileData + section[i].PointerToRawData;
    memcpy(dest, src, section[i].SizeOfRawData);
    printf("Section copied: %-8s VA=0x%X RawSize=0x%X\n",
           section[i].Name, section[i].VirtualAddress, section[i].SizeOfRawData);
  }

  // Process import table
  DWORD importDirRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
  if (importDirRVA)
  {
    IMAGE_IMPORT_DESCRIPTOR* importDesc = (IMAGE_IMPORT_DESCRIPTOR*)(newBase + importDirRVA);

    while (importDesc->Name != 0)
    {
      const char* dllName = (const char*)(newBase + importDesc->Name);
      printf("Loading DLL: %s\n", dllName);

      HMODULE hDll = LoadLibraryA(dllName);
      if (!hDll)
      {
        printf("LoadLibrary failed: %u\n", GetLastError());
        importDesc++;
        continue;
      }

      IMAGE_THUNK_DATA* origThunk = (IMAGE_THUNK_DATA*)(newBase + importDesc->OriginalFirstThunk);
      IMAGE_THUNK_DATA* iatThunk = (IMAGE_THUNK_DATA*)(newBase + importDesc->FirstThunk);

      while (origThunk->u1.AddressOfData != 0)
      {
        FARPROC funcAddr = NULL;

        if (origThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
        {
          WORD ordinal = IMAGE_ORDINAL(origThunk->u1.Ordinal);
          funcAddr = GetProcAddress(hDll, (LPCSTR)(ULONG_PTR)ordinal);
          printf("    [ordinal %u] -> 0x%p\n", ordinal, funcAddr);
        } else
        {
          IMAGE_IMPORT_BY_NAME* importByName =
              (IMAGE_IMPORT_BY_NAME*)(newBase + origThunk->u1.AddressOfData);
          funcAddr = GetProcAddress(hDll, (LPCSTR)importByName->Name);
          printf("    [%s] -> 0x%p\n", importByName->Name, funcAddr);
        }

        iatThunk->u1.Function = (ULONG_PTR)funcAddr;
        origThunk++;
        iatThunk++;
      }
      importDesc++;
    }
  }

  // Process reloc table
  DWORD relocDirRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
  DWORD relocDirSize = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

  printf("Relocation Dir RVA: 0x%X, Size: 0x%X\n", relocDirRVA, relocDirSize);
  if (relocDirRVA && delta != 0)
  {
    printf("Applying relocations...\n");
    IMAGE_BASE_RELOCATION* reloc = (IMAGE_BASE_RELOCATION*)(newBase + relocDirRVA);

    while (reloc->VirtualAddress != 0)
    {
      DWORD pageBase = (DWORD)(newBase + reloc->VirtualAddress);
      WORD* entries = (WORD*)((BYTE*)reloc + sizeof(IMAGE_BASE_RELOCATION));
      int count = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

      for (int i = 0; i < count; ++i)
      {
        WORD type = entries[i] >> 12;
        WORD offset = entries[i] & 0xFFF;

        if (type == IMAGE_REL_BASED_HIGHLOW)
        {
          DWORD* patchAddr = (DWORD*)(pageBase + offset);
          *patchAddr += delta;
        }
      }
      reloc = (IMAGE_BASE_RELOCATION*)((BYTE*)reloc + reloc->SizeOfBlock);
    }
  } else if (delta == 0)
  {
    printf("Delta is zero, relocations not needed\n");
  }

  // Call entry point
  printf("Jumping to entry point 0x%p ...\n", newBase + entryRVA);
  typedef void (__stdcall *EntryPoint)();
  EntryPoint entry = (EntryPoint)(newBase + entryRVA);
  entry();

  // Clearing
  VirtualFree(newBase, 0, MEM_RELEASE);
  delete[] fileData;
  return true;
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    printf("Usage: %s <pe_file.exe>\n", argv[0]);
    return 1;
  }
  if (!LoadPE(argv[1]))
  {
    printf("[!] Failed to load PE\n");
    return 1;
  }
  return 0;
}
