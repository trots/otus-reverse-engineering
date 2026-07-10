#pragma once
#include <Windows.h>

#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

SERVICE_STATUS serviceStatus =
{
	SERVICE_WIN32_SHARE_PROCESS,
	SERVICE_START_PENDING,
	SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE
};

SERVICE_STATUS_HANDLE serviceStatusHandle = NULL;
HANDLE hWorkThread = NULL;

void Log(char *message)
{
	FILE *file;

	file = fopen("C:\\MyService_log.txt", "a+");

	fputs(message, file);
	fclose(file);
}

void WriteStartFile()
{
	FILE* file;

	file = fopen("C:\\serv.log", "a+");

	fputs("MyService start log", file);
	fclose(file);
}
