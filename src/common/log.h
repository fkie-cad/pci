#pragma once

#include <ntifs.h>


#include "kfileio.h"
#include "../print.h"


#ifdef RING3 

#define KeGetCurrentIrql() 0 // FlushLogFileBuffer needs this 

//warning C4005: 'ExFreePool': macro redefinition
#pragma warning ( disable : 4005 )
#define ExAllocatePoolWithTag(_pt_, _n_, _t_) malloc(_n_)
// #define ExFreePoolWithTag(_p_, _t_) free(_p_)
#define ExFreePool(_p_) free(_p_)
#pragma warning ( default : 4005 )

#endif


typedef struct _LOG {
    PWCHAR Dir;
    SIZE_T BufferSize;
    PCHAR Buffer;
    HANDLE File;
} LOG, *PLOG;




NTSTATUS LogBytes(
    _In_opt_ PCHAR Label, 
    _In_opt_ PCHAR Prefix, 
    _In_ PVOID Buffer,
    _In_ ULONG BufferSize,
    _In_opt_ PVOID Address,
    _In_ BOOLEAN flush,
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
);

NTSTATUS createLogDir(
    _In_ PWCHAR Path
);

NTSTATUS FlushLogFileBuffer(
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
);

NTSTATUS getLogFilePath(
    _Out_ PWCHAR *LogFilePath,
    _In_ PWCHAR ParentDir,
    _In_opt_ PWCHAR Prefix,
    _In_ PWCHAR Name,
    _In_ PWCHAR Type
);

NTSTATUS openLogFileEx(
    _Out_ PVOID* Buffer,
    _In_ ULONG BufferSize,
    _Out_ PHANDLE File,
    _In_ PWCHAR Path,
    _In_ ULONG OpenMode
);



NTSTATUS createLogDir(
    _In_ PWCHAR Path
)
{
    NTSTATUS status = STATUS_SUCCESS;

    ULONG pathCb = (ULONG)wcslen(Path);
    if ( !pathCb )
        return STATUS_INVALID_PARAMETER;

    HANDLE dir = NULL;
    status = kOpenFile(Path, pathCb, &dir, CREATE_DIR, 0, 0);
    if ( !NT_SUCCESS(status) )
    {
        EPrint("Create directory failed. (0x%x)\n", status);
        goto clean;
    }
    ZwClose(dir);
    
clean:
    return status;
}

#define CLOSE_LOG_FILE(__logFile__, __logBuffer__) \
{ \
    if ( __logFile__ ) \
        ZwClose(__logFile__); \
    __logFile__ = NULL; \
    if ( __logBuffer__ ) \
        ExFreePool(__logBuffer__); \
    __logBuffer__ = NULL; \
}

NTSTATUS getLogFilePath(
    _Out_ PWCHAR *LogFilePath,
    _In_ PWCHAR ParentDir,
    _In_opt_ PWCHAR Prefix,
    _In_ PWCHAR Name,
    _In_ PWCHAR Type
)
{
    //PAGED_CODE();

    NTSTATUS status = STATUS_SUCCESS;

    ULONG pathCch = 0;
    ULONG prefixCch = (Prefix)?(ULONG)wcslen(Prefix):0;
    ULONG typeCch = (Type)?(ULONG)wcslen(Type):0;
    ULONG parentCch = (ParentDir)?(ULONG)wcslen(ParentDir):0;

    PWCHAR pathBuffer = NULL;
    ULONG pathBufferSize = 0;
    ULONG restSize = 0;
    PWCHAR namePtr = NULL;
    PWCHAR pathPtr = NULL;

    *LogFilePath = NULL;

    FEnter();

    // + 1 for parent backslash + 1 for prefix separator
    pathCch = (ULONG)(parentCch + 2 + wcslen(Name) + prefixCch + typeCch);
    pathBufferSize = ( pathCch * 2 ) + 2; // + 2 for \0

    pathBuffer = (PWCHAR)ExAllocatePoolWithTag(PagedPool, pathBufferSize, POOL_TAG+10);
    if ( !pathBuffer )
    {
        status = STATUS_NO_MEMORY;
        EPrint("No memory for path buffer! (0x%x)\n", status);
        return status;
    }

    RtlStringCbPrintfW(pathBuffer, pathBufferSize, L"%ws\\", ParentDir);
    
    restSize = pathBufferSize - ((parentCch+1)*2);
    namePtr = Name;
    pathPtr = &pathBuffer[parentCch+1];

    if ( Prefix )
    {
        RtlStringCbPrintfW(pathPtr, restSize, L"%ws-", Prefix);
        restSize -= ((prefixCch+1)*2);
        pathPtr += (prefixCch+1);
    }

    // skip initial slashes
    while ( *namePtr == L'\\' || *namePtr == L'/' )
        namePtr++;

    // copy rest of name and convert inner slashes
    while ( *namePtr != 0 )
    {
        if ( *namePtr == L'\\' || *namePtr == L'/' )
        {
            *pathPtr = L'-';
        }
        else
        {
            *pathPtr = *namePtr;
        }

        ++namePtr;
        ++pathPtr;
    }

    memcpy(pathPtr, Type, typeCch*2);
    pathPtr[typeCch] = 0;

    DPrint("path: %ws\n", pathBuffer);

    *LogFilePath = pathBuffer;

    FLeave();
    return status;
}

