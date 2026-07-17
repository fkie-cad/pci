#pragma once


/**
 * Log buffer memory
 */
NTSTATUS logBarBuffer(
    _In_ PVOID Address,
    _In_ PVOID Buffer,
    _In_ ULONG Size,
    _In_ PLOG Log
)
{
    NTSTATUS status = 0;
    
    PCHAR logBufferPtr = Log->Buffer;
    SIZE_T logBufferRestSize = Log->BufferSize;

    FEnter();
    
    LogBytes(NULL, NULL, Buffer, Size, Address, TRUE, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

//clean:

    FLeave();
    return status;
}

/**
 * Log bar memory as binary file
 */
NTSTATUS logBarBinary(
    _In_ PBDF BDF,
    _In_ ULONG BarId,
    _In_ PVOID Buffer,
    _In_ UINT64 Size,
    _In_ PWCHAR LogDir
)
{
    NTSTATUS status = 0;
    
    PWCHAR logFilePath = NULL;
    HANDLE binLogFile = NULL;

    ULONG written = 0;

    FEnter();
    
    WCHAR logFileName[0x40] = {0};
    PWCHAR logFileNamePtr = logFileName;
    ULONG logFileNameCch = 0x40;
    SIZE_T logFileNameRestCch = logFileNameCch;
    PWCHAR prefix = L"cfg";

    RtlStringCchPrintfExW(logFileNamePtr, logFileNameRestCch, &logFileNamePtr, &logFileNameRestCch, 0,
        L"%s-%02x-%02x-%02x-bar%u",
        prefix,
        BDF->Bus, BDF->Device, BDF->Function,
        BarId);
    
    status = getLogFilePath(&logFilePath, LogDir, NULL, logFileName, L".bin");
    if ( status != 0 )
        goto clean;

    status = kOpenFile(logFilePath, (ULONG)wcslen(logFilePath), &binLogFile, OPEN_FOR_WRITE_ONLY, FILE_OVERWRITE_IF, FILE_SHARE_READ);
    if ( !NT_SUCCESS(status) )
    {
        EPrint("Opening log file \"%ws\" failed! (0x%x)\n", logFilePath, status);
        goto clean;
    }

    status = kWriteFile(binLogFile, Buffer, (UINT32)Size, &written);
    if ( !NT_SUCCESS(status) )
    {
        EPrint("kWriteFile failed! (0x%x)\n", status);
        goto clean;
    }

clean:
    if ( logFilePath )
        ExFreePool(logFilePath);
    if ( binLogFile )
        ZwClose(binLogFile);
    
    FLeave();
    return status;
}

NTSTATUS
writeBarToFile(
    _In_ PBDF BDF,
    _In_ PVOID BarBaseAddress,
    _In_ ULONG BarId,
    _In_ PBAR Bar,
    _In_ UINT64 BarSize,
    _In_ PVOID BarBuffer,
    _In_ PLOG Log
)
{
    FEnter();

    NTSTATUS status = 0;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;
    
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "Bar[%u] (%s, %s, 0x%llx, 0x%llx)\r\n", 
                            BarId,
                            BAR_TYPE_STR(Bar),
                            BAR_ADDRESS_WIDTH(Bar),
                            (UINT64)BarBaseAddress,
                            BarSize);
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    if ( BarSize < MAX_BAR_SIZE_TO_LOG )
    {
        status = logBarBuffer(BarBaseAddress, BarBuffer, (UINT32)BarSize, Log);
    }
    else
    {
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
            "=> cfg-%02x-%02x-%02x-bar%u.bin\r\n"
            "\r\n",
            BDF->Bus, BDF->Device, BDF->Function,
            BarId);
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }
    status = logBarBinary(BDF, BarId, BarBuffer, BarSize, Log->Dir);

//clean:
    FLeave();
    return status;
}
