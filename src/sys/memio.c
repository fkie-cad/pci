#include <ntifs.h>
#include <ntstrsafe.h>

#include "memio.h"

#include "inc/ntEx.h"
#include "../print.h"

#include "helper/portIo.h"

#include "helper/locker.h"



UNICODE_STRING dosDeviceName = RTL_CONSTANT_STRING(L"\\DosDevices\\" DEVICE_BASE_NAME_W); 


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    NTSTATUS status = STATUS_SUCCESS;
    
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\" DEVICE_BASE_NAME_W);
    PDEVICE_OBJECT deviceObject = NULL;

    UNREFERENCED_PARAMETER(RegistryPath);

#ifdef DEBUG_PRINT
    DbgPrint("\n"
        "-----------------------------\n");
    DPrint("%s - %s\n", DRIVER_VS, DRIVER_LC);
#ifdef DBG
    DPrint("compiled: %s %s\n", __DATE__, __TIME__);
#endif
    DbgPrint("-----------------------------\n");
#endif
    
    status = IoCreateDevice(
                DriverObject, 
                0, 
                &deviceName, 
                FILE_DEVICE_UNKNOWN, 
                FILE_DEVICE_SECURE_OPEN, 
                FALSE, 
                &deviceObject
            );
    if ( !NT_SUCCESS(status) )
    {
        EPrint("IoCreateDevice failed. (0x%x)\n", status);
        goto clean;
    }
    
    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
    DriverObject->DriverUnload = UnloadDriver;
    
    status = IoCreateSymbolicLink(&dosDeviceName, &deviceName);
    if ( !NT_SUCCESS(status) )
    {
        EPrint("IoCreateSymbolicLink failed. (0x%x)\n", status);
        goto clean;
    }
    
clean:
    if ( status != STATUS_SUCCESS )
    {
        IoDeleteSymbolicLink(&dosDeviceName);
        if ( DriverObject->DeviceObject )
            IoDeleteDevice(DriverObject->DeviceObject);
    }
    
    SPrint(status);
    return status;
}

VOID UnloadDriver(PDRIVER_OBJECT DriverObject)
{
    PAGED_CODE();
    FEnter();

    IoDeleteSymbolicLink(&dosDeviceName);
    if ( DriverObject->DeviceObject )
        IoDeleteDevice(DriverObject->DeviceObject);

    FLeave();
}

NTSTATUS CreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    PAGED_CODE();

    (DeviceObject);
    
    FEnter();

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    FLeave();
    return STATUS_SUCCESS;
}