NTSTATUS openLogFileEx(
    _Out_ PVOID* Buffer,
    _In_ ULONG BufferSize,
    _Out_ PHANDLE File,
    _In_ PWCHAR Path,
    _In_ ULONG OpenMode
)
{
    //PAGED_CODE();

    NTSTATUS status = STATUS_SUCCESS;
    ULONG openMode = FILE_OVERWRITE_IF;

    if ( openMode )
        openMode = OpenMode;

    *Buffer = NULL;
    *File = NULL;

    ULONG pathCch = (ULONG)wcslen(Path);
    ULONG dirPathCch = pathCch;
    PWCHAR pathPtr = Path + dirPathCch - 1;
    while ( pathPtr != Path && *pathPtr != L'\\' )
    {
        --dirPathCch;
        --pathPtr;
    }

    if ( !dirPathCch )
        return STATUS_INVALID_PARAMETER;
    
    if ( !kDirExists(Path, dirPathCch) )
    {
        HANDLE dir = NULL;
        status = kOpenFile(Path, dirPathCch, &dir, CREATE_DIR, 0, 0);
        if ( !NT_SUCCESS(status) )
        {
            EPrint("Create target directory failed. (0x%x)\n", status);
            return status;
        }
        ZwClose(dir);
    }
    
    *Buffer = ExAllocatePoolWithTag(PagedPool, BufferSize, POOL_TAG);
    if ( !(*Buffer) )
    {
        status = STATUS_NO_MEMORY;
        EPrint("No logBuffer memory! (0x%x)\n", status);
        return status;
    }

    status = kOpenFile(Path, pathCch, File, OPEN_FOR_APPEND, openMode, FILE_SHARE_READ);
    if ( !NT_SUCCESS(status) )
    {
        EPrint("Opening log file \"%ws\" failed! (0x%x)\n", Path, status);
        return status;
    }

    return status;
}
//#pragma warning ( default : 6014 )

