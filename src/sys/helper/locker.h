#ifndef __LOCKER_H
#define __LOCKER_H

#include <ntddk.h>


NTSTATUS
MdlLockBuffer(
    _Inout_ PVOID* Buffer, 
    _In_ ULONG BufferSize, 
    _Out_ PMDL* Mdl, 
    _In_ ULONG Pm,
    _In_ KPROCESSOR_MODE Mode,
    _In_ LOCK_OPERATION Lo,
    _In_ ULONG Priority
);

VOID
MdlUnlockBuffer(
    _Inout_ PMDL* Mdl
);

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, MdlLockBuffer)
#pragma alloc_text (PAGE, MdlUnlockBuffer)
#endif


#define PROBE_FOR_READ  (1)
#define PROBE_FOR_WRITE (2)


/**
 * Lock user buffer and let it point to Buffer.
 * On success mdl will be not NULL and locked. 
 * On failure mdl will be NULL (and not locked). 
 * 
 * @param Buffer PVOID* The input Buffer. !! On success points to the locked mdl buffer. Important, if it's allocated and has to be freed, save the original pointer beforehand!!
 * @param BufferSize ULONG The size of the input Buffer. 
 * @param mdl PMDL* The resulting mdl, needed for clean up after work is done. 
 * @param Pm ULONG ProbeForRead (1), ProbeForWrite (2) or don't probe for other values, like (0)
 * @param Mode KPROCESSOR_MODE KernelMode (0) or UserMode (1)
 * @param Lo LOCK_OPERATION lo IoReadAccess (0) or IoWriteAccess (1) or IoModifyAccess (2)
 * @param Priority ULONG Pass 0 for default NormalPagePriority | MdlMappingNoExecute. Possible values: LowPagePriority, NormalPagePriority, NormalPagePriority, NormalPagePriority, NormalPagePriority 
 * @return NTSTATUS
 */
// warning C6001: Using uninitialized memory '**mdl'.: Lines: 60, 61, 63, 65, 68, 70, 75, 77, 82, 83, 90
#pragma warning ( disable : 6001 )
NTSTATUS MdlLockBuffer(
    _Inout_ PVOID* Buffer, 
    _In_ ULONG BufferSize, 
    _Out_ PMDL* Mdl, 
    _In_ ULONG Pm,
    _In_ KPROCESSOR_MODE Mode,
    _In_ LOCK_OPERATION Lo,
    _In_ ULONG Priority
)
{
    PAGED_CODE();

    NTSTATUS status = STATUS_SUCCESS;
    BOOLEAN locked = FALSE;

    if ( Mdl == NULL )
        return STATUS_INVALID_PARAMETER_3;

    *Mdl = NULL;

    if ( Priority == 0 )
        Priority = NormalPagePriority | MdlMappingNoExecute;

    __try
    {
        if ( Pm == PROBE_FOR_READ )
            ProbeForRead(*Buffer, BufferSize, sizeof(UCHAR));
        else if ( Pm == PROBE_FOR_WRITE )
            ProbeForWrite(*Buffer, BufferSize, sizeof(UCHAR));

        // ChargeQuota (Param4): Reserved for system use. Drivers must set this parameter to FALSE.
        // windows ioctl example sets it to TRUE??
        *Mdl = IoAllocateMdl(*Buffer, BufferSize, FALSE, FALSE, NULL);
        if ( !(*Mdl) )
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean;
        }

        MmProbeAndLockPages(*Mdl, Mode, Lo);
        locked = TRUE;
        
        *Buffer = MmGetSystemAddressForMdlSafe(*Mdl, Priority);

        if ( !*Buffer )
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean;
        }

    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
        goto clean;
    }

clean:
    if ( !NT_SUCCESS(status) )
    {
        if ( *Mdl )
        {
            if ( locked )
                MmUnlockPages(*Mdl);
            IoFreeMdl(*Mdl);
        }
        *Mdl = NULL;
    }

    return status;
}
#pragma warning ( default : 6001 )

/**
 * Unlock and free mdl and set to NULL.
 */
VOID MdlUnlockBuffer(_Inout_ PMDL* Mdl)
{
    PAGED_CODE();

    if ( *Mdl )
    {
        MmUnlockPages(*Mdl);
        IoFreeMdl(*Mdl);
        *Mdl = NULL;
    }
}

#endif
