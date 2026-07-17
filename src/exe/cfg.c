#pragma warning( push )
#include "../warnings_ntifs_wdm.h"
#include <ntifs.h>
#include <ntstrsafe.h>
#pragma warning( pop )


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "inc/acpi.h"
#include "inc/nt.h"
#include "inc/win.h"
//#include "../MemIoShared.h" // included in memoryIo.h
#include "../CfgShared.h"
#include "../print.h"
#include "helper/deviceIo.h"
#include "helper/Converter.h"
#include "helper/Args.h"
#include "../common/kfileio.h"
#include "helper/filesW.h"
#include "helper/fileio.h"
//#include "helper/virtmem.h"


#define CACHE_TYPE (MmNonCached)

// maximum bar size to include into cfg log
// binaries will be written nonetheless
#define MAX_BAR_SIZE_TO_LOG 0x100

#include "../inc/pci/pci.h"
#include "../inc/pci/pciDeviceCfg-pch300series.h"
#include "../common/strings.h"
#include "../pci/pciids.h"
#include "memoryIo.h"

#include "listPciReg.h"
#include "bars.h"

#include "../common/log.h"
#include "log/logCapabilities.h"
#include "log/logCfg.h"
#include "log/logBar.h"


#define APP_NAME "CfgIO"
#define BIN_NAME APP_NAME ".exe"
#define APP_VS "1.1.2"
#define APP_LC "15.07.2026"

#define DEF_LOG_DIR_A "C:\\logs"
//#define DEF_LOG_FILE_TYPE L".log"

#define PCI_CONFIG_SIZE (0x100)
#define PCIE_CONFIG_SIZE (0x1000)

#define NO_OFFSET ((ULONG)-1)

#define MODE_READ_CFG       (0x1)
#define MODE_WRITE_CFG      (0x2)
#define MODE_DUMP_CFG       (0x3)
#define MODE_LIST_PCI_REG   (0x4)
#define MODE_LIST_PCI_IO    (0x5)
#define MODE_DUMP_PCI       (0x6)
// ...
#define MODE_MAX            (0xf)

#define LIST_TYPE_REG       (0x0)
#define LIST_TYPE_IO        (0x1)
#define LIST_TYPE_MAX       (0x2)

#define MODE_PCI_IO_MIN MODE_READ_CFG
#define MODE_PCI_IO_MAX MODE_DUMP_PCI

//#pragma pack(1)

typedef struct _PCI_IO {
    UINT64 PciExBarAddress; // 00
    UINT64 PciExBarSize; // 08
    BDF BDF; // 10
    UINT16 Offset; // 16
    ULONG Size; // 18
    PCI_READ_IN_FLAGS Flags; // 2C
    PUINT8 Data; // 30
    UINT32 Rcbo; // 38
    UINT16 BarId; // 3C
} PCI_IO, *PPCI_IO; // 3E

typedef struct _FLAGS {
    UINT32 Verbose:1;
    UINT32 PrintPlain:1;
    UINT32 Print:1;
    UINT32 WriteToFile:1;
    UINT32 Mode:4;
    UINT32 Reserved:24;
} FLAGS, *PFLAGS;

typedef struct _CMD_PARAMS {
    PWCHAR LogDir; // 00
    ULONG LogDirCb; // 08
    FLAGS Flags; // 0C
    ULONG OutBufferSize; // 10
    PCI_IO PciIo; // 14
} CMD_PARAMS, *PCMD_PARAMS; // 52

//#pragma pack()

#include "cfg.h"
#include "listPciIo.h"

INT printPciConfig(_In_ PVOID CfgBuffer, _In_ ULONG CfgBufferSize);

VOID printLPciEntryList(
    _In_ PVOID OutputBuffer, 
    _In_ ULONG Size
);

VOID printCfgDataList(
    _In_ PVOID Buffer, 
    _In_ ULONG Size,
    _In_ ULONG Count
);

NTSTATUS dumpPci(
    _In_ HANDLE Device,
    _In_ PCMD_PARAMS Params
);

INT dumpPciConfig(
    _In_ HANDLE Device, 
    _In_ PPCI_IO PciIoParams,
    _In_ PWCHAR LogDir,
    _In_ UINT32 LogDirCb,
    _In_ FLAGS Flags
);

INT readPciConfig32(
    _In_ HANDLE Device, 
    _In_ PPCI_IO PciIoParams
);

INT writePciConfig32(
    _In_ HANDLE Device, 
    _In_ PPCI_IO PciIoParams
);



int parseParams(
    int argc, 
    char** argv, 
    PCMD_PARAMS Params,
    int start_i
);

int checkParams(
    PCMD_PARAMS Params
);

void printParams(
    PCMD_PARAMS Params
);

void printUsage();

void printHelp();