NTSTATUS DeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    PAGED_CODE();
    FEnter();

    NTSTATUS status = STATUS_NOT_SUPPORTED;
    PIO_STACK_LOCATION irpStack = NULL;
    ULONG bytesWritten = 0;
    
    ULONG ioctl = 0;

    PVOID inputBuffer = NULL;
    ULONG inputBufferSize = 0;
    PVOID outputBuffer = NULL;
    ULONG outputBufferSize = 0;

    PMDL inputBufferMdl = NULL;
    PMDL outputBufferMdl = NULL;
    
    UNREFERENCED_PARAMETER(DeviceObject);
    
    irpStack = IoGetCurrentIrpStackLocation(Irp); 

    if ( irpStack == NULL )
    {
        status = STATUS_UNSUCCESSFUL;
        goto clean;
    }

    ioctl = irpStack->Parameters.DeviceIoControl.IoControlCode;
    DPrint(" - Flags: 0x%lx\r\n", DeviceObject->Flags);
    DPrint(" - IoControlCode: 0x%lx\r\n", ioctl);

    // should be in the IRP somewhere as well
    UINT8 method = METHOD_FROM_CTL_CODE(ioctl);
    if ( method == METHOD_BUFFERED )
    {
        inputBuffer = Irp->AssociatedIrp.SystemBuffer;
        inputBufferSize = irpStack->Parameters.DeviceIoControl.InputBufferLength;
        outputBuffer = Irp->AssociatedIrp.SystemBuffer;
        outputBufferSize = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }
    else if ( method == METHOD_NEITHER )
    {
        inputBuffer = irpStack->Parameters.DeviceIoControl.Type3InputBuffer;
        inputBufferSize = irpStack->Parameters.DeviceIoControl.InputBufferLength;
        outputBuffer = Irp->UserBuffer;
        outputBufferSize = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
    }



    switch ( ioctl )
    {
        case IOCTL_PORT_IN:
        {
            DPrint("IOCTL_PORT_IN\n");
            
            PPORT_IN_IN in = (PPORT_IN_IN)inputBuffer;

            if ( !in 
                || inputBufferSize < sizeof(PORT_IN_IN) 
               )
            {
                status = STATUS_INVALID_PARAMETER;
                EPrint("Invalid input buffer! (0x%x)\n", status);
                goto clean;
            }

            status = inPort(in->Port, in->Size, outputBuffer, outputBufferSize, &bytesWritten);

            break;
        }
        case IOCTL_PORT_OUT:
        {
            DPrint("IOCTL_PORT_OUT\n");
            
            PPORT_OUT in = (PPORT_OUT)inputBuffer;

            if ( !in 
                || inputBufferSize < sizeof(PORT_OUT)
               )
            {
                status = STATUS_INVALID_PARAMETER;
                EPrint("Invalid input buffer! (0x%x)\n", status);
                goto clean;
            }

            status = outPort(in->Port, in->Size, in->Value);

            break;
        }
        case IOCTL_MMIO_MAP:
        {
            DPrint("IOCTL_MMIO_MAP\n");
            
            PMAP_PA_IN in = (PMAP_PA_IN)inputBuffer;
            PMAP_PA_OUT out = (PMAP_PA_OUT)outputBuffer;

            if ( !in 
                || !out
                || inputBufferSize < sizeof(MAP_PA_IN)
                || outputBufferSize < sizeof(MAP_PA_OUT)
               )
            {
                status = STATUS_INVALID_PARAMETER;
                EPrint("Invalid input buffer! (0x%x)\n", status);
                goto clean;
            }
            
            UINT32 size = in->Size;
            status = mapPA(in->Address, size, &out->MappedAddress, &out->LockedAddress, &out->Mdl, &bytesWritten);
            if ( status == 0 )
            {
                out->Size = size;
                bytesWritten = sizeof(MAP_PA_OUT);
            }
            break;
        }
        case IOCTL_MMIO_UNMAP:
        {
            DPrint("IOCTL_MMIO_UNMAP\n");
            
            PUNMAP_PA_IN in = (PUNMAP_PA_IN)inputBuffer;

            if ( !in 
                || inputBufferSize < sizeof(UNMAP_PA_IN)
               )
            {
                status = STATUS_INVALID_PARAMETER;
                EPrint("Invalid input buffer! (0x%x)\n", status);
                goto clean;
            }

            status = unmapPA(in->MappedAddress, in->LockedAddress, in->Mdl, in->Size);
            break;
        }
        case IOCTL_READ_PA:
        {
            DPrint("IOCTL_READ_PA\n");
            // neither io
            if ( !inputBuffer || !inputBufferSize || !outputBuffer || !outputBufferSize )
            {
                status = STATUS_INVALID_PARAMETER;
                EPrint("Invalid output buffer! (0x%x)\n", status);
                goto clean;
            }

            status = MdlLockBuffer(&inputBuffer, inputBufferSize, &inputBufferMdl, PROBE_FOR_READ, KernelMode, IoReadAccess, NormalPagePriority);
            if ( !NT_SUCCESS(status) )
                goto clean;

            status = MdlLockBuffer(&outputBuffer, outputBufferSize, &outputBufferMdl, PROBE_FOR_WRITE, KernelMode, IoWriteAccess, NormalPagePriority);
            if ( !NT_SUCCESS(status) )
                goto clean;

            PREAD_PA_IN in = (PREAD_PA_IN)inputBuffer;
            PREAD_PA_OUT out = (PREAD_PA_OUT)outputBuffer;

            if ( !in 
                || inputBufferSize < READ_PA_IN_SIZE 
                || outputBufferSize < in->Size
                || (UINT64)in->Address % 4 != 0
                || in->Size % 4 != 0
               )
            {
                status = STATUS_INVALID_PARAMETER;
                EPrint("Invalid input buffer! (0x%x)\n", status);
                goto clean;
            }
            DPrint("Address: %p\n", in->Address);
            DPrint("Size: 0x%x\n", in->Size);
            
            status = readPA(in->Address, in->Size, in->PageFlags, out->Data, outputBufferSize, &bytesWritten);

            break;
        }
        case IOCTL_WRITE_PA:
        {
            DPrint("IOCTL_WRITE_PA\n");
            // neither io
            if ( !inputBuffer || !inputBufferSize )
            {
                status = STATUS_INVALID_PARAMETER;
                EPrint("Invalid output buffer! (0x%x)\n", status);
                goto clean;
            }

            status = MdlLockBuffer(&inputBuffer, inputBufferSize, &inputBufferMdl, PROBE_FOR_READ, KernelMode, IoReadAccess, NormalPagePriority);
            if ( !NT_SUCCESS(status) )
                goto clean;

            PWRITE_PA_IN in = (PWRITE_PA_IN)inputBuffer;

            if ( !in 
                || inputBufferSize < WRITE_PA_IN_DEF_SIZE 
                || inputBufferSize < WRITE_PA_IN_DEF_SIZE + in->Size
                || (UINT64)in->Address % 4 != 0
                || in->Size % 4 != 0
               )
            {
                status = STATUS_INVALID_PARAMETER;
                EPrint("Invalid input buffer! (0x%x)\n", status);
                goto clean;
            }
            
            status = writePA(in->Address, in->PageFlags, in->Data, in->Size);
            
            break;
        }
        default: 
        {
            EPrint("Unknown IOCTL! (0x%x)\n", status);
            status = STATUS_NOT_SUPPORTED;

            break;
        }
    }
    
