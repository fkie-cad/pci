#pragma once


//#define SPI_RAM_NR_FLASH_REGIONS (0x6) defined in spi-ram.h
//#define SPI_RAM_MAX_NR_FLASH_REGIONS (0xC) defined in spi-ram.h
#define DEFAULT_REGION_MASK ((1<<SPI_RAM_NR_FLASH_REGIONS)-1)
#define MAX_REGION_MASK ((1<<SPI_RAM_MAX_NR_FLASH_REGIONS)-1)


#define DUMP_BIOS_MODE_NONE (0x0)
#define DUMP_BIOS_MODE_SPI  (0x1)
//#define DUMP_BIOS_MODE_PTE  (0x2)
#define DUMP_BIOS_MODE_MMIO (0x3)

#define READ_CFG_MODE_NONE (0x0)
#define READ_CFG_MODE_PORT (0x1)
#define READ_CFG_MODE_MM   (0x2)


#pragma pack(1)

#ifndef _BDF_DEF
#define _BDF_DEF
typedef struct _BDF {
    UINT16 Bus;
    UINT16 Device;
    UINT16 Function;
} BDF, *PBDF;
#endif

typedef struct _PCI_READ_IN_FLAGS {
    UINT32 ReadMode:2;
    UINT32 ReadBars:1;
    UINT32 ReadCapabilities:1;
    UINT32 Print:1;
    UINT32 WriteToFile:1;
    UINT32 PCIe:1;
    UINT32 Reserved:26;
} PCI_READ_IN_FLAGS, *PPCI_READ_IN_FLAGS;

typedef struct _COMMAND_SPI_PARAMS {
    BDF BDF;    // 00
    UINT8 CycleCommand; // 06
    UINT8 FDBC; // 07 zero base actual fcycle byte count
    PVOID OutputBuffer; // 08
    UINT32 OutputBufferSize; // 10
    //RSP_STATUS_INFO StatusInfo; // 14
    UINT32 LinearAddress; // 18
    struct {
        UINT32 ReadMode:2;
        UINT32 Reserved:30;
    } Flags; // 1C
    PVOID FAddrVa; // 20
    PVOID HsFstsCtlVa; // 28
    UINT32 DataSize; // 30 desired full read size or full write size
    UINT16 BarId; // 34
    UINT16 int16_36; // 36 padding
    UINT32 Rcbo; // 38
    UINT32 int32_3C; // 3C padding
    PVOID Data; // 40
} COMMAND_SPI_PARAMS, *PCOMMAND_SPI_PARAMS; // 48

typedef struct _READ_SPI_PARAMS {
    BDF BDF; // 00
    UINT16 BarId; // 06
    PVOID OutputBuffer; // 08
    UINT32 OutputBufferSize; // 10
    //RSP_STATUS_INFO StatusInfo; // 14
    UINT32 Base; // 18
    UINT32 Limit; // 1C
    struct {
        UINT32 ReadMode:2;
        UINT32 DumpBiosMode:2;
        UINT32 Regions:12;
        UINT32 Reserved:16;
    } Flags; // 20
    UINT32 Rcbo; // 24
    PVOID FAddrVa; // 28
    PVOID HsFstsCtlVa; // 30
} READ_SPI_PARAMS, *PREAD_SPI_PARAMS; // 38

#pragma pack()