int _cdecl main(int argc, char** argv)
{
    int s = 0;

    HANDLE device = NULL;
    
    CMD_PARAMS params;

    PUINT8 outputBuffer = NULL;
    ULONG outputBufferSize = 0;

    ULONG bytesReturned = 0;



    if ( isAskForHelp(argc, argv) )
    {
        printHelp();
        return 0;
    }

    if ( argc < 2 )
    {
        printUsage();
        return -1;
    }

    RtlZeroMemory(&params, sizeof(params));

    s = parseParams(argc, argv, &params, 1);
    if ( s != 0 )
    {
        printUsage();
        goto clean;
    }

    s = checkParams(&params);
    if ( s != 0 )
    {
        printUsage();
        goto clean;
    }

    if ( params.Flags.Verbose )
        printParams(&params);


    if ( params.Flags.Mode != MODE_LIST_PCI_REG )
    {
        s = openDevice(&device, DEVICE_BASE_NAME_W);
        if ( s != 0 )
            goto clean;
    }

    outputBufferSize = (params.OutBufferSize)?params.OutBufferSize:PAGE_SIZE;
    SUPPRESS_WARNING( 6014 );
    outputBuffer = malloc(outputBufferSize);
    if ( !outputBuffer )
    {
        outputBufferSize = 0;
        goto clean;
    }

    
    //
    // create log dir if not present

    if ( params.LogDir 
      && params.Flags.WriteToFile )
    {
        s = createLogDir(params.LogDir);
        if ( s != 0 )
            goto clean;
    }
    

    //
    // handle command mode

    switch ( params.Flags.Mode )
    {
        // dump a full pci config
        // rw / memoryIo.h
        case MODE_DUMP_CFG:
        {
            s = dumpPciConfig(device, &params.PciIo, params.LogDir, params.LogDirCb, params.Flags);
            
            break;
        }
        
        // read 4 bytes of pci config
        // rw / memoryIo.h
        case MODE_READ_CFG:
        {
            s = readPciConfig32(device, &params.PciIo);
            
            if ( s != 0 )
                break;
            
            printf("value: 0x%x", *(PUINT32)params.PciIo.Data);
            
            break;
        }
        
        // write 4 bytes to pci config
        // rw / memoryIo.h
        case MODE_WRITE_CFG:
        {
            s = writePciConfig32(device, &params.PciIo);

            break;
        }

        // list pci devices
        // registry io
        case MODE_LIST_PCI_REG:
        {
            s = listPciReg(BDF_FLAG_SKIP_DOUBLE, outputBuffer, outputBufferSize, &bytesReturned);
            if ( s != 0 || bytesReturned < LIST_PCI_OUT_DEF_SIZE )
            {
                EPrint("listPciReg failed! (0x%x)\n", s);
                goto clean;
            }
            PLIST_PCI_OUT out = (PLIST_PCI_OUT)outputBuffer;
            if ( bytesReturned < out->Size )
            {
                UINT32 reqSize = out->Size;
                DPrint("Output buffer too small! Allocation new 0x%x buffer!\n", reqSize);
                if ( outputBuffer )
                    free(outputBuffer);
                outputBufferSize = reqSize;
                outputBuffer = malloc(outputBufferSize);
                if ( !outputBuffer )
                    goto clean;
                s = listPciReg(BDF_FLAG_SKIP_DOUBLE, outputBuffer, outputBufferSize, &bytesReturned);
                if ( s != 0 || bytesReturned < LIST_PCI_OUT_DEF_SIZE )
                {
                    EPrint("listPciReg failed! (0x%x)\n", s);
                    goto clean;
                }
            }

            printLPciEntryList(outputBuffer, bytesReturned);

            break;
        }
        
        // list pci devices
        // rw / memoryIo.h
        case MODE_LIST_PCI_IO:
        {
            UINT32 count = 0;
            s = listPciIo(device, &params, &outputBuffer, (PUINT32)&outputBufferSize, &count);

            if ( s != 0 )
                break;

            printCfgDataList(outputBuffer, outputBufferSize, count);

            break;
        }
        
        // dump all found pci device configs
        // registry io / rw / memoryIo.h
        case MODE_DUMP_PCI:
        {
            //ioctl = IOCTL_DUMP_PCI;
            s = dumpPci(device, &params);
            break;
        }

        default:
            printf("No valid mode set!\n");
            goto clean;
    }
    
#ifdef DEBUG_PRINT
    if ( bytesReturned > 0 )
    {
        DPrint("Plain bytes returned:\n");
        DPrintMemCol8(outputBuffer, bytesReturned, 0);
    }
#endif
    
    //switch ( params.Flags.Mode )
    //{
    //    default:
    //        break;
    //}

clean:
    if ( device )
        NtClose(device);
    if ( outputBuffer)
        free(outputBuffer);
    if ( params.PciIo.Data )
        free(params.PciIo.Data);
    if ( params.LogDir )
        free(params.LogDir);

    return s;
}

int compareLPciEntryAsc(const void *arg1, const void *arg2)
{
    //FEnter();
    
    PLPCI_ENTRY e1 = (PLPCI_ENTRY)arg1;
    PLPCI_ENTRY e2 = (PLPCI_ENTRY)arg2;
    UINT32 v1 = (((UINT32)e1->Bdf.Bus<<0x10ul) | ((UINT32)e1->Bdf.Device<<0x8ul) | (e1->Bdf.Function))&0xFFFFFF;
    UINT32 v2 = (((UINT32)e2->Bdf.Bus<<0x10ul) | ((UINT32)e2->Bdf.Device<<0x8ul) | (e2->Bdf.Function))&0xFFFFFF;
    //DPrint("bdf1: %02x:%02x:%02x (0x%06lx)\n", bdf1->Bdf.Bus, bdf1->Bdf.Device, bdf1->Bdf.Function, v1);
    //DPrint("bdf2: %02x:%02x:%02x (0x%06lx)\n", bdf2->Bdf.Bus, bdf2->Bdf.Device, bdf2->Bdf.Function, v2);
    
    //FLeave();
    return (int)(v1 - v2); 
}

VOID printLPciEntryList(
    _In_ PVOID OutputBuffer, 
    _In_ ULONG Size
)
{
    FEnter();
    if ( Size < LIST_PCI_OUT_DEF_SIZE )
        return;

    PLIST_PCI_OUT out = (PLIST_PCI_OUT)OutputBuffer;

    if ( Size < out->Size )
    {
        printf("[i] Output buffer too small! 0x%x bytes needed (/obs 0x%x)!\n", out->Size, out->Size);
        return;
    }

    PLPCI_ENTRY entryList = (PLPCI_ENTRY)&out->Buffer[out->BDFOffset];

    qsort(entryList, out->Count, sizeof(LPCI_ENTRY), compareLPciEntryAsc);

    printf("\n");
    printf("Registered pci devices (%u):\n", out->Count);
    for ( UINT32 i = 0; i < out->Count; i++ )
    {
        PLPCI_ENTRY e = &entryList[i];
                
        printf("[%u] %02x:%02x:%02x : %s\n", 
            i, 
            e->Bdf.Bus, e->Bdf.Device, e->Bdf.Function,
            e->Name);
    }

    FLeave();
}

int compareCFGDataAsc( const void *arg1, const void *arg2 )
{
    //FEnter();
    
    PCFG_DATA e1 = (PCFG_DATA)arg1;
    PCFG_DATA e2 = (PCFG_DATA)arg2;
    UINT32 v1 = (((UINT32)e1->Bdf.Bus<<0x10ul) | ((UINT32)e1->Bdf.Device<<0x8ul) | (e1->Bdf.Function))&0xFFFFFF;
    UINT32 v2 = (((UINT32)e2->Bdf.Bus<<0x10ul) | ((UINT32)e2->Bdf.Device<<0x8ul) | (e2->Bdf.Function))&0xFFFFFF;
    //DPrint("bdf1: %02x:%02x:%02x (0x%06lx)\n", e1->Bdf.Bus, e1->Bdf.Device, e1->Bdf.Function, v1);
    //DPrint("bdf2: %02x:%02x:%02x (0x%06lx)\n", e2->Bdf.Bus, e2->Bdf.Device, e2->Bdf.Function, v2);
    
    //FLeave();
    return (int)(v1 - v2); 
}