clean:

    MdlUnlockBuffer(&inputBufferMdl);
    MdlUnlockBuffer(&outputBufferMdl);

    // Fill in Info for the IOSB.
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = bytesWritten;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    SPrint(status);

    return status;
}

NTSTATUS
inPort(
    _In_ UINT16 Port,
    _In_ UINT16 Size,
    _Inout_ PVOID OutputBuffer,
    _In_ UINT32 OutputBufferSize,
    _Out_ PULONG BytesWritten
)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    PUINT8 bufferPtr = OutputBuffer;


    *BytesWritten = 0;

    FEnter();
    DPrint("  Port: 0x%x\n", Port);
    DPrint("  Size: 0x%x\n", Size);
    DPrint("  OutputBuffer: %p\n", OutputBuffer);
    DPrint("  OutputBufferSize: 0x%x\n", OutputBufferSize);

    if ( !Size )
    {
        return STATUS_INVALID_PARAMETER;
    }
    if ( OutputBufferSize < Size )
    {
        return STATUS_BUFFER_OVERFLOW;
    }

    __try
    {
        switch ( Size )
        {
            case 1:
            {
                _disable();
                *(PUINT8)bufferPtr = __inbyte(Port);
                KeStallExecutionProcessor(PROC_DELAY);
                _enable();
                status = 0;

                if ( status != 0 )
                {
                    EPrint("__inbyte(0x%x failed! (0x%x)\n", Port, status);
                    goto clean;
                }

                break;
            }
            case 2:
            {
                _disable();
                *(PUINT16)bufferPtr = __inword(Port);
                KeStallExecutionProcessor(PROC_DELAY);
                _enable();
                status = 0;

                if ( status != 0 )
                {
                    EPrint("__inword(0x%x failed! (0x%x)\n", Port, status);
                    goto clean;
                }

                break;
            }
            case 4:
            {
                _disable();
                *(PUINT32)bufferPtr = __indword(Port);
                KeStallExecutionProcessor(PROC_DELAY);
                _enable();
                status = 0;

                if ( status != 0 )
                {
                    EPrint("__indword(0x%x) failed! (0x%x)\n", Port, status);
                    goto clean;
                }

                break;
            }
            default:
                status = STATUS_INVALID_PARAMETER;
                EPrint("Unknow size! (0x%x)\n", status);
                break;
        }
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    if ( status == 0 )
        *BytesWritten = Size;

clean:
    
    FLeave();
    return status;
}

