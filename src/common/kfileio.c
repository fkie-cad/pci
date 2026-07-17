#include "kfileio.h"

#ifdef RING3
#undef OBJ_KERNEL_HANDLE
#define OBJ_KERNEL_HANDLE (0)

#define KeGetCurrentIrql() 0

//#define PAGED_CODE()NTSYSAPI
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSYSAPI
NTSTATUS
NTAPI
ZwDeleteFile(
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
    );
#endif

//
// \CreateDisposition individual CreateDisposition overwriting the default one
NTSTATUS kOpenFile(PWCHAR FileName, ULONG FilenameCch, PHANDLE Handle, ULONG OpenMode, ULONG CreateDisposition, ULONG Share)
{
    PAGED_CODE();

    NTSTATUS status = STATUS_SUCCESS;
    HANDLE file;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK iosb;
    UNICODE_STRING filenameUcs;
    
    RtlZeroMemory(&oa, sizeof(oa));
    RtlZeroMemory(&iosb, sizeof(iosb));
    
    *Handle = NULL;

    filenameUcs.Buffer = FileName;
    filenameUcs.Length = (USHORT)(FilenameCch<<1);
    filenameUcs.MaximumLength = filenameUcs.Length;
    
    InitializeObjectAttributes( &oa, 
                                &filenameUcs, 
                                OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 
                                NULL, NULL);

    if ( OpenMode == OPEN_FOR_READ_ONLY )
    {
        if ( CreateDisposition == 0 )
            CreateDisposition = FILE_OPEN; // no create
        status = ZwCreateFile(  &file, 
                                FILE_GENERIC_READ | SYNCHRONIZE, 
                                &oa, &iosb, 
                                NULL, 
                                0, 
                                FILE_SHARE_READ, 
                                CreateDisposition,
                                FILE_SYNCHRONOUS_IO_NONALERT, 
                                NULL, 0);
    }
    else if ( OpenMode == OPEN_FOR_WRITE_ONLY )
    {
        if ( CreateDisposition == 0 )
            CreateDisposition = FILE_OPEN_IF;

        status = ZwCreateFile(  &file, 
                                FILE_GENERIC_WRITE | SYNCHRONIZE, 
                                &oa, &iosb, 
                                NULL,
                                FILE_ATTRIBUTE_NORMAL, 
                                Share, 
                                CreateDisposition, 
                                FILE_SYNCHRONOUS_IO_NONALERT, 
                                NULL, 0);
    }
    else if ( OpenMode == OPEN_FOR_READWRITE )
    {
        if ( CreateDisposition == 0 )
            CreateDisposition = FILE_OPEN_IF;

        status = ZwCreateFile(  &file, 
                                FILE_GENERIC_READ | FILE_GENERIC_WRITE | SYNCHRONIZE, 
                                &oa, &iosb, 
                                NULL,
                                FILE_ATTRIBUTE_NORMAL, 
                                Share, 
                                CreateDisposition,
                                FILE_SYNCHRONOUS_IO_NONALERT, 
                                NULL, 0);
    }
    else if ( OpenMode == OPEN_FOR_APPEND )
    {
        if ( CreateDisposition == 0 )
            CreateDisposition = FILE_OPEN_IF;

        status = ZwCreateFile(  &file, 
                                FILE_APPEND_DATA | SYNCHRONIZE, 
                                &oa, &iosb, 
                                NULL,
                                FILE_ATTRIBUTE_NORMAL, 
                                Share, 
                                CreateDisposition, 
                                FILE_SYNCHRONOUS_IO_NONALERT, 
                                NULL, 0);
    }
    else if ( OpenMode == OPEN_DIR )
    {
        if ( CreateDisposition == 0 )
            CreateDisposition = FILE_OPEN;
        status = ZwCreateFile(  &file, 
                                SYNCHRONIZE, 
                                &oa, &iosb, 
                                NULL,
                                FILE_ATTRIBUTE_NORMAL, 
                                FILE_SHARE_READ, 
                                CreateDisposition,
                                FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, 
                                NULL, 0);
    }
    else if ( OpenMode == CREATE_DIR )
    {
        if ( CreateDisposition == 0 )
            CreateDisposition = FILE_OPEN_IF;
        status = ZwCreateFile(  &file, 
                                SYNCHRONIZE, 
                                &oa, &iosb, 
                                NULL,
                                FILE_ATTRIBUTE_NORMAL, 
                                0, 
                                CreateDisposition,
                                FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, 
                                NULL, 0);
    }
    else
    {
        return STATUS_NOT_SUPPORTED;
    }

    if ( status == 0 )
        *Handle = file;
    
    return status;
}