VOID printCfgDataList(
    _In_ PVOID Buffer, 
    _In_ ULONG Size,
    _In_ ULONG Count
)
{
    FEnter();
    (Size);

    PCFG_DATA data = (PCFG_DATA)Buffer;
    
    qsort(data, Count, sizeof(CFG_DATA), compareCFGDataAsc);

    for ( UINT32 i = 0; i < Count; i++ )
    {
        PCFG_DATA entry = &data[i];
                
        printf("[%.2u] %02x:%02x:%02x 0x%04x (%s) 0x%04x (%s)\n", 
            i, entry->Bdf.Bus, entry->Bdf.Device, entry->Bdf.Function, 
            entry->VendorId, vendorIdStr(entry->VendorId), 
            entry->DeviceId, deviceIdStr(entry->VendorId, entry->DeviceId));
    }

    FLeave();
}

NTSTATUS dumpPci(
    _In_ HANDLE Device,
    _In_ PCMD_PARAMS Params
)
{
    FEnter();
    
    NTSTATUS status = 0;
    
    PLPCI_ENTRY pciEntryList = NULL;
    UINT32 pciEntryCount = 0;
    PCHAR names = NULL;
    UINT32 namesSize = 0;


    //
    // get all pci BDFs via registry

    status = enumPci(&pciEntryList, &pciEntryCount, &names, &namesSize, BDF_FLAG_SKIP_DOUBLE);
    if ( status != 0 || !pciEntryList || !names )
    {
        EPrint("enumPci failed! (0x%x)\n", status);
        goto clean;
    }
    if ( pciEntryCount == 0 )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("No PCI devices found! (0x%x)\n", status);
        goto clean;
    }
    
    // write name pointer into entry list for convenience (e.g. sorting)
    PCHAR name = names;
    for ( UINT32 i = 0; i < pciEntryCount; i++ )
    {
        PLPCI_ENTRY e = &pciEntryList[i];
        e->Name = name;
        name += strlen(name)+1;
    }
    
    qsort(pciEntryList, pciEntryCount, sizeof(LPCI_ENTRY), compareLPciEntryAsc);

    
    //
    // write output list

    printf("\n");
    printf("Registered pci devices (%u):\n", pciEntryCount);
    for ( UINT32 i = 0; i < pciEntryCount; i++ )
    {
        PLPCI_ENTRY e = &pciEntryList[i];
                
        printf("[%u] %02x:%02x:%02x : %s\n", 
            i, 
            e->Bdf.Bus, e->Bdf.Device, e->Bdf.Function,
            e->Name);
    }
    printf("\n");
    
    //ioctl = IOCTL_PCI_READ;
#ifdef DEBUG_PRINT
    name = names;
#endif
    
    //
    // iterate list and dump configs

    for ( UINT32 i = 0; i < pciEntryCount; i++ )
    {
        PLPCI_ENTRY e = &pciEntryList[i];
        
        // use some global settings and add bdf
        PPCI_IO pciIoParams = &Params->PciIo;
        pciIoParams->BDF = e->Bdf;
        
        printf("Reading %02x:%02x:%02x : %s\n", 
                e->Bdf.Bus, e->Bdf.Device, e->Bdf.Function, e->Name);

        status = dumpPciConfig(Device, pciIoParams, Params->LogDir, Params->LogDirCb, Params->Flags);
        if ( status != 0 )
        {
            EPrint("Dumping PCI Config failed! (0x%x)\n", status);
            goto skipBDF;
        }
        
    skipBDF:
        ;
    }

clean:
    if ( pciEntryList )
        ExFreePool(pciEntryList);
    if ( names )
        ExFreePool(names);

    return status;
}

