#pragma once

#ifdef RING3
#pragma warning( push )
#include "../warnings_ntifs_wdm.h"
#include <ntifs.h>
#pragma warning( pop )
#else
#include <ntifs.h>
#endif

// ============================================================

// 
// docu
// https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntcreatefile
// This API only provides the most basic standard read / write / getFilesize I/O.
// No EA or REPARSEPOINTS or similar.
// 

#define OPEN_FOR_READ_ONLY      (1)
#define OPEN_FOR_WRITE_ONLY     (2)
#define OPEN_FOR_READWRITE      (3)
#define OPEN_FOR_APPEND         (4)
#define OPEN_DIR                (5)
#define CREATE_DIR              (6)


NTSTATUS kOpenFile(PWCHAR FileName, ULONG FileNameCch, PHANDLE Handle, ULONG OpenMode, ULONG CreateDisposion, ULONG Share);
NTSTATUS kCloseFile(HANDLE Handle);
NTSTATUS kDeleteFile(PWCHAR FileName);
unsigned __int64 kGetFileSize(HANDLE Handle);
NTSTATUS kReadFile(HANDLE Handle, PVOID Buffer, ULONG Size, PULONG BytesRead);
NTSTATUS kReadFileAtOffset(HANDLE Handle, PVOID Buffer, ULONG Size, PULONG BytesRead, PLARGE_INTEGER Offset);
NTSTATUS kWriteFile(HANDLE Handle, PVOID Buffer, ULONG Size, PULONG BytesWritten);
NTSTATUS kWriteFileAtOffset(HANDLE Handle, PVOID Buffer, ULONG Size, PULONG BytesWritten, PLARGE_INTEGER Offset);

BOOLEAN kDirExists(_In_ WCHAR* Path, _In_ ULONG PathCch);
BOOLEAN kFileExists(_In_ WCHAR* Path, _In_ ULONG PathCch);

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, kOpenFile)
#pragma alloc_text (PAGE, kCloseFile)
#pragma alloc_text (PAGE, kDeleteFile)
#pragma alloc_text (PAGE, kGetFileSize)
#pragma alloc_text (PAGE, kReadFile)
#pragma alloc_text (PAGE, kReadFileAtOffset)
#pragma alloc_text (PAGE, kWriteFile)
#pragma alloc_text (PAGE, kWriteFileAtOffset)
#pragma alloc_text (PAGE, kDirExists)
#pragma alloc_text (PAGE, kFileExists)
#endif
