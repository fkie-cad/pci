#pragma once

#include "warnings.h"

//
// global device name of the rw driver
// used in the main function to open the device
//
#define DEVICE_BASE_NAME_W L"MemIo"


//
// ioctl codes used in memoryIo.h
//

// 0x22e000

#define IOCTL_PORT_IN CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_PORT_OUT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_MMIO_MAP CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_MMIO_UNMAP CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)

#define IOCTL_READ_PA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_WRITE_PA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)


#pragma pack(1)


//
// input output structs used in memoryIo.h
//

typedef struct _PORT_IN_IN {
    UINT16 Port;
    UINT16 Size;
} PORT_IN_IN, *PPORT_IN_IN;

typedef struct _PORT_IN_OUT {
    UINT32 Value;
} PORT_IN_OUT, *PPORT_IN_OUT;

typedef struct _PORT_OUT {
    UINT16 Port;
    UINT16 Size;
    UINT32 Value;
} PORT_OUT, *PPORT_OUT;




typedef struct _READ_PA_IN {
    PVOID Address;
    UINT32 Size;
    UINT32 PageFlags;
} READ_PA_IN, *PREAD_PA_IN;
#define READ_PA_IN_SIZE (ULONG)(sizeof(READ_PA_IN))

typedef struct _READ_PA_OUT {
    UINT8 Data[1];
} READ_PA_OUT, *PREAD_PA_OUT;
#define READ_PA_OUT_SIZE (ULONG)(sizeof(READ_PA_OUT))



typedef struct _WRITE_PA_IN {
    PVOID Address; // 00
    UINT32 Size; // 08
    UINT32 PageFlags; // 0C
    UINT8 Data[1]; // 10
} WRITE_PA_IN, *PWRITE_PA_IN; // 10++
#define WRITE_PA_IN_DEF_SIZE (ULONG)(sizeof(WRITE_PA_IN)-1)



typedef struct _MAP_PA_IN {
    PVOID Address;
    UINT32 Size;
    //UINT32 PageFlags;
} MAP_PA_IN, *PMAP_PA_IN;
#define MAP_PA_IN_SIZE (ULONG)(sizeof(MAP_PA_IN))

typedef struct _MAP_PA_OUT {
    PVOID MappedAddress;
    PVOID LockedAddress;
    PMDL Mdl;
    UINT32 Size;
} MAP_PA_OUT, *PMAP_PA_OUT;
#define MAP_PA_OUT_SIZE (ULONG)(sizeof(MAP_PA_OUT))
typedef MAP_PA_OUT UNMAP_PA_IN;
typedef MAP_PA_OUT *PUNMAP_PA_IN;

//typedef struct _UNMAP_PA_IN {
//    PVOID MappedAddress;
//    PVOID LockedAddress;
//    PMDL Mdl;
//    UINT32 Size;
//} UNMAP_PA_IN, *PUNMAP_PA_IN;
#define UNMAP_PA_IN_SIZE (ULONG)(sizeof(UNMAP_PA_IN))



#pragma pack()
