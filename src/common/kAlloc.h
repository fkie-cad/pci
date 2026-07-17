#pragma once

#ifdef RING3
//warning C4005: 'ExFreePool': macro redefinition
#pragma warning ( disable : 4005 )
#define ExAllocatePoolWithTag(_pt_, _n_, _t_) malloc(_n_)
// #define ExFreePoolWithTag(_p_, _t_) free(_p_)
#define ExFreePool(_p_) free(_p_)
#pragma warning ( default : 4005 )
#endif

FORCEINLINE
NTSTATUS ExReAllocPool(
    _Inout_ PVOID *In, 
    _Inout_ PULONG OldSize, 
    _In_ ULONG NewSize, 
    _In_ ULONG Tag
)
{
    (Tag);
    PVOID tmp = ExAllocatePoolWithTag(PagedPool, NewSize, Tag);
    if ( !tmp )
        return STATUS_NO_MEMORY;
    RtlZeroMemory(tmp, NewSize);

    if ( *In )
    {
        RtlMoveMemory(tmp, *In, (NewSize>*OldSize)?*OldSize:NewSize);
        RtlSecureZeroMemory(*In, *OldSize);
        ExFreePool(*In);
    }
    *In = tmp;
    *OldSize = NewSize;
    
    return STATUS_SUCCESS;
}
