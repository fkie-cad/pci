#pragma once

#include "../warnings.h"
#include "../MemIoShared.h"

#ifdef DEBUG_PRINT
#define DRIVER_NAME "MemIo"
#define DRIVER_VS "1.1.1"
#define DRIVER_LC "15.07.2026"
#endif

#define POOL_TAG ('imem')

#define FLAG_FORCE_UNLOAD (0x1)



#pragma pack(1)

#pragma pack()


DRIVER_INITIALIZE DriverEntry;

DRIVER_UNLOAD UnloadDriver;

_Dispatch_type_(IRP_MJ_CREATE)
_Dispatch_type_(IRP_MJ_CLOSE)
DRIVER_DISPATCH CreateClose;

_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH DeviceControl;



NTSTATUS
inPort(
    _In_ UINT16 Port,
    _In_ UINT16 Size,
    _Inout_ PVOID OutputBuffer,
    _In_ UINT32 OutputBufferSize,
    _Out_ PULONG BytesWritten
);

NTSTATUS
outPort(
    _In_ UINT16 Port,
    _In_ UINT16 Size,
    _In_ UINT32 Value
);

NTSTATUS readPA(
    _In_ PVOID Address,
    _In_ UINT32 Size,
    _In_ MEMORY_CACHING_TYPE CachingType,
    _Inout_ PVOID OutData,
    _In_ UINT32 OutSize,
    _Out_ PULONG BytesWritten
);
NTSTATUS writePA(
    _In_ PVOID Address,
    _In_ MEMORY_CACHING_TYPE CachingType,
    _In_ PVOID Data,
    _In_ UINT32 Size
);

NTSTATUS mapPA(
    _In_ PVOID Address,
    _In_ UINT32 Size,
    _Out_ PVOID* MappedAddress,
    _Out_ PVOID* LockedAddress,
    _Out_ PMDL* Mdl,
    _Out_ PULONG BytesWritten
);

NTSTATUS unmapPA(
    _In_ PVOID MappedAddress,
    _In_ PVOID LockedAddress,
    _In_ PMDL Mdl,
    _In_ UINT32 Size
);

#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, UnloadDriver)
#pragma alloc_text (PAGE, CreateClose)
#pragma alloc_text (PAGE, DeviceControl)

//#pragma alloc_text (NONPAGE, readADPort)
//#pragma alloc_text (NONPAGE, writeADPort)
#pragma alloc_text (NONPAGE, inPort)
#pragma alloc_text (NONPAGE, outPort)

#pragma alloc_text (PAGE, readPA)
#pragma alloc_text (PAGE, writePA)

#pragma alloc_text (PAGE, mapPA)
#pragma alloc_text (PAGE, unmapPA)