NTSTATUS
outPort(
    _In_ UINT16 Port,
    _In_ UINT16 Size,
    _In_ UINT32 Value
)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    FEnter();
    DPrint("  Port: 0x%x\n", Port);
    DPrint("  Size: 0x%x\n", Size);
    DPrint("  Value: 0x%x\n", Value);

    if ( !Size )
    {
        return STATUS_INVALID_PARAMETER;
    }
    //if ( OutputBufferSize < Size )
    //{
    //    return STATUS_BUFFER_OVERFLOW;
    //}
    
    __try
    {
        switch ( Size )
        {
            case 1:
            {
                _disable();
                __outbyte(Port, (UINT8)Value);
                _enable();
                KeStallExecutionProcessor(PROC_DELAY);
                status = 0;

                if ( status != 0 )
                {
                    EPrint("__outbyte(0x%x, 0x%x) failed! (0x%x)\n", Port, Value, status);
                    goto clean;
                }

                break;
            }
            case 2:
            {
                _disable();
                __outword(Port, (UINT16)Value);
                _enable();
                KeStallExecutionProcessor(PROC_DELAY);
                status = 0;

                if ( status != 0 )
                {
                    EPrint("__outword(0x%x, 0x%x) failed! (0x%x)\n", Port, Value, status);
                    goto clean;
                }

                break;
            }
            case 4:
            {
                _disable();
                __outdword(Port, Value);
                _enable();
                KeStallExecutionProcessor(PROC_DELAY);
                status = 0;

                if ( status != 0 )
                {
                    EPrint("__outdword(0x%x, 0x%x) failed! (0x%x)\n", Port, Value, status);
                    goto clean;
                }

                break;
            }
            default:
            {
                status = STATUS_INVALID_PARAMETER;
                EPrint("Unknow size of 0x%x! (0x%x)\n", Size, status);

                break;
            }
        }
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

clean:

    FLeave();
    return status;
}

//
// read physical memory in 4 byte chunks
//
NTSTATUS readPA(
    _In_ PVOID Address,
    _In_ UINT32 Size,
    _In_ MEMORY_CACHING_TYPE CachingType,
    _Inout_ PVOID OutData,
    _In_ UINT32 OutSize,
    _Out_ PULONG BytesWritten
)
{
    PAGED_CODE();

    NTSTATUS status = 0;

    PHYSICAL_ADDRESS pa = { .QuadPart = (UINT64)Address };
    PUINT8 mappedVA = NULL;

    *BytesWritten = 0;

    FEnter();
    DPrint("Address: %p\n", Address);
    DPrint("Size: 0x%x\n", Size);

    if ( Size > OutSize )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Read size > out size! (0x%x)\n", status);
        return status;
    }
    if ( (UINT64)Address%4 != 0 || Size % 4 != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Not 4 Byte aligned! (0x%x)\n", status);
        return status;
    }
    RtlZeroMemory(OutData, Size);
    
    UINT64 nextPage = (UINT64)ALIGN_UP_TO_NEXT_BY(Address, PAGE_SIZE);
    if ( (UINT64)Address + Size > nextPage )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Read exceeding page! (0x%x)\n", status);
        return status;
    }
    
    try
    {
        mappedVA = MmMapIoSpace(pa, Size, CachingType);
        if ( !mappedVA )
        {
            status = STATUS_UNSUCCESSFUL;
            EPrint("mapping failed! (0x%x)\n", status);
            goto clean;
        }
        DPrint("mappedVA: %p\n", mappedVA);

        // just copying may be not safe for some physical addresses
        // even 4 byte chunks may not be safe but are the most common
        //RtlCopyMemory(OutData, mappedVA, Size);
        PUINT32 outPtr32 = (PUINT32)OutData;
        volatile PUINT32 vaPtr32 = (PUINT32)mappedVA;

        for ( UINT32 i = 0; i < Size; i += 4 )
        {
            *outPtr32 = *vaPtr32;
            ++outPtr32;
            ++vaPtr32;
        }
    } 
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        EPrint("Unable to read 0x%x bytes from %p! (0x%x)", Size, Address, status);
        goto clean;
    }

    *BytesWritten = (UINT32)Size;

clean:
    if ( mappedVA )
        MmUnmapIoSpace(mappedVA, Size);

    FLeave();
    return status;
}

