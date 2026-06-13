#include <ntifs.h>
#include <string.h>
#include <Ntstrsafe.h>

#define deviceName      L"\\Device\\Terminator"
#define symbolicName    L"\\DosDevices\\Terminator"

#define IOCTL_TERMINATE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

PDEVICE_OBJECT deviceObject;
UNICODE_STRING  SymbolicLinkName;

void Unload(PDRIVER_OBJECT pDriverObject)
{
    DbgPrint("Terminator has unloaded!\n");
    IoDeleteSymbolicLink(&SymbolicLinkName);
    IoDeleteDevice(pDriverObject->DeviceObject);
}

// x Terminator!Terminate
NTSTATUS Terminate(DWORD pid) {
    CLIENT_ID clientId = { (HANDLE)(ULONG_PTR)pid, NULL };
    OBJECT_ATTRIBUTES objAttr = { sizeof(OBJECT_ATTRIBUTES) };
    HANDLE hProcess = NULL;
    NTSTATUS status;

    DbgPrint("Current PID is %lu\n", pid);

    status = ZwOpenProcess(&hProcess, 0x0001, &objAttr, &clientId); // 0x0001 == PROCESS_TERMINATE
    if (NT_SUCCESS(status)) {
        ZwTerminateProcess(hProcess, STATUS_SUCCESS);
        ZwClose(hProcess);
    }

    return status;
}

NTSTATUS CtlPass(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS CtlDriverDispatch(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
    UNREFERENCED_PARAMETER(pDeviceObject);
    PIO_STACK_LOCATION stack;
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;

    stack = IoGetCurrentIrpStackLocation(Irp);

    switch (stack->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_TERMINATE:
        if (stack->Parameters.DeviceIoControl.InputBufferLength >= sizeof(DWORD)) {
            DWORD* pid = (DWORD*)Irp->AssociatedIrp.SystemBuffer;
            status = Terminate(*pid);
        }
        else {
            status = STATUS_INVALID_BUFFER_SIZE;
        }
        break;
    default:
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

/*
    Ring3 API -> IRP_MJ_* mapping

    CreateFileA/W, NtCreateFile                         -> IRP_MJ_CREATE
    CloseHandle, NtClose                                -> IRP_MJ_CLOSE
    ReadFile, NtReadFile                                -> IRP_MJ_READ
    WriteFile, NtWriteFile                              -> IRP_MJ_WRITE
    DeviceIoControl, NtDeviceIoControlFile              -> IRP_MJ_DEVICE_CONTROL
    FlushFileBuffers, NtFlushBuffersFile                -> IRP_MJ_FLUSH_BUFFERS
    GetFileInformationByHandle, NtQueryInformationFile  -> IRP_MJ_QUERY_INFORMATION
    SetFileInformationByHandle, NtSetInformationFile    -> IRP_MJ_SET_INFORMATION
    LockFileEx, NtLockFile                              -> IRP_MJ_LOCK_CONTROL
    UnlockFileEx, NtUnlockFile                          -> IRP_MJ_LOCK_CONTROL
    NtQuerySecurityObject                               -> IRP_MJ_QUERY_SECURITY
    NtSetSecurityObject                                 -> IRP_MJ_SET_SECURITY
*/

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
    DbgPrint("Terminator has loaded!\n");
	UNREFERENCED_PARAMETER(pRegistryPath);
    int i;
    NTSTATUS status;
    UNICODE_STRING DeviceName;

    PDRIVER_DISPATCH* pDispacher;
    pDispacher = pDriverObject->MajorFunction;

    RtlInitUnicodeString(&DeviceName, deviceName);
    RtlInitUnicodeString(&SymbolicLinkName, symbolicName);

    status = IoCreateDevice(pDriverObject, 0, &DeviceName, FILE_DEVICE_NULL, 0, FALSE, &deviceObject);
    if (status == STATUS_SUCCESS)
    {
        deviceObject->Flags |= DO_BUFFERED_IO;

        status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);

        for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
            pDriverObject->MajorFunction[i] = CtlPass;

        pDispacher[IRP_MJ_DEVICE_CONTROL] = CtlDriverDispatch;
        pDriverObject->DriverUnload = Unload;
    }

    return STATUS_SUCCESS;
}