//
// Read PciExBarValue, and set the ReadMode depending on it's result
//
INT setPciExBarAddressMode(
    _In_ HANDLE Device, 
    _In_ PPCI_IO PciIoParams
)
{
    FEnter();

    NTSTATUS status = 0;
    
    UINT64 pciExBarAddress = 0;
    UINT64 pciExBarSize = 0;

    if ( PciIoParams->PciExBarAddress )
    {
        //pciExBarAddress = PciIoParams->PciExBarAddress;
        //pciExBarSize = PciIoParams->PciExBarSize;
        PciIoParams->Flags.ReadMode = READ_CFG_MODE_MM;
        status = 0;
    }
    else
    {
        status = getPciExBarValueMCFG(Device, &pciExBarAddress, &pciExBarSize);
        if ( status != 0 )
        {
            EPrint("getPciExBarValueMCFG failed! (0x%x)\n", status);
            status = getPciExBarValuePortIo(Device, &pciExBarAddress, &pciExBarSize);
            
            if ( status != 0 )
            {
                EPrint("getPciExBarValuePortIo failed! (0x%x)\n", status);
                if ( PciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
                {
                    goto clean;
                }
                else
                {
                    PciIoParams->Flags.ReadMode = READ_CFG_MODE_PORT;
                    status = 0;
                }
            }
            else
            {
                goto exbarSuccess;
            }
        }
        else
        {
        exbarSuccess:
            DPrint("pciExBarAddress: 0x%llx\n", pciExBarAddress);
            DPrint("pciExBarSize: 0x%llx\n", pciExBarSize);
        
            PciIoParams->PciExBarAddress = pciExBarAddress;
            PciIoParams->PciExBarSize = pciExBarSize;
            PciIoParams->Flags.ReadMode = READ_CFG_MODE_MM;
        }
    }

clean:

    FLeave();
    return status;
}

INT dumpPciConfig(
    _In_ HANDLE Device, 
    _In_ PPCI_IO PciIoParams,
    _In_ PWCHAR LogDir,
    _In_ UINT32 LogDirCb,
    _In_ FLAGS Flags
)
{
    NTSTATUS status = 0;
    INT s = 0;

    FEnter();

    UINT64 barBufferSize = 0;
    PVOID barBuffer = NULL;

    PBDF bdf = &PciIoParams->BDF;
    PVOID cfgBuffer = NULL;
    UINT32 cfgBufferSize = 0;

    LOG log = { 0 };
    log.Dir = LogDir;
    log.BufferSize = PAGE_SIZE;
    
    SIZE_T logBufferRestSize = 0;
    PCHAR logBufferPtr = NULL;

    PWCHAR filePath = NULL;
    UINT32 filePathCch = 0;
    HANDLE logFile = NULL;
    
    UINT64 pciExBarAddress = 0;
    UINT64 pciExBarSize = 0;
    
    DPrint("LogDir: %ws\n", LogDir);
    DPrint("Flags: 0x%x\n", *(PUINT32)&Flags);


    if ( PciIoParams->Flags.PCIe )
        cfgBufferSize = PCIE_CONFIG_SIZE;
    else
        cfgBufferSize = PCI_CONFIG_SIZE;

    cfgBuffer = malloc(cfgBufferSize);
    if ( !cfgBuffer )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }
    
    if ( Flags.WriteToFile )
    {
        log.Buffer = malloc(log.BufferSize);
        if ( !log.Buffer )
            goto clean;

        logBufferRestSize = log.BufferSize;
        logBufferPtr = log.Buffer;
    }
    
    if ( PciIoParams->Flags.ReadMode == 0
      || PciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
    {
        status = setPciExBarAddressMode(Device, PciIoParams);
        if ( status != 0 )
        {
            goto clean;
        }

        pciExBarAddress = PciIoParams->PciExBarAddress;
        pciExBarSize = PciIoParams->PciExBarSize;
        //if ( PciIoParams->PciExBarAddress )
        //{
        //    pciExBarAddress = PciIoParams->PciExBarAddress;
        //    pciExBarSize = PciIoParams->PciExBarSize;
        //    PciIoParams->Flags.ReadMode = READ_CFG_MODE_MM;
        //    status = 0;
        //}
        //else
        //{
        //    status = getPciExBarValueMCFG(Device, &pciExBarAddress, &pciExBarSize);
        //    if ( status != 0 )
        //    {
        //        EPrint("getPciExBarValueMCFG failed! (0x%x)\n", status);
        //        status = getPciExBarValuePortIo(Device, &pciExBarAddress, &pciExBarSize);
        //    
        //        if ( status != 0 )
        //        {
        //            EPrint("getPciExBarValuePortIo failed! (0x%x)\n", status);
        //            if ( PciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
        //                goto clean;
        //            else
        //                PciIoParams->Flags.ReadMode = READ_CFG_MODE_PORT;
        //        }
        //        else
        //        {
        //            goto exbarSuccess;
        //        }
        //    }
        //    else
        //    {
        //    exbarSuccess:
        //        DPrint("pciExBarAddress: 0x%llx\n", pciExBarAddress);
        //        DPrint("pciExBarSize: 0x%llx\n", pciExBarSize);
        //
        //        PciIoParams->PciExBarAddress = pciExBarAddress;
        //        PciIoParams->PciExBarSize = pciExBarSize;
        //        PciIoParams->Flags.ReadMode = READ_CFG_MODE_MM;
        //    }
        //}
    }

    DPrint("Reading config\n");
    if ( PciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
    {
        DPrint("mm io\n");
        UINT64 pa = PCI_CFG_MM_ADDR(pciExBarAddress, bdf->Bus, bdf->Device, bdf->Function, 0);
        DPrint("pa: %p\n", (PVOID)pa);
        // should be 4 byte aligned !!
        //status = readPA(Device, pa, CACHE_TYPE, cfgBuffer, cfgBufferSize, READ_PA_MODE_DEF);
        status = readPA(Device, pa, CACHE_TYPE, cfgBuffer, cfgBufferSize, READ_PA_MODE_CHUNKED_4);
        if ( status != 0 )
        {
            EPrint("readPA config %02x:%02x:%02x failed! (0x%x)\n", bdf->Bus, bdf->Device, bdf->Function, status);
            goto clean;
        }
    }
    else
    {
        DPrint("port io\n");
        ULONG parts = cfgBufferSize / 4;
        ULONG rest = cfgBufferSize % 4;

        PUINT32 bufferPtr = (PUINT32)cfgBuffer;
        UINT32 index = 0;
        UINT32 offset = 0;
        UINT32 value32 = 0;

        if ( rest != 0 )
        {
            status = STATUS_INVALID_PARAMETER_3;
            EPrint("Length has to be 4-Byte aligned! (0x%x)\n", status);
            goto clean;
        }
    
        //RtlZeroMemory(cfgBuffer, cfgBufferSize);
        
        // read in 32 bit chunks
        for ( UINT32 i = 0; i < parts; i++ )
        {
            index = PCI_CFG_PORT_IO_ADDR(bdf->Bus, bdf->Device, bdf->Function, offset);
            DPrint("i: 0x%x\n", i);
            DPrint("  index: 0x%x\n", index);
            status = outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, index, &value32);
            if ( status != 0 )
            {
                EPrint("outIn32 0x%x:0x%x => 0x%x failed! (0x%x)\n", PCI_CONFIG_COMMAND_PORT, index, PCI_CONFIG_STATUS_PORT, status);
                goto clean;
            }
        
            *bufferPtr = value32;

            ++bufferPtr;
            offset += 4;
        }
    }

    DPrint("cfgBuffer:\n");
    DPrintMemCol8(cfgBuffer, cfgBufferSize, 0);

    if ( Flags.Print )
    {
        printf("\n");
        s = printPciConfig(cfgBuffer, cfgBufferSize);
#ifndef DEBUG_PRINT
        if ( s == 1 || Flags.Verbose )
        {
            printf("\n");
            PrintMemCol8(cfgBuffer, cfgBufferSize, 0);
        }
#endif
    }
    if ( Flags.WriteToFile )
    {
        filePathCch = (LogDirCb/2) + 0x11;
        filePath = malloc(filePathCch*2 + 2);
        if ( !filePath )
            goto clean;

        RtlStringCchPrintfW(filePath, filePathCch+1, L"%s\\cfg-%02x-%02x-%02x.log",
            log.Dir, bdf->Bus, bdf->Device, bdf->Function);

        s = kOpenFile(filePath, filePathCch, &logFile, OPEN_FOR_WRITE_ONLY, FILE_OVERWRITE_IF, 0);
        if ( s != 0 )
        {
            EPrint("Opening file \"%ws\" failed! (0x%x)\n", filePath, s);
            goto clean;
        }
        log.File = logFile;

        s = logCfg(bdf, cfgBuffer, cfgBufferSize, &log);
        if ( s != 0 )
        {
            EPrint("Writing config to file failed! (0x%x)\n", s);
            goto clean;
        }
    }

    // 
    // read bars

    if ( PciIoParams->Flags.ReadBars )
    {
        DPrint("Reading bars\n");

        PPCI_CONFIG cfg = (PPCI_CONFIG)cfgBuffer;
        UINT32 barsStartOffset = 0;
        PBAR bar = NULL;
        PBAR nextBar = NULL;
        UINT64 barSize = 0;

        if ( cfg->HeaderType.Id == PCI_HEADER_GENERAL_DEVICE )
        {
            PPCI_CONFIG_GENERAL pciCfgGeneral = (PPCI_CONFIG_GENERAL)cfgBuffer;
            barsStartOffset = (UINT32)offsetof(PCI_CONFIG_GENERAL, Bar[0]);

            for ( ULONG barId = 0; barId < PCI_TYPE0_ADDRESSES; barId++ )
            {
                bar = (PBAR)&pciCfgGeneral->Bar[barId];
                nextBar = (barId+1<PCI_TYPE0_ADDRESSES)?(PBAR)&pciCfgGeneral->Bar[barId+1]:NULL;
                DPrint("Bar[%u]: 0x%x\n", barId, *(PUINT32)bar);
                
                status = readBar(Device, pciExBarAddress, &PciIoParams->BDF, barsStartOffset, barId, bar, nextBar, &barBuffer, &barBufferSize, &barSize);
                if ( status != 0 )
                {
                    EPrint("Reading bar[%u] failed! (0x%x)\n", barId, status);
                    continue;
                }

                if ( Flags.WriteToFile && barSize )
                {
                    PVOID baseAddress = getBarBaseAddress(bar, nextBar);

                    writeBarToFile(&PciIoParams->BDF,
                                   baseAddress,
                                   barId,
                                   bar,
                                   barSize,
                                   barBuffer,
                                   &log
                                );
                }

                if ( IS_64BIT_BAR(bar) )
                    barId++;
            }
        }
        else if ( cfg->HeaderType.Id == PCI_HEADER_PCI_TO_PCI_BRIDGE )
        {
            PPCI_CONFIG_PCI_BRIDGE pciCfgPciBridge = (PPCI_CONFIG_PCI_BRIDGE)cfgBuffer;
            barsStartOffset = (UINT32)offsetof(PCI_CONFIG_PCI_BRIDGE, Bar[0]);
        
            for ( ULONG barId = 0; barId < PCI_TYPE1_ADDRESSES; barId++ )
            {
                bar = (PBAR)&pciCfgPciBridge->Bar[barId];
                nextBar = (barId+1<PCI_TYPE1_ADDRESSES)?(PBAR)&pciCfgPciBridge->Bar[barId+1]:NULL;
                DPrint("Bar[%u]: 0x%x\n", barId, *(PUINT32)bar);
                
                status = readBar(Device, pciExBarAddress, &PciIoParams->BDF, barsStartOffset, barId, bar, nextBar, &barBuffer, &barBufferSize, &barSize);
                if ( status != 0 )
                {
                    EPrint("Reading bar[%u] failed! (0x%x)\n", barId, status);
                    continue;
                }
                
                if ( Flags.WriteToFile && barSize )
                {
                    PVOID baseAddress = getBarBaseAddress(bar, nextBar);

                    writeBarToFile(&PciIoParams->BDF,
                                   baseAddress,
                                   barId,
                                   bar,
                                   barSize,
                                   barBuffer,
                                   &log
                                );
                }

                if ( IS_64BIT_BAR(bar) )
                    barId++;
            }
        }
        else if ( cfg->HeaderType.Id == PCI_HEADER_PCI_TO_CARDBUS_BRIDGE
               && cfgBufferSize >= 0x48 )
        {
            //PPCI_CONFIG_PCI_CB_BRIDGE pciConfigPciCbBridge = (PPCI_CONFIG_PCI_CB_BRIDGE)CfgBuffer;
        
            // sober
        }
    }

    if ( PciIoParams->Flags.ReadCapabilities )
    {
        DPrint("Reading Capabilities\n");

        if ( Flags.WriteToFile )
        {
            status = logCapabilities(Device, cfgBuffer, cfgBufferSize, *(PUINT32)&PciIoParams->Flags, &log);
            if ( status != 0 )
            {
                EPrint("Reading Capabilities failed! (0x%x)\n", status);
                goto clean;
            }
        }
        else
        {
            printf("[i] Currently capabilities are just logged to file. You have to set the '/wtf' param for this to work!\n");
        }
    }

clean:
    if ( log.Buffer )
        free(log.Buffer);
    if ( filePath )
        free(filePath);
    if ( cfgBuffer )
        free(cfgBuffer);
    if ( barBuffer )
        NtFreeVirtualMemory(
                (HANDLE)-1,
                &barBuffer,
                &barBufferSize,
                MEM_RELEASE
            );

    FLeave();

    return status;
}

INT printPciConfig(_In_ PVOID CfgBuffer, _In_ ULONG CfgBufferSize)
{
    NTSTATUS status = 0;

    //PPCI_CONFIG cfg = (PPCI_CONFIG)CfgBuffer;

    if ( CfgBufferSize < 0x40 )
        return STATUS_UNSUCCESSFUL;
    if ( isReadDenied(CfgBuffer, 0x40) )
    {
        printf("[i] Reading denied!\n");
        return STATUS_UNSUCCESSFUL;
    }

    UINT32 bufferSize = 0x1000;
    PCHAR buffer = (PCHAR)malloc(bufferSize);
    if ( !buffer )
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean;
    }
    PCHAR bufferPtr = buffer;
    SIZE_T bufferRestSize = bufferSize;


    status = logPciConfig(CfgBuffer, CfgBufferSize, NULL, buffer, bufferSize, &bufferPtr, &bufferRestSize);
    if ( status != 0 )
        goto clean;

    printf("%s", buffer);

clean:
    if ( buffer )
        free(buffer);

    return status;
}