NTSTATUS kDeleteFile(PWCHAR FileName)
{
    PAGED_CODE();
    NTSTATUS status = STATUS_SUCCESS;
    OBJECT_ATTRIBUTES objAttr;
    UNICODE_STRING uni_Filename;
    
    RtlZeroMemory(&objAttr, sizeof(objAttr));
    
    RtlInitUnicodeString(&uni_Filename, FileName);
    
    InitializeObjectAttributes( &objAttr, 
                                &uni_Filename, 
                                OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 
                                NULL, NULL);

    status = ZwDeleteFile(&objAttr);

    return status;
}

NTSTATUS kCloseFile(HANDLE Handle)
{ 
    PAGED_CODE();
    return ZwClose(Handle);
}


// Note: this function can't return an ntstatus by design.
// On error GetFileSize returns 0.
unsigned __int64 kGetFileSize(HANDLE Handle)
{
    PAGED_CODE();
    NTSTATUS status = STATUS_SUCCESS;
    IO_STATUS_BLOCK iosb;
    FILE_STANDARD_INFORMATION fi;
    
    RtlZeroMemory(&iosb, sizeof(iosb));

    status = ZwQueryInformationFile(Handle, &iosb, (PVOID) &fi, sizeof(fi), FileStandardInformation);
    
    if (!NT_SUCCESS(status)) 
    {
        return 0;
    }

    return fi.EndOfFile.QuadPart;
}


NTSTATUS kReadFile(HANDLE Handle, PVOID Buffer, ULONG Size, PULONG pBytesRead)
{
    PAGED_CODE();
    NTSTATUS status;
    IO_STATUS_BLOCK iosb;
    RtlZeroMemory(&iosb, sizeof(iosb));
    
    status = ZwReadFile(Handle, NULL, NULL, NULL, &iosb, Buffer, Size, NULL, NULL);
    
    if (!NT_SUCCESS(status)) return status;
    if (NT_SUCCESS(iosb.Status)) *pBytesRead = (ULONG) iosb.Information;
    return iosb.Status;
}

NTSTATUS kReadFileAtOffset(HANDLE Handle, PVOID Buffer, ULONG Size, PULONG pBytesRead, PLARGE_INTEGER Offset)
{
    PAGED_CODE();
    NTSTATUS status;
    IO_STATUS_BLOCK iosb;
    RtlZeroMemory(&iosb, sizeof(iosb));
    
    status = ZwReadFile(Handle, NULL, NULL, NULL, &iosb, Buffer, Size, Offset, NULL);
    
    if (!NT_SUCCESS(status)) return status;
    if (NT_SUCCESS(iosb.Status)) *pBytesRead = (ULONG) iosb.Information; // iosb.Information is actually type SIZE_T.
    return iosb.Status;
}


NTSTATUS kWriteFile(HANDLE Handle, PVOID Buffer, ULONG Size, PULONG pBytesWritten)
{
    PAGED_CODE();
    NTSTATUS status;
    IO_STATUS_BLOCK iosb;
    RtlZeroMemory(&iosb, sizeof(iosb));
    
    status = ZwWriteFile(Handle, NULL, NULL, NULL, &iosb, Buffer, Size, NULL, NULL);
    
    //DbgPrint("WriteFile(%p, %p, 0x%x, 0x%x): status: 0x%x, iosb.status: 0x%x\n", Handle, Buffer, Size, pBytesWritten, status, iosb.Status);
    if (!NT_SUCCESS(status)) return status;
    if (NT_SUCCESS(iosb.Status)) *pBytesWritten = (ULONG) iosb.Information;  // iosb.Information is actually type SIZE_T.
    return iosb.Status;
}

