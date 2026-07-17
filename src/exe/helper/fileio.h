#pragma once

//#include <fileapi.h>
#include <strsafe.h>

#include "../../common/kfileio.h"


//int openFile(
//    _Out_ PHANDLE File, 
//    _In_ PWCHAR FileName, 
//    _In_ ACCESS_MASK DesiredAccess, 
//    _In_ ULONG ShareAccess
//)
//{
//    NTSTATUS status = 0;
//    OBJECT_ATTRIBUTES objAttr = { 0 };
//    UNICODE_STRING fileNameUS;
//    IO_STATUS_BLOCK iostatusblock = { 0 };
//
//    RtlInitUnicodeString(&fileNameUS, FileName);
//    objAttr.Length = sizeof(objAttr);
//    objAttr.ObjectName = &fileNameUS;
//    
//    *File = NULL;
//
//    status = NtCreateFile(
//                File,
//                DesiredAccess,
//                &objAttr,
//                &iostatusblock,
//                NULL,
//                FILE_ATTRIBUTE_NORMAL,
//                ShareAccess,
//                FILE_OPEN,
//                FILE_NON_DIRECTORY_FILE|FILE_SYNCHRONOUS_IO_NONALERT,
//                NULL, 
//                0
//            );
//    if ( status != 0 )
//    {
//        EPrint("Open file failed! (0x%x) [%s].\n", status, getStatusString(status));
//        return status;
//    }
//
//    return 0;
//}

INT parseFile(_In_ PCHAR FilePath, _Out_ PVOID *Buffer, _Out_ PULONG Size)
{
    INT s = 0;
    //BOOL b = FALSE;
    
    PWCHAR ntPath = NULL;
    HANDLE file = NULL;

    WCHAR filePathW[MAX_PATH];
    ULONG cch = 0;
    ULONG cb = 0;

    PVOID buffer = NULL;
    ULONG size = 0;

    *Buffer = NULL;
    *Size = 0;

    StringCchPrintfW(filePathW, MAX_PATH, L"%hs", FilePath);
    cch = GetFullPathNameW(filePathW, 0, NULL, NULL);
    if ( cch == 0 )
    {
        s = GetLastError();
        EPrint("Getting full path failed! (0x%x)\n", s);
        goto clean;
    }
    cch += 4; // nt prefix
    cb = cch * 2;
    ntPath = (PWCHAR)malloc(cb);
    if ( !ntPath )
    {
        s = ERROR_NO_SYSTEM_RESOURCES;
        EPrint("No memory for path!\n");
        goto clean;
    }
    cch = GetFullPathNameW(filePathW, cch-4, &ntPath[4], NULL);
    if ( cch == 0 )
    {
        s = GetLastError();
        EPrint("Getting full path failed! (0x%x)\n", s);
        goto clean;
    }
    *(PUINT64)ntPath = NT_PATH_PREFIX_W;

    cch = (ULONG)wcslen(ntPath);
    //s = openFile(&file, ntPath, FILE_GENERIC_READ, FILE_SHARE_READ);
    s = kOpenFile(ntPath, cch, &file, OPEN_FOR_READ_ONLY, FILE_GENERIC_READ, FILE_SHARE_READ);
    if ( s != 0 )
    {
        goto clean;
    }

    LARGE_INTEGER fileSize = {0};
    fileSize.QuadPart = kGetFileSize(file);
    if ( !fileSize.QuadPart )
    {
        s = GetLastError();
        EPrint("Getting file size failed! (0x%x)\n", s);
        goto clean;
    }
    if ( fileSize.HighPart )
    {
        s = ERROR_INVALID_PARAMETER;
        EPrint("File too big! (0x%x)\n", s);
        goto clean;
    }
    //b = GetFileSizeEx(file, &fileSize);
    //if ( !b || !fileSize.QuadPart )
    //{
    //    s = GetLastError();
    //    EPrint("Getting file size failed! (0x%x)\n", s);
    //    goto clean;
    //}
    //if ( fileSize.HighPart )
    //{
    //    s = ERROR_INVALID_PARAMETER;
    //    EPrint("File too big! (0x%x)\n", s);
    //    goto clean;
    //}

    buffer = malloc(fileSize.LowPart);
    if ( !buffer )
    {
        s = ERROR_NO_SYSTEM_RESOURCES;
        EPrint("No memory for file data!\n");
        goto clean;
    }
    size = fileSize.LowPart;
            
    //IO_STATUS_BLOCK iosb;
    //RtlZeroMemory(&iosb, sizeof(iosb));
    
    //s = NtReadFile(file, NULL, NULL, NULL, &iosb, buffer, size, NULL, NULL);
    s = kReadFile(file, buffer, size, &size);
    if ( s != 0 ) 
    {
        EPrint("Reading file data failed! (0x%x)\n", s);
        goto clean;
    }
    //size = (ULONG) iosb.Information;
    
    *Buffer = buffer;
    *Size = size;

clean:
    if ( s != 0 )
        free(buffer);

    if ( ntPath )
        free(ntPath);
    if ( file )
        NtClose(file);

    return s;
}

INT fillNtPath(_In_ PCHAR Path, _Out_ PWCHAR* NtPath)
{
    INT s;

    ULONG pathCch = (ULONG)strlen(Path);
    ULONG ntPathCb = pathCch * 2;
    PWCHAR pathW = NULL;

    *NtPath = NULL;

    pathW = malloc(ntPathCb + 2);
    if ( !pathW )
    {
        s = GetLastError();
        goto clean;
    }
    s = RtlStringCchPrintfW(pathW, pathCch+1, L"%hs", Path);
    if ( s != S_OK )
        goto clean;

    ntPathCb = RtlGetFullPathName_U(pathW, 0, NULL, NULL);
    ntPathCb += 8; // nt prefix w (8)
    *NtPath = (PWCHAR)malloc(ntPathCb); // L'0' (2)
    if ( *NtPath == NULL )
    {
        s = ERROR_NOT_ENOUGH_MEMORY;
        EPrint("Not enough memory for path\n");
        goto clean;
    }

    s = ntGetFullPathName(pathW, ntPathCb, *NtPath, NULL);
    if ( s == 0 || s >= (INT)ntPathCb )
    {
        s = ERROR_INVALID_PARAMETER;
        EPrint("ntGetFullPathName failed! (0x%x)\n", s);
        goto clean;
    }
    s = 0;

clean:
    if ( pathW )
        free(pathW);

    return s;
}
