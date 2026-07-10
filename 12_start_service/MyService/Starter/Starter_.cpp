#include <Windows.h>
#include <stdio.h>

BOOL installService(LPWSTR szServiceName, LPWSTR dllName, LPWSTR szDescription)
{
	BOOL result = FALSE;
	HKEY hKey, hParKey;
	WCHAR szRegPath[MAX_PATH];
	WCHAR szServiceDll[MAX_PATH];

	wsprintf(szRegPath, L"SYSTEM\\CurrentControlSet\\Services\\%s", szServiceName);
	wsprintf(szServiceDll, L"%%SystemRoot%%\\system32\\%s", dllName);

	if (RegCreateKey(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\SvcHost", &hKey) == ERROR_SUCCESS)
	{
		DWORD cData;
		WCHAR data[700] = {0};
		RegQueryValueEx(hKey, L"netsvcs", NULL, NULL, (LPBYTE)data, &cData);
		wsprintf((LPWSTR)&data[cData / 2 - 1], L"%s\0\0\0", szServiceName);
		RegSetValueEx(hKey, L"netsvcs", 0, REG_MULTI_SZ, (BYTE*)data, cData + lstrlen(dllName)*2);
	}
	RegCloseKey(hKey);

	if (RegCreateKey(HKEY_LOCAL_MACHINE, szRegPath, &hKey) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, L"Description", 0, REG_SZ, (BYTE*)szDescription, lstrlen(szDescription));
		//CHAR szObjectName[] = "NT AUTHORITY\\NetworkService";
		//RegSetValueEx(hKey, "ObjectName", 0, REG_SZ, (BYTE*)szObjectName, lstrlen(szObjectName));
		if (RegCreateKey(hKey, L"Parameters", &hParKey) == ERROR_SUCCESS)
		{
			if (RegSetValueEx(hParKey, L"ServiceDll", 0, REG_EXPAND_SZ, (BYTE*)szServiceDll, lstrlen(szServiceDll)*2) == ERROR_SUCCESS)
				result = TRUE;
			RegCloseKey(hParKey);
		}
		RegCloseKey(hKey);
	}
	return result;
}

int main()
{
	WCHAR szServiceName[] = L"SvcHostDemo";
	WCHAR dllName[] = L"SvcHostDemo.dll";
	WCHAR szDescription[] = L"SvcHostDemo description";

	BOOL install = installService(szServiceName, dllName, szDescription);
	if (install)
	{
		printf("[+] Installed\n");
		SC_HANDLE hService, hSCM;
		hSCM = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
		if (hSCM)
		{
			printf("[+] OpenSCManager\n");
			hService = CreateService(
				hSCM,
				szServiceName,
				szServiceName,
				SERVICE_ALL_ACCESS, SERVICE_WIN32_SHARE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE,
				L"\%SystemRoot\%\\system32\\svchost.exe -k netsvcs", 0, 0, 0, 0, 0);

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

	system("pause");
}