NTSTATUS LogBytes(
    _In_opt_ PCHAR Label, 
    _In_opt_ PCHAR Prefix, 
    _In_ PVOID Buffer,
    _In_ ULONG BufferSize,
    _In_opt_ PVOID Address,
    _In_ BOOLEAN flush,
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
)
{
    //PAGED_CODE();

    NTSTATUS status = STATUS_SUCCESS;

    PUINT8 buffer = (PUINT8)Buffer;

    CHAR digitMap[] = "0123456789abcdef";
    CHAR digits[3 * 0x10 + 5];
    CHAR ascii[0x11];
    UINT64 offset = (UINT64)Address;
    INT offsetSize = ((offset&0xFFFFFFFFFFFF0000)==0)?4:((offset&0xFFFFFFFF00000000)==0)?8:16;
    SIZE_T i, di, ai;
    PCHAR prefix = (Prefix)?Prefix:"";
//warning C6326: Potential comparison of a constant with another constant
DISABLE_WARNING ( 6326 )
    if ( KeGetCurrentIrql() != PASSIVE_LEVEL )
    {
        status = STATUS_NOT_SUPPORTED;
        if ( KeGetCurrentIrql() <= DISPATCH_LEVEL )
        {
            EPrint("Irql too high! (0x%x)\n", status);
        }
        return status;
    }
DEFAULT_WARNING ( 6326 )

    //if ( !LogBuffer || !LogFile )
    //{
    //    status = STATUS_INVALID_PARAMETER;
    //    EPrint("NO Log Buffer (0x%x)\n", status);
    //    return status;
    //}
    
    if ( flush )
    {
        status = FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);
        if ( !NT_SUCCESS(status) )
            return status;
    }
    else
    {
        (*LogBufferRestSize) = LogBufferSize;
        (*LogBufferPtr) = LogBuffer;
        RtlZeroMemory(LogBuffer, LogBufferSize);
    }

    if ( BufferSize == 0 )
        return status;
    if ( (*LogBufferRestSize) < 0x50 )
        return STATUS_BUFFER_TOO_SMALL;

    if ( Label && Label[0] != 0 )
    {
        status = RtlStringCchPrintfExA((*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                                        "%s",
                                       Label);
    }
    status = FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);
    if ( !NT_SUCCESS(status) )
        goto clean;

    ai = 0;
    di = 0;
    ascii[0] = 0;
    digits[0] = 0;
    for ( i = 0; i < BufferSize; i++ )
    {
        digits[di++] = digitMap[buffer[i] >> 4];
        digits[di++] = digitMap[buffer[i] & 0x0f];
        if( di == 0x17 ) 
            digits[di++] = '-';
        else
            digits[di++] = ' ';

        if( buffer[i] < 32 || buffer[i] > 126 || buffer[i] == '%' ) 
        {
            ascii[ai++] = '.';
        } 
        else 
        {
            ascii[ai++] = buffer[i];
        }

        if ( i % 0x10 == 0x0f )
        {
            ascii[ai] = 0;
            digits[di] = 0;

            if ( (*LogBufferRestSize) < 0x60 )
            {
                status = FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);
                if ( !NT_SUCCESS(status) )
                    goto clean;
            }
            if ( digits[0] != 0 )
            {
                status = RtlStringCchPrintfExA((*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                                               "%s%0*llx  %s %s\r\n",
                                                prefix, offsetSize, offset, digits, ascii);
            }
            // reset
            ascii[ai] = 0;
            digits[di] = 0;
            ai = 0;
            di = 0;
            ascii[0] = 0;
            digits[0] = 0;
            offset += 0x10;
        }
    }

    // print unaligned rest
    if ( i % 0x10 != 0 )
    {
        for ( ; di < 0x30; di++ )
            digits[di] = ' ';
        digits[di] = 0;
        ascii[ai] = 0;

        if ( (*LogBufferRestSize) < 0x60 )
        {
            status = FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);
            if ( !NT_SUCCESS(status) )
                goto clean;
        }
        if ( digits[0] != 0 )
        {
            status = RtlStringCchPrintfExA((*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                                            "%s%0*llx  %s %s\r\n",
                                            prefix, offsetSize, offset, digits, ascii);
        }
    }

    status = FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);
    if ( !NT_SUCCESS(status) )
        goto clean;

clean:
    return status;
}

NTSTATUS FlushLogFileBuffer(
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
)
{
    //PAGED_CODE();
    
    NTSTATUS status = STATUS_SUCCESS;

    ULONG written = 0;
    ULONG toWrite = (ULONG)(LogBufferSize-(*LogBufferRestSize));
    if ( toWrite == 0 )
        return 0;

    status = kWriteFile(LogFile, LogBuffer, toWrite, &written);
    if ( status != 0 )
    {
        EPrint("kWriteFile failed! (0x%x)\n", status);
        return status;
    }
    if ( written != toWrite )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("kWriteFile wrote less than expected! (0x%x)\n", status);
        return status;
    }
    RtlZeroMemory(LogBuffer, toWrite);
    (*LogBufferRestSize) = LogBufferSize;
    (*LogBufferPtr) = LogBuffer;

    return status;
}