//
// Read a 32 bit value of a pci config.
//
INT readPciConfig32(
    _In_ HANDLE Device, 
    _In_ PPCI_IO PciIoParams
)
{
    FEnter();

    NTSTATUS status = 0;

    PBDF bdf = &PciIoParams->BDF;
    UINT16 offset = PciIoParams->Offset;
    UINT32 value = 0;

    UINT64 pciExBarAddress = 0;
    UINT64 pciExBarSize = 0;

    if ( offset % 4 != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Offset is not 4 byte aligned! (0x%x)\n", status);
        goto clean;
    }
    
    //
    // get PciExBar value, if needed
    // 
    if ( PciIoParams->Flags.ReadMode == 0
      || PciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
    {
        status = setPciExBarAddressMode(Device, PciIoParams);
        if ( status != 0 )
        {
            goto clean;
        }

        pciExBarAddress = PciIoParams->PciExBarAddress;
        pciExBarSize = PciIoParams->PciExBarSize;
        //if ( PciIoParams->PciExBarAddress )
        //{
        //    pciExBarAddress = PciIoParams->PciExBarAddress;
        //    pciExBarSize = PciIoParams->PciExBarSize;
        //    PciIoParams->Flags.ReadMode = READ_CFG_MODE_MM;
        //    status = 0;
        //}
        //else
        //{
        //    status = getPciExBarValueMCFG(Device, &pciExBarAddress, &pciExBarSize);
        //    if ( status != 0 )
        //    {
        //        EPrint("getPciExBarValueMCFG failed! (0x%x)\n", status);
        //        status = getPciExBarValuePortIo(Device, &pciExBarAddress, &pciExBarSize);
        //    
        //        if ( status != 0 )
        //        {
        //            EPrint("getPciExBarValuePortIo failed! (0x%x)\n", status);
        //            if ( PciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
        //                goto clean;
        //            else
        //                PciIoParams->Flags.ReadMode = READ_CFG_MODE_PORT;
        //        }
        //        else
        //        {
        //            goto exbarSuccess;
        //        }
        //    }
        //    else
        //    {
        //    exbarSuccess:
        //        DPrint("pciExBarAddress: 0x%llx\n", pciExBarAddress);
        //        DPrint("pciExBarSize: 0x%llx\n", pciExBarSize);
        //
        //        PciIoParams->PciExBarAddress = pciExBarAddress;
        //        PciIoParams->PciExBarSize = pciExBarSize;
        //        PciIoParams->Flags.ReadMode = READ_CFG_MODE_MM;
        //    }
        //}
    }

    //
    // read 32 bit value depending on method, memory mapped or port io'ed
    // 
    if ( PciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
    {
        DPrint("mm io\n");
        UINT64 pa = PCI_CFG_MM_ADDR(pciExBarAddress, bdf->Bus, bdf->Device, bdf->Function, offset);
        DPrint("pa: %p\n", (PVOID)pa);
        status = readPA(Device, pa, CACHE_TYPE, &value, sizeof(value), READ_PA_MODE_4);
        if ( status != 0 )
        {
            EPrint("readPA config %02x:%02x:%02x:%02x failed! (0x%x)\n", bdf->Bus, bdf->Device, bdf->Function, offset, status);
            goto clean;
        }
    }
    else
    {
        DPrint("port io\n");

        UINT32 index = PCI_CFG_PORT_IO_ADDR(bdf->Bus, bdf->Device, bdf->Function, offset);
        DPrint("index: 0x%x\n", index);
        status = outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, index, &value);
        if ( status != 0 )
        {
            EPrint("outIn32 0x%x:0x%x => 0x%x failed! (0x%x)\n", PCI_CONFIG_COMMAND_PORT, index, PCI_CONFIG_STATUS_PORT, status);
            goto clean;
        }
    }

    *(PUINT32)PciIoParams->Data = value;

clean:

    FLeave();
    return status;
}

//
// Write a 32 bit value to a pci config.
//
INT writePciConfig32(
    _In_ HANDLE Device, 
    _In_ PPCI_IO PciIoParams
)
{
    FEnter();

    NTSTATUS status = 0;

    PBDF bdf = &PciIoParams->BDF;
    UINT16 offset = PciIoParams->Offset;
    UINT32 value = *(PUINT32)PciIoParams->Data;

    UINT64 pciExBarAddress = 0;
    UINT64 pciExBarSize = 0;
    
    if ( offset % 4 != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Offset is not 4 byte aligned! (0x%x)\n", status);
        goto clean;
    }
    
    //
    // get PciExBar value, if needed
    // 
    if ( PciIoParams->Flags.ReadMode == 0
      || PciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
    {
        status = setPciExBarAddressMode(Device, PciIoParams);
        if ( status != 0 )
        {
            goto clean;
        }

        pciExBarAddress = PciIoParams->PciExBarAddress;
        pciExBarSize = PciIoParams->PciExBarSize;
        //if ( PciIoParams->PciExBarAddress )
        //{
        //    pciExBarAddress = PciIoParams->PciExBarAddress;
        //    pciExBarSize = PciIoParams->PciExBarSize;
        //    PciIoParams->Flags.ReadMode = READ_CFG_MODE_MM;
        //    status = 0;
        //}
        //else
        //{
        //    status = getPciExBarValueMCFG(Device, &pciExBarAddress, &pciExBarSize);
        //    if ( status != 0 )
        //    {
        //        EPrint("getPciExBarValueMCFG failed! (0x%x)\n", status);
        //        status = getPciExBarValuePortIo(Device, &pciExBarAddress, &pciExBarSize);
        //    
        //        if ( status != 0 )
        //        {
        //            EPrint("getPciExBarValuePortIo failed! (0x%x)\n", status);
        //            if ( PciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
        //                goto clean;
        //            else
        //                PciIoParams->Flags.ReadMode = READ_CFG_MODE_PORT;
        //        }
        //        else
        //        {
        //            goto exbarSuccess;
        //        }
        //    }
        //    else
        //    {
        //    exbarSuccess:
        //        DPrint("pciExBarAddress: 0x%llx\n", pciExBarAddress);
        //        DPrint("pciExBarSize: 0x%llx\n", pciExBarSize);
        //
        //        PciIoParams->PciExBarAddress = pciExBarAddress;
        //        PciIoParams->PciExBarSize = pciExBarSize;
        //        PciIoParams->Flags.ReadMode = READ_CFG_MODE_MM;
        //    }
        //}
    }
    
    //
    // write 32 bit value depending on method, memory mapped or port io'ed
    // 
    if ( PciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
    {
        DPrint("mm io\n");
        UINT64 pa = PCI_CFG_MM_ADDR(pciExBarAddress, bdf->Bus, bdf->Device, bdf->Function, offset);
        DPrint("pa: %p\n", (PVOID)pa);
        status = writePA(Device, pa, CACHE_TYPE, &value, sizeof(value), WRITE_PA_MODE_4);
        if ( status != 0 )
        {
            EPrint("writePA config %02x:%02x:%02x:%02x failed! (0x%x)\n", bdf->Bus, bdf->Device, bdf->Function, offset, status);
            goto clean;
        }
    }
    else
    {
        DPrint("port io\n");
        
        UINT32 index = PCI_CFG_PORT_IO_ADDR(bdf->Bus, bdf->Device, bdf->Function, offset);
        DPrint("index: 0x%x\n", index);
        status = outOut32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, index, value);
        if ( status != 0 )
        {
            EPrint("outOut32 0x%x:0x%x => 0x%x failed! (0x%x)\n", PCI_CONFIG_COMMAND_PORT, index, PCI_CONFIG_STATUS_PORT, status);
            goto clean;
        }
    }

clean:

    FLeave();
    return status;
}

int parseParams(
    int argc, 
    char** argv, 
    PCMD_PARAMS Params,
    int start_i
)
{
    int i = 0;
    char* arg = NULL;
    char *val1 = NULL;
    char *val2 = NULL;
    char *val3 = NULL;
    char *val4 = NULL;
    char *val5 = NULL;
    int s = 0;

    // defaults

    Params->PciIo.Flags.ReadMode = 0;

    // start parsing

    for ( i = start_i; i < argc; i++ )
    {
        arg = argv[i];

        if ( IS_4C_ARG(arg, 'dcfg') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No bus value!\n");

            val2 = GET_ARG_VALUE(argc, argv, i, 2);
            BREAK_ON_NOT_A_VALUE(val2, s, "No device value!\n");

            val3 = GET_ARG_VALUE(argc, argv, i, 3);
            BREAK_ON_NOT_A_VALUE(val3, s, "No function value!\n");

            Params->PciIo.BDF.Bus = (UINT16)strtoul(val1, NULL, 16);
            Params->PciIo.BDF.Device = (UINT16)strtoul(val2, NULL, 16);
            Params->PciIo.BDF.Function = (UINT16)strtoul(val3, NULL, 16);
            Params->PciIo.Offset = 0;
            Params->PciIo.Size = PCI_CONFIG_SIZE;
            
            Params->Flags.Mode = MODE_DUMP_CFG;

            if ( Params->OutBufferSize < Params->PciIo.Size )
                Params->OutBufferSize = Params->PciIo.Size;

            i += 3;
        }
        else if ( IS_4C_ARG(arg, 'rcfg') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No bus value!\n");

            val2 = GET_ARG_VALUE(argc, argv, i, 2);
            BREAK_ON_NOT_A_VALUE(val2, s, "No device value!\n");

            val3 = GET_ARG_VALUE(argc, argv, i, 3);
            BREAK_ON_NOT_A_VALUE(val3, s, "No function value!\n");

            val4 = GET_ARG_VALUE(argc, argv, i, 4);
            BREAK_ON_NOT_A_VALUE(val4, s, "No offset value!\n");

            Params->PciIo.BDF.Bus = (UINT16)strtoul(val1, NULL, 16);
            Params->PciIo.BDF.Device = (UINT16)strtoul(val2, NULL, 16);
            Params->PciIo.BDF.Function = (UINT16)strtoul(val3, NULL, 16);
            Params->PciIo.Offset = (UINT16)strtoul(val4, NULL, 16);;
            Params->PciIo.Size = 4;
            Params->PciIo.Data = (PUINT8)malloc(4);
            if ( !Params->PciIo.Data )
            {
                s = ERROR_NO_SYSTEM_RESOURCES;
                break;
            }
            
            Params->Flags.Mode = MODE_READ_CFG;

            if ( Params->OutBufferSize < Params->PciIo.Size )
                Params->OutBufferSize = Params->PciIo.Size;

            i += 4;
        }
        else if ( IS_4C_ARG(arg, 'wcfg') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No bus value!\n");

            val2 = GET_ARG_VALUE(argc, argv, i, 2);
            BREAK_ON_NOT_A_VALUE(val2, s, "No device value!\n");

            val3 = GET_ARG_VALUE(argc, argv, i, 3);
            BREAK_ON_NOT_A_VALUE(val3, s, "No function value!\n");

            val4 = GET_ARG_VALUE(argc, argv, i, 4);
            BREAK_ON_NOT_A_VALUE(val4, s, "No offset value!\n");

            val5 = GET_ARG_VALUE(argc, argv, i, 5);
            BREAK_ON_NOT_A_VALUE(val5, s, "No data value!\n");

            Params->PciIo.BDF.Bus = (UINT16)strtoul(val1, NULL, 16);
            Params->PciIo.BDF.Device = (UINT16)strtoul(val2, NULL, 16);
            Params->PciIo.BDF.Function = (UINT16)strtoul(val3, NULL, 16);
            Params->PciIo.Offset = (UINT16)strtoul(val4, NULL, 16);
            Params->PciIo.Size = 4;
            Params->PciIo.Data = (PUINT8)malloc(4);
            if ( !Params->PciIo.Data )
            {
                s = ERROR_NO_SYSTEM_RESOURCES;
                break;
            }
            *(PUINT32)Params->PciIo.Data = strtoul(val5, NULL, 16);
            
            Params->Flags.Mode = MODE_WRITE_CFG;

            i += 5;
        }
        else if ( IS_4C_ARG(arg, 'dpci') )
        {
            Params->Flags.Mode = MODE_DUMP_PCI;
        }
        else if ( IS_4C_ARG(arg, 'lpci') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No type value!\n");
            UINT32 type = strtoul(val1, NULL, 0);
            type = (type-1) % LIST_TYPE_MAX;
            
            if ( type == LIST_TYPE_REG )
                Params->Flags.Mode = MODE_LIST_PCI_REG;
            else if ( type == LIST_TYPE_IO )
                Params->Flags.Mode = MODE_LIST_PCI_IO;

            i += 1;
        }
        else if ( IS_3C_ARG(arg, 'obs') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No obs value!\n");

            Params->OutBufferSize = strtoul(val1, NULL, 16);

            i++;
        }
        else if ( IS_2C_ARG(arg, 'mm') )
        {
            Params->PciIo.Flags.ReadMode = READ_CFG_MODE_MM;
        }
        else if ( IS_4C_ARG(arg, 'port') )
        {
            Params->PciIo.Flags.ReadMode = READ_CFG_MODE_PORT;
        }
        else if ( IS_4C_ARG(arg, 'bars') )
        {
            Params->PciIo.Flags.ReadBars = 1;
        }
        else if ( IS_4C_ARG(arg, 'caps') )
        {
            Params->PciIo.Flags.ReadCapabilities = 1;
        }
        else if ( IS_2C_ARG(arg, 'lp') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No log file parent path!\n");
            
            s = fillNtPath(val1, &Params->LogDir);
            if ( s != 0 )
                break;
            Params->LogDirCb = (ULONG)wcslen(Params->LogDir) << 1;

            i++;
        }
        else if ( IS_2C_ARG(arg, 'ex') )
        {
            Params->PciIo.Flags.PCIe = 1;
        }
        else if ( IS_2C_ARG(arg, 'pp') )
        {
            Params->Flags.PrintPlain = 1;
        }
        else if ( IS_3C_ARG(arg, 'wtf') )
        {
            Params->Flags.WriteToFile = 1;
            Params->PciIo.Flags.WriteToFile = 1;
        }
        else if ( IS_3C_ARG(arg, 'ptc') )
        {
            Params->Flags.Print = 1;
            Params->PciIo.Flags.Print = 1;
        }
        else if ( IS_1C_ARG(arg, 'v') )
        {
            Params->Flags.Verbose = 1;
        }
    }

    if ( s != 0 )
        return s;

    return s;
}

int checkParams(
    PCMD_PARAMS Params
)
{
    int s = 0;

    if ( !Params->Flags.Mode )
      //&& !Params->LogDir )
    {
        EPrint("No mode selected!\n");
        s = ERROR_INVALID_PARAMETER;
    }

    if ( !Params->Flags.Print
      && !Params->Flags.WriteToFile )
    {
        Params->Flags.Print = 1;
    }

    if ( Params->PciIo.Flags.ReadCapabilities
      && !Params->Flags.WriteToFile )
    {
        Params->Flags.WriteToFile = 1;
    }

    if ( !Params->LogDir && Params->Flags.WriteToFile )
    {
        s = fillNtPath(DEF_LOG_DIR_A, &Params->LogDir);
        if ( s != 0 )
            goto clean;
        Params->LogDirCb = (ULONG)wcslen(Params->LogDir) << 1;
    }

    if ( Params->LogDir )
    {
        if ( Params->LogDirCb <= 4 )
        {
            EPrint("Log dir too small!\n");
            s = ERROR_INVALID_PARAMETER;
        }
    }

#ifdef ERROR_PRINT
    if ( s != 0 )
        printf("\n");
#endif 

clean:
    return s;
}

void printParams(
    PCMD_PARAMS Params
)
{
    printf("Params:\n");
    printf("  Flags: 0x%x\n", *(PUINT32)&Params->Flags);
    printf("    Mode: 0x%x\n", Params->Flags.Mode);
    printf("    Print: 0x%x\n", Params->Flags.Print);
    printf("    PrintPlain: 0x%x\n", Params->Flags.PrintPlain);
    printf("    Verbose: 0x%x\n", Params->Flags.Verbose);
    printf("    WriteToFile: 0x%x\n", Params->Flags.WriteToFile);
    if ( Params->Flags.Mode >= MODE_PCI_IO_MIN 
      && Params->Flags.Mode <= MODE_PCI_IO_MAX )
    {
        printf("  PciIo:\n");
        printf("    Bus: 0x%x\n", Params->PciIo.BDF.Bus);
        printf("    Device: 0x%x\n", Params->PciIo.BDF.Device);
        printf("    Function: 0x%x\n", Params->PciIo.BDF.Function);
        printf("    Offset: 0x%x\n", Params->PciIo.Offset);
        printf("    Size: 0x%x\n", Params->PciIo.Size);
        if ( Params->PciIo.Data ) {
            PrintMemCol8(Params->PciIo.Data, Params->PciIo.Size, 0); }
        printf("    Flags: 0x%x\n", *(PUINT32)&Params->PciIo.Flags);
        printf("      PCIe: 0x%x\n", Params->PciIo.Flags.PCIe);
        printf("      Print: 0x%x\n", Params->PciIo.Flags.Print);
        printf("      ReadBars: 0x%x\n", Params->PciIo.Flags.ReadBars);
        printf("      ReadMode: 0x%x\n", Params->PciIo.Flags.ReadMode);
        printf("      WriteToFile: 0x%x\n", Params->PciIo.Flags.WriteToFile);
    }
    printf("  OutBufferSize: 0x%x\n", Params->OutBufferSize);
    if ( Params->LogDir )
        printf("  LogDir: %ws\n", Params->LogDir);
    printf("\n");
}

void printVersion()
{
    printf("%s %s\n", APP_NAME, APP_VS);
    printf("Last changed: %s\n", APP_LC);
    printf("Compiled: %s -- %s\n", __DATE__, __TIME__);
}

void printUsage()
{
    printf("Usage:\n"
            "%s "
            "[/dcfg <bus> <device> <function>]\n"
            "      [/rcfg <bus> <device> <function> <offset>]\n"
            "      [/wcfg <bus> <device> <function> <offset> <value>]\n"
            "      [/lpci <type>]\n"
            "      [/dpci]\n"
            "      [/bars]\n"
            "      [/caps]\n"
            "      [/ex]\n"
            "      [/mm]\n"
            "      [/port]\n"
            "      [/obs <size>]\n"
            "      [/lp <path>]\n"
            "      [/wtf]\n"
            "      [/ptc]\n"
            "      [/v]\n"
            "      [/h]\n", APP_NAME);
}

void printHelp()
{
    printVersion();
    printf("\n");
    printUsage();
    printf("\n");
    printf("PCI IO:\n");
    printf("/lpci: List all registered pci devices.\n");
    printf("       Type 1: Using Registry. (No driver needed for this.)\n");
    printf("       Type 2: Using port IO device iteration.\n");
    printf("/dpci: Dump the configs of all registered pci devices (using registry listing).\n");
    printf("/dcfg: Dump a single pci config from <bus> <device> <function>.\n");
    printf("/rcfg: Read a uint32 of a single pci config at <bus> <device> <function> <offset>.\n");
    printf("/wcfg: Write a uint32 <value> to a single pci cfg at <bus> <device> <function> <offset>.\n");
    printf("\n");
    printf("PCI IO Flags:\n");
    printf("/mm: Read memory mapped BDF configs. Faster and PCIe (size of 0x1000) compatible. (Default)\n");
    printf("/port: Read BDF configs via port io. Slow legacy mode and just PCI (size of 0x100) compatible.\n");
    printf("/bars: Read out the bars of a device and write them into files.\n");
    printf("/caps: Read out the capabilities of a device and write them into the log.\n");
    printf("/ex: Set the read size for the configs to 0x1000 bytes (PCIe). Default: 0x100 (PCI).\n");
    printf("\n");
    printf("Options:\n");
    printf("/obs: Output buffer size.\n");
    printf("/lp: Parent path for writing config file, caps and bar dumps. Default: %s.\n", DEF_LOG_DIR_A);
    printf("\n");
    printf("IO Flags:\n");
    printf("/ptc: Print to console.\n");
    printf("/wtf: Write to file.\n");
    printf("/pp: Print plain bytes output buffer.\n");
    printf("/v: Verbose output.\n");
    printf("/h: Print this.\n");
    printf("\n");
    printf("Remarks:\n");
    printf("All values given for bdfs, values and stuff are interpreted as hex values, except if noted otherwise.\n");
    printf("\n");
}
