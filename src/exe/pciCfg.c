#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include <winerror.h>
#else
#define __cdecl
#include "inc/error.h"
#endif

#include "../print.h"
#include "helper/Args.h"


#define APP_NAME "PciCfg"
#define BIN_NAME APP_NAME ".exe"
#define APP_VS "1.0.3"
#define APP_LC "15.07.2026"



// for each bus, 
// we need 32 (device) * 8 (function) * 4KB of memory space, this is equal to 1MB; 
// for each device, we need 8 (function) * 4KB of memory space, this is equal to 32KB.
#define PCI_CFG_MM_ADDR(__xbar__, __b__, __d__, __f__, __o__) \
    /*__xbar__ + __b__ * 0x10'0000 + __d__ * 0x8000 + __f__ * 0x1000 + __o__*/ \
    (uint64_t)(((uint64_t)__xbar__) + (((uint64_t)__b__) << 0x14) + (((uint64_t)__d__) << 0xf ) + (((uint64_t)__f__) << 0xc) + ((uint64_t)__o__))

typedef struct _MMIO_ADDRESS {
    uint64_t Offset:12; //- Offset 11:0 (12 bits)
    uint64_t Function:3; //- Function 14:12 (3 bits)
    uint64_t Device:5; //- Device 19:15 (5 bits)
    uint64_t Bus:8; //- Bus 27:20 (8 bits)
    uint64_t PciExBar:11; //- PCIEXBAR's Bits 38:28
    uint64_t RES_63_39:25; //-res 63:39
} MMIO_ADDRESS, *PMMIO_ADDRESS;

#define PCI_CFG_PORT_IO_ADDR(__b__, __d__, __f__, __o__) \
    (uint32_t)( 0x80000000 | (((uint32_t)__b__) << 0x10) | (((uint32_t)__d__) << 0xb) | (((uint32_t)__f__) << 8 | __o__) )



void printVersion()
{
    printf("%s %s\n", APP_NAME, APP_VS);
    printf("Last changed: %s\n", APP_LC);
    printf("Compiled: %s -- %s\n", __DATE__, __TIME__);
}

void printUsage(char* arg0)
{
    printf("Usage: %s [/io <bus> <device> <function> <offset>]\n"
        "     [/mm <pciexbar> <bus> <device> <function> <offset>]\n"
        "     [/io <portIoAddr>]\n"
        "     [/mm <mmAddr>]\n"
        "     [/h]\n", arg0);
}

void printHelp(char* arg0)
{
    printVersion();
    printf("\n");
    printUsage(arg0);
    printf("\n");
    printf("/io : encode <bus> <device> <function> <offset> into portio data value\n"
           "/mm : encode <pciexbar> <bus> <device> <function> <offset> into memory mapped io config address\n"
           "/io : decode <portIoAddr> into bus, device, function, offset\n"
           "/mm : decode <mmAddr> into pciexbarbus, bus, device, function, offset\n"
           "\n"
           "All values are expected as hex values.\n");
}