//
// Write / append modes:
// If the call to NtCreateFile set either of the CreateOptions flags, 
// - FILE_SYNCHRONOUS_IO_ALERT or 
// - FILE_SYNCHRONOUS_IO_NONALERT, 
// the I/O Manager maintains the current file position. 
// If so, the caller of NtWriteFile can specify that the current file position offset be used instead of an explicit ByteOffset value. 
// This specification can be made by using one of the following methods:
// * Specify a pointer to a LARGE_INTEGER value with the HighPart member set to -1 and the LowPart member set to the system-defined value FILE_USE_FILE_POINTER_POSITION.
// * Pass a NULL pointer for ByteOffset. (<= done here =>)
// NtWriteFile updates the current file position by adding the number of bytes written when it completes the write operation, 
// if it is using the current file position maintained by the I/O Manager.
// Even when the I/O Manager is maintaining the current file position, 
// the caller can reset this position by passing an explicit ByteOffset value to NtWriteFile.
//
NTSTATUS kWriteFileAtOffset(HANDLE Handle, PVOID Buffer, ULONG Size, PULONG pBytesWritten, PLARGE_INTEGER Offset)
{
    PAGED_CODE();
    NTSTATUS status;
    IO_STATUS_BLOCK iosb;
    RtlZeroMemory(&iosb, sizeof(iosb));
    
    status = ZwWriteFile(Handle, NULL, NULL, NULL, &iosb, Buffer, Size, Offset, NULL);

    //DbgPrint("WriteFileAtOffset(%p, %p, 0x%x, 0x%x, 0x%llx): status: 0x%x, iosb.status: 0x%x\n", Handle, Buffer, Size, pBytesWritten, Offset->QuadPart, status, iosb.Status);
    if (!NT_SUCCESS(status)) return status;
    if (NT_SUCCESS(iosb.Status)) *pBytesWritten = (ULONG) iosb.Information;  // iosb.Information is actually type SIZE_T.
    return iosb.Status;
}

/**
 * Internal function, wrapped by k(Dir|Path)Exists.
 * 
 * Its a normal file if existing and no dir?
 */
BOOLEAN kPathExists(_In_ WCHAR* Path, _In_ ULONG PathCch, _In_ BOOLEAN IsDir);
#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, kPathExists)
#endif
BOOLEAN kPathExists(_In_ WCHAR* Path, _In_ ULONG PathCch, _In_ BOOLEAN IsDir)
{
    PAGED_CODE();
    FILE_NETWORK_OPEN_INFORMATION info;
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    OBJECT_ATTRIBUTES objAttr;
    UNICODE_STRING uName;
    ULONG fileAttr = 0;

    if ( NULL == Path )
        return FALSE;
    
    RtlZeroMemory(&uName, sizeof(UNICODE_STRING));
    RtlZeroMemory(&info, sizeof(FILE_NETWORK_OPEN_INFORMATION));
    RtlZeroMemory(&objAttr, sizeof(OBJECT_ATTRIBUTES));

    uName.Buffer = Path;
    uName.Length = (USHORT)(PathCch<<1);
    uName.MaximumLength = uName.Length;

    InitializeObjectAttributes(
        &objAttr, 
        &uName, 
        OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, 
        NULL, 
        NULL
    );

    status = ZwQueryFullAttributesFile(
                    &objAttr, 
                    &info
                );

    if ( !NT_SUCCESS(status) )
    {
        return FALSE;
    }

    fileAttr = info.FileAttributes;
    //DbgPrint("%ws has attributes 0x%x \n", Path, fileAttr);
     
    //if (fileAttr & FILE_ATTRIBUTE_ARCHIVE)
    //{
    //    DbgPrint("%S is an archive \n", filename);
    //    return TRUE;
    //}
    if ( IsDir )
    {
        return (fileAttr & FILE_ATTRIBUTE_DIRECTORY) > 0;
    }
    else
    {
        return !((fileAttr & FILE_ATTRIBUTE_DIRECTORY));
    }
}

// convenience wrapper for kPathExists
BOOLEAN kDirExists(_In_ WCHAR* Path, _In_ ULONG PathCch)
{
    PAGED_CODE();
    return kPathExists(Path, PathCch, TRUE);
}

// convenience wrapper for kPathExists
BOOLEAN kFileExists(_In_ WCHAR* Path, _In_ ULONG PathCch)
{
    PAGED_CODE();
    return kPathExists(Path, PathCch, FALSE);
}
