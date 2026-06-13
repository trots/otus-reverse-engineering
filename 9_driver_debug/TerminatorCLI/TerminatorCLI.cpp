#include "windows.h"
#include "stdio.h"

#pragma comment(lib, "psapi.lib")

#define IOCTL_TERMINATE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

int main(int argc, char* argv[]) {

    DWORD bytesReturned;
    DWORD userPID = 0xf1f2;

    if (argc >= 2) {
        userPID = strtoul(argv[1], NULL, 0);
    }

    HANDLE hDevice = CreateFileA(
        "\\\\.\\Terminator",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("[-] Failed to open device. Exit...\n");
        ExitProcess(1);
    }

    if (DeviceIoControl(
        hDevice,
        IOCTL_TERMINATE,
        &userPID,
        sizeof(userPID),
        NULL,
        0,
        &bytesReturned,
        NULL))
    {
        printf("[+] Request sent! PID=%u\n", userPID);
    }
    else {
        printf("[-] DeviceIoControl failed! Error=%lu\n", GetLastError());
    }

    CloseHandle(hDevice);
    return 0;
}
