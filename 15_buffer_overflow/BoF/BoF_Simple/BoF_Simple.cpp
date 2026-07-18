// BoF_Simple.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <winsock.h>
#include <stdio.h>
#pragma comment (lib, "ws2_32.lib")

#define PORT 4444


/*
*packet = datasize + data
*/
void vuln(char *buf)
{
	LPVOID data;
	DWORD size;
	char localBuf[500];
	size = *(DWORD*)buf;
	data = buf + 4;
	printf("size: %d\nbuf: %s\n",size, data);
	memcpy(localBuf, data, size);
}

int main()
{
	//char *shell = "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2AAa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A";
	
	//printf("0x%08x\n", SECURITY_SQOS_PRESENT);

	char buf[0x1000];
	WSADATA wsaData;
	SOCKET s, client_sock;
	sockaddr_in serv_addr, client_addr;

	WSAStartup(0x202, &wsaData);		
	s = socket(AF_INET, SOCK_STREAM, 0);
		
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = 0; // слушаем все IP с указанного порта связывание (bind)
	bind(s, (sockaddr*)&serv_addr, sizeof(serv_addr));


	listen(s, 1);
		
	client_sock = accept(s, NULL, NULL);
	int msize = recv(client_sock, buf, sizeof(buf), 0);
	
	vuln(buf);	
	WSACleanup();

	system("pause");
    return 0;
}

