#pragma once



int openDevice(
    _Out_ PHANDLE Device, 
    _In_ PWCHAR Name
)
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES objAttr = { 0 };
    UNICODE_STRING devname;
    IO_STATUS_BLOCK iosb;

    WCHAR dev_name[MAX_PATH];
    swprintf_s(dev_name, MAX_PATH, L"\\Device\\%s", Name);
    dev_name[MAX_PATH-1] = 0;

    *Device = NULL;

    RtlInitUnicodeString(&devname, dev_name);
    objAttr.Length = sizeof( objAttr );
    objAttr.ObjectName = &devname;

    status = NtOpenFile(Device, 
                        FILE_GENERIC_READ|FILE_GENERIC_WRITE, 
                        &objAttr, 
                        &iosb,
                        0, 
                        FILE_NON_DIRECTORY_FILE|FILE_SYNCHRONOUS_IO_NONALERT);
    if ( !NT_SUCCESS(status) )
    {
        EPrint("Open device \"%ws\" failed. (0x%08x)\n", Name, status);
        return status;
    }

    //printf("device: 0x%p\n", *device);
    //printf("\n");

    return 0;
}

const char* getStatusString(NTSTATUS status)
{
    switch (status )
    {
    case STATUS_UNSUCCESSFUL:
        return "STATUS_UNSUCCESSFUL";
    case STATUS_NOT_IMPLEMENTED:
        return "STATUS_NOT_IMPLEMENTED";
    case STATUS_ACCESS_DENIED:
        return "STATUS_ACCESS_DENIED";
    case STATUS_OBJECT_TYPE_MISMATCH:
        return "STATUS_OBJECT_TYPE_MISMATCH";
    case STATUS_ACCESS_VIOLATION:
        return "STATUS_ACCESS_VIOLATION";
    case STATUS_NO_SUCH_DEVICE:
        return "STATUS_NO_SUCH_DEVICE";
    case STATUS_INVALID_PARAMETER:
        return "STATUS_INVALID_PARAMETER";
    case STATUS_NO_SUCH_FILE:
        return "STATUS_NO_SUCH_FILE";
    case STATUS_INVALID_DEVICE_REQUEST:
        return "STATUS_INVALID_DEVICE_REQUEST: The specified request is not a valid operation for the target device";
    case STATUS_ILLEGAL_FUNCTION:
        return "STATUS_ILLEGAL_FUNCTION: kernel driver is irritated";
    case STATUS_INVALID_HANDLE:
        return "STATUS_INVALID_HANDLE";
    case STATUS_OBJECT_PATH_SYNTAX_BAD:
        return "STATUS_OBJECT_PATH_SYNTAX_BAD";
    case STATUS_DATATYPE_MISALIGNMENT_ERROR:
        return "STATUS_DATATYPE_MISALIGNMENT_ERROR: A data type misalignment error was detected in a load or store instruction";
    case STATUS_NOT_SUPPORTED:
        return "STATUS_NOT_SUPPORTED: The request is not supported";
    case STATUS_OBJECT_NAME_INVALID:
        return "STATUS_OBJECT_NAME_INVALID";
    case STATUS_OBJECT_PATH_INVALID:
        return "STATUS_OBJECT_PATH_INVALID";
    case STATUS_OBJECT_NAME_NOT_FOUND:
        return "STATUS_OBJECT_NAME_NOT_FOUND";
    case STATUS_OBJECT_NAME_COLLISION:
        return "STATUS_OBJECT_NAME_COLLISION";
    case STATUS_OBJECT_PATH_NOT_FOUND:
        return "STATUS_OBJECT_PATH_NOT_FOUND";
    case STATUS_ACPI_INVALID_INDEX:
        return "STATUS_ACPI_INVALID_INDEX";
    case STATUS_ACPI_INVALID_ARGTYPE:
        return "STATUS_ACPI_INVALID_ARGTYPE";
    case STATUS_ACPI_INCORRECT_ARGUMENT_COUNT:
        return "STATUS_ACPI_INCORRECT_ARGUMENT_COUNT";
    case STATUS_ACPI_INVALID_DATA:
        return "STATUS_ACPI_INVALID_DATA";

    default:
        return "unknown";
    }
}

NTSTATUS generateIoRequest(
    _In_ HANDLE Device, 
    _In_ ULONG Ioctl, 
    _In_ PIO_STATUS_BLOCK Iosb,
    _In_opt_ PVOID InputBuffer, 
    _In_ ULONG InputBufferSize, 
    _Inout_opt_ PVOID OutputBuffer, 
    _In_ ULONG OutputBufferSize
)
{
    NTSTATUS status = STATUS_SUCCESS;

    HANDLE event = NULL;
    
    //DPrint("Launching IOCTL 0x%x\n", Ioctl);
    
    status = NtCreateEvent(&event,
                           FILE_ALL_ACCESS,
                           0,
                           0,
                           0);
    if ( status != STATUS_SUCCESS )
    {
        EPrint("Create event failed. (0x%08x)\n", status);
        goto clean;
    }

    RtlZeroMemory(Iosb, sizeof(*Iosb));

    status = NtDeviceIoControlFile(Device, 
                                   event, 
                                   NULL, 
                                   NULL, 
                                   Iosb, 
                                   Ioctl, 
                                   InputBuffer, 
                                   InputBufferSize, 
                                   OutputBuffer,
                                   OutputBufferSize);

    //DPrint("status: 0x%x\n", status);
    //DPrint("iosb.Status: 0x%x\n", Iosb->Status);
    //DPrint("iosb.Information: 0x%llx\n", Iosb->Information);

    if ( status == STATUS_PENDING )
    {
        status = NtWaitForSingleObject(event, 0, 0);
        if ( status == 0 )
            status = Iosb->Status;

    }
    
    if ( status != STATUS_SUCCESS )
    {
        EPrint("NtDeviceIoControlFile failed! (0x%x)\n", status);
        DPrint("    %s\n", getStatusString(status));
    }

clean:
    if ( event )
        NtClose(event);

    return status;
}