//
// write physical address in 4 byte chunks
//
NTSTATUS writePA(
    _In_ PVOID Address,
    _In_ MEMORY_CACHING_TYPE CachingType,
    _In_ PVOID Data,
    _In_ UINT32 Size
)
{
    FEnter();
    PAGED_CODE();

    NTSTATUS status = 0;

    PHYSICAL_ADDRESS pa = { .QuadPart = (UINT64)Address };
    PUINT8 mappedVA = NULL;

    DPrint("Address: %p\n", Address);
    DPrint("CachingType: 0x%x\n", CachingType);
    DPrint("Data: %p\n", Data);
    DPrint("Size: 0x%x\n", Size);
    DPrintMemCol8(Data, Size, 0);

    if ( (UINT64)Address%4 != 0 || Size % 4 != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Not 4 Byte aligned! (0x%x)\n", status);
        return status;
    }
    UINT64 nextPage = (UINT64)ALIGN_UP_TO_NEXT_BY(Address, PAGE_SIZE);
    if ( (UINT64)Address + Size > nextPage )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Write exceeding page! (0x%x)\n", status);
        return status;
    }

    mappedVA = MmMapIoSpace(pa, Size, CachingType);
    if ( !mappedVA )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("Mapping %p failed! (0x%x)\n", (PVOID)pa.QuadPart, status);
        goto clean;
    }

    switch ( Size )
    {
        case 1:
            mappedVA[0] = ((PUINT8)Data)[0];
            break;
        case 2:
            _InterlockedExchange16((SHORT*)(mappedVA), *(SHORT*)Data);
            break;
        case 4:
            _InterlockedExchange((LONG*)(mappedVA), *(LONG*)Data);
            break;
        case 8:
            _InterlockedExchange64((LONG64*)(mappedVA), *(LONG64*)Data);
            break;
        default:
            //RtlCopyMemory(mappedVA, Data, Size);
            // just copying may be not safe for some physical addresses
            // even 4 byte chunks may not be safe but are the most common
            //RtlCopyMemory(OutData, mappedVA, Size);
            ;
            PUINT32 dataPtr32 = (PUINT32)Data;
            volatile PUINT32 vaPtr32 = (PUINT32)mappedVA;

            for ( UINT32 i = 0; i < Size; i += 4 )
            {
                *vaPtr32 = *dataPtr32;
                ++dataPtr32;
                ++vaPtr32;
            }
            break;
    }

clean:
    if ( mappedVA )
        MmUnmapIoSpace(mappedVA, Size);
    
    FLeave();
    return status;
}


NTSTATUS mapPA(
    _In_ PVOID Address,
    _In_ UINT32 Size,
    _Out_ PVOID* MappedAddress,
    _Out_ PVOID* LockedAddress,
    _Out_ PMDL* Mdl,
    _Out_ PULONG BytesWritten
)
{
    PAGED_CODE();
    FEnter();

    NTSTATUS status = 0;
    PVOID mappedVA = NULL;
    PHYSICAL_ADDRESS pa = { .QuadPart = (UINT64)Address };
    PMDL mdl = NULL;
    PVOID locakedVA = NULL;
    MEMORY_CACHING_TYPE cacheType = MmNonCached;
    
    DPrint("Address: %p\n", Address);
    DPrint("Size: 0x%x\n", Size);

    mappedVA = MmMapIoSpace(pa, Size, cacheType);
    if ( !mappedVA )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("Mapping %p failed! (0x%x)\n", (PVOID)pa.QuadPart, status);
        goto clean;
    }

    mdl = IoAllocateMdl(mappedVA, Size, 0, 0, 0);
    if ( !mdl )
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        EPrint("Creating MDL failed! (0x%x)\n", status);
        goto clean;
    }
    
    MmBuildMdlForNonPagedPool(mdl);

    __try
    {
        locakedVA = MmMapLockedPagesSpecifyCache(mdl, UserMode, cacheType, 0, 0, 0x10u);
        if ( !locakedVA )
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            EPrint("Locking Mdl failed! (0x%x)\n", status);
            goto clean;
        }
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
        goto clean;
    }

    *MappedAddress = mappedVA;
    *LockedAddress = locakedVA;
    *Mdl = mdl;
    *BytesWritten = 3 * sizeof(PVOID);

    DPrint("MappedAddress: %p\n", *MappedAddress);
    DPrint("LockedAddress: %p\n", *LockedAddress);
    DPrint("Mdl: %p\n", *Mdl);

clean:
    if ( status != 0 )
    {
        if ( mappedVA )
            MmUnmapIoSpace(mappedVA, Size);
    }

    FLeave();
    return status;
}

NTSTATUS unmapPA(
    _In_ PVOID MappedAddress,
    _In_ PVOID LockedAddress,
    _In_ PMDL Mdl,
    _In_ UINT32 Size
)
{
    PAGED_CODE();
    FEnter();
    
    NTSTATUS status = 0;

    DPrint("MappedAddress: %p\n", MappedAddress);
    DPrint("LockedAddress: %p\n", LockedAddress);
    DPrint("Mdl: %p\n", Mdl);
    DPrint("Size: 0x%x\n", Size);

    if ( !MappedAddress || !LockedAddress || !Mdl )
    {
        status = STATUS_INVALID_PARAMETER;
        goto clean;
    }
    
    MmUnmapLockedPages(LockedAddress, Mdl);
    
    IoFreeMdl(Mdl);
    
    MmUnmapIoSpace(MappedAddress, Size);
    
clean:
    FLeave();
    return status;
}
