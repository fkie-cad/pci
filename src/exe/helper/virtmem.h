#pragma once

// warning C6001: Using uninitialized memory 'Buffer'.
DISABLE_WARNING(6001)
FORCEINLINE
NTSTATUS
reAllocVirtMem(
    _Inout_ PVOID* Buffer,
    _Inout_ PSIZE_T BufferSize,
    _In_ SIZE_T RequiredSize
)
{
    FEnter();

    NTSTATUS status = 0;

    if ( *Buffer )
    {
        NtFreeVirtualMemory(
                (HANDLE)-1,
                Buffer,
                BufferSize,
                MEM_RELEASE
            );
        *Buffer = NULL;
    }
    *BufferSize = RequiredSize;
    status = NtAllocateVirtualMemory(
            (HANDLE)-1,
            Buffer,
            0,
            BufferSize,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE
        );
    
    FLeave();
    return status;
}
DEFAULT_WARNING(6001)