int __cdecl main(int argc, char** argv)
{
    // value to parts
    if ( argc == 2 && ( IS_1C_ARG(argv[1], 'h') || IS_1C_ARG(argv[1], '?')  ))
    {
        printHelp(argv[0]);
    }
    else if ( argc == 3 && IS_2C_ARG(argv[1], 'io') )
    {
        uint32_t portIoAddr = strtoul(argv[2], NULL, 16);
        PRINT_INT_HD(portIoAddr, "");

        portIoAddr = portIoAddr & ~0x80000000;

        uint32_t bus = (portIoAddr >> 0x10) & 0xFF;
        uint32_t device = (portIoAddr >> 0xb) & 0x1F;
        uint32_t function = (portIoAddr >> 0x8) & 0x7;
        uint32_t offset = portIoAddr & 0xFF;
        
        printf("\n");
        PRINT_INT_HD(bus, "");
        PRINT_INT_HD(device, "");
        PRINT_INT_HD(function, "");
        PRINT_INT_HD(offset, "");
    }
    else if ( argc == 3 && IS_2C_ARG(argv[1], 'mm') )
    {
        uint64_t mmAddr = strtoull(argv[2], NULL, 16);
        PRINT_INT_HD_LL(mmAddr, "");

        //uint64_t xbar = mmAddr & ~0x7FF'FFFF;
        //uint32_t bus = ( (mmAddr & 0xFF'0000) >> 0x14) & 0xFF;
        //uint32_t device = ( (mmAddr & 0xf'8000) >> 0xful ) & 0x1F;
        //uint32_t function = ((mmAddr & 0x7000)>> 0xc) & 0x7;
        //uint32_t offset = mmAddr & 0xFFF;
        
        printf("\n");
        //PRINT_INT_HD_LL(xbar, "");
        //PRINT_INT_HD(bus, "");
        //PRINT_INT_HD(device, "");
        //PRINT_INT_HD(function, "");
        //PRINT_INT_HD(offset, "");
        
        MMIO_ADDRESS mmioAddr;
        memcpy(&mmioAddr, &mmAddr, sizeof(mmioAddr));
        printf("PciExBar: 0x%llx\n", (unsigned long long)mmioAddr.PciExBar << 28);
        //printf("PciExBar: 0x%llx (0x%x)\n", mmioAddr.PciExBar<<28, (uint16_t)mmioAddr.PciExBar);
        printf("Bus: 0x%llx\n", (unsigned long long)mmioAddr.Bus);
        printf("Device: 0x%llx\n", (unsigned long long)mmioAddr.Device);
        printf("Function: 0x%llx\n", (unsigned long long)mmioAddr.Function);
        printf("Offset: 0x%llx\n", (unsigned long long)mmioAddr.Offset);
    }
    else if ( argc == 6 && IS_2C_ARG(argv[1], 'io') )
    {
        uint32_t bus = strtoul(argv[2], NULL, 16);
        uint32_t device = strtoul(argv[3], NULL, 16);
        uint32_t function = strtoul(argv[4], NULL, 16);
        uint32_t offset = strtoul(argv[5], NULL, 16);
        
        PRINT_INT_HD(bus, "");
        PRINT_INT_HD(device, "");
        PRINT_INT_HD(function, "");
        PRINT_INT_HD(offset, "");
        printf("\n");

        if ( bus >= 0x100 )
        {
            EPrint("Invalid bus number!\n");
            return ERROR_INVALID_PARAMETER;
        }
        if ( device >= 0x20 )
        {
            EPrint("Invalid device number!\n");
            return ERROR_INVALID_PARAMETER;
        }
        if ( function >= 0x8 )
        {
            EPrint("Invalid function number!\n");
            return ERROR_INVALID_PARAMETER;
        }

        uint32_t portIoAddr = PCI_CFG_PORT_IO_ADDR(bus, device, function, offset);

        PRINT_INT_HD(portIoAddr, "");
    }
    else if ( argc == 7 && IS_2C_ARG(argv[1], 'mm') )
    {
        uint64_t xbar = strtoull(argv[2], NULL, 16);
        uint64_t bus = strtoull(argv[3], NULL, 16);
        uint64_t device = strtoull(argv[4], NULL, 16);
        uint64_t function = strtoull(argv[5], NULL, 16);
        uint64_t offset = strtoull(argv[6], NULL, 16);
        
        PRINT_INT_HD_LL(xbar, "");
        PRINT_INT_HD_LL(bus, "");
        PRINT_INT_HD_LL(device, "");
        PRINT_INT_HD_LL(function, "");
        PRINT_INT_HD_LL(offset, "");
        printf("\n");

        if ( bus >= 0x100 )
        {
            EPrint("Invalid bus number!\n");
            return ERROR_INVALID_PARAMETER;
        }
        if ( device >= 0x20 )
        {
            EPrint("Invalid device number!\n");
            return ERROR_INVALID_PARAMETER;
        }
        if ( function >= 0x8 )
        {
            EPrint("Invalid function number!\n");
            return ERROR_INVALID_PARAMETER;
        }

        //uint64_t mmAddr = PCI_CFG_MM_ADDR(xbar, bus, device, function, offset);
        //PRINT_INT_HD_LL(mmAddr, "");
        
        MMIO_ADDRESS mmioAddr = {
            .PciExBar=(xbar>>28)&0x7ff,
            .Bus=bus,
            .Device=device,
            .Function=function,
            .Offset=offset
        };
        uint64_t rawAddr;
        memcpy(&rawAddr, &mmioAddr, sizeof(rawAddr));
        printf("mmioAddr: 0x%llx\n", (unsigned long long)rawAddr);
    }
    else
    {
        printUsage(argv[0]);
        return ERROR_INVALID_PARAMETER;
    }
    
    return 0;
}
