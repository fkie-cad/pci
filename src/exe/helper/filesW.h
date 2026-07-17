#pragma once



#define PATH_SEPARATOR 0x5C

#define NT_FILE_READ_ACCESS     (FILE_GENERIC_READ | SYNCHRONIZE)
#define NT_FILE_WRITE_ACCESS    (FILE_GENERIC_WRITE | SYNCHRONIZE)


ULONG ntGetFullPathName(
    _In_ PWCHAR FileName,
    _In_ ULONG BufferCb,
    _Out_writes_bytes_to_(BufferCb, return) PWSTR Buffer,
    _Out_opt_ PWSTR *FilePart
);
