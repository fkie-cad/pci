#pragma warning( push )
#include "../warnings_ntifs_wdm.h"
#include <ntifs.h>
#include <ntstrsafe.h>
#pragma warning( pop )

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../warnings.h"

#include "inc/acpi.h"
#include "inc/nt.h"
#include "inc/win.h"
#include "../inc/spi-ram.h"
#include "../SpiShared.h"
#include "../print.h"
#include "helper/deviceIo.h"
#include "helper/Converter.h"
#include "helper/Args.h"
#include "../common/kfileio.h"
#include "helper/filesW.h"
#include "helper/fileio.h"
#include "helper/virtmem.h"

#define CACHE_TYPE (MmNonCached)

// maximum bar size to include into cfg log
#define MAX_BAR_SIZE_TO_LOG 0x1000


#define POOL_TAG ('sips')
#ifdef RING3
//warning C4005: 'ExFreePool': macro redefinition
#pragma warning ( disable : 4005 )
#define ExAllocatePoolWithTag(_pt_, _n_, _t_) malloc(_n_)
// #define ExFreePoolWithTag(_p_, _t_) free(_p_)
#define ExFreePool(_p_) free(_p_)
#pragma warning ( default : 4005 )
#endif

#include "../inc/pci/pci.h"
#include "../common/strings.h"
#include "../pci/pciids.h"
#include "memoryIo.h"

#include "bars.h"

#include "../common/log.h"
#include "../inc/pci/pciDeviceCfg-pch300series.h"
#include "log/logCfg.h"
#include "log/logBar.h"
#include "log/logSpi.h"


#define APP_NAME "SpiUi"
#define BIN_NAME APP_NAME ".exe"
#define APP_VS "1.1.2"
#define APP_LC "15.07.2026"

#define DEF_LOG_DIR_A "C:\\logs"
//#define DEF_LOG_FILE_TYPE L".log"

#define PCI_CONFIG_SIZE (0x100)
#define PCIE_CONFIG_SIZE (0x1000)

#define NO_OFFSET ((ULONG)-1)

#define MODE_DUMP_SPI   (0x1)
#define MODE_COMMAND_SPI    (0x2)
// currently unused
#define MODE_JEDEC_ID       (0x4)

#define MODE_MAX            (0xf)

#define SPI_LOC_AUTO   (0x0)
#define SPI_LOC_BDF    (0x1)
#define SPI_LOC_RCBA   (0x2)

#define MODE_DUMP (MODE_DUMP_SPI|MODE_DUMP_SPI_DRV)


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

typedef struct _SPI_IO {
    UINT32 Base;
    UINT32 Limit;
} SPI_IO, *PSPI_IO;

typedef struct _CMD_PARAMS {
    ULONG OutBufferSize;
    PCI_IO PciIo;
    SPI_IO SpiIo;
    struct {
        UINT32 LinearAddress;
        UINT32 DataSize;
        UINT8 CycleCommand;
        PUINT8 Data;
    } CommandSpi;
    struct {
        UINT32 Verbose:1;
        UINT32 PrintPlain:1;
        UINT32 Print:1;
        UINT32 WriteToFile:1;
        UINT32 Mode:4;
        UINT32 DumpBiosMode:2;
        UINT32 Regions:12;
        UINT32 SpiLocation:2;
        UINT32 Reserved:8;
    } Flags;
    ULONG LogDirCb;
    PWCHAR LogDir;
} CMD_PARAMS, *PCMD_PARAMS;

//#pragma pack()

#include "cfg.h"
#include "spi.h"



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


    s = openDevice(&device, DEVICE_BASE_NAME_W);
    if ( s != 0 )
        goto clean;

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
        // rw / memoryIo.h
        case MODE_DUMP_SPI:
        {
            s = dumpSpi(device, &params);
            break;
        }
        case MODE_COMMAND_SPI:
        {
            s = commandSpi(device, &params);
            break;
        }
        default:
            printf("No valid mode set!\n");
            goto clean;
    }
    
clean:
    if ( device )
        NtClose(device);
    if ( params.PciIo.Data )
        free(params.PciIo.Data);
    if ( params.LogDir )
        free(params.LogDir);
    if ( params.CommandSpi.Data )
        free(params.CommandSpi.Data);

    return s;
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
    int s = 0;


    // defaults

    Params->Flags.Regions = DEFAULT_REGION_MASK;

    Params->PciIo.Flags.ReadMode = 0;

    Params->SpiIo.Base = FD_INVALID_REGION_BASE;
    Params->SpiIo.Limit = FD_INVALID_REGION_LIMIT;

    // start parsing

    for ( i = start_i; i < argc; i++ )
    {
        arg = argv[i];

        if ( IS_4C_ARG(arg, 'dspi') )
        {
            Params->Flags.Mode = MODE_DUMP_SPI;
            Params->Flags.WriteToFile = 1;
            Params->PciIo.Flags.WriteToFile = 1;
        }
        else if ( IS_4C_ARG(arg, 'rcbo') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No offset given!\n");

            Params->PciIo.Rcbo = strtoul(val1, NULL, 16);
            Params->Flags.SpiLocation = SPI_LOC_RCBA;

            i += 1;
        }
        else if ( IS_4C_ARG(arg, 'bdfb') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No bus!\n");

            val2 = GET_ARG_VALUE(argc, argv, i, 2);
            BREAK_ON_NOT_A_VALUE(val2, s, "No device!\n");

            val3 = GET_ARG_VALUE(argc, argv, i, 3);
            BREAK_ON_NOT_A_VALUE(val3, s, "No function!\n");

            val4 = GET_ARG_VALUE(argc, argv, i, 4);
            BREAK_ON_NOT_A_VALUE(val4, s, "No bar id!\n");
            
            Params->PciIo.BDF.Bus = (UINT16)strtoul(val1, NULL, 16);
            Params->PciIo.BDF.Device = (UINT16)strtoul(val2, NULL, 16);
            Params->PciIo.BDF.Function = (UINT16)strtoul(val3, NULL, 16);
            Params->PciIo.BarId = (UINT8)strtoul(val4, NULL, 16);

            Params->Flags.SpiLocation = SPI_LOC_BDF;

            i += 4;
        }
        else if ( IS_4C_ARG(arg, 'cspi') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No command!\n");

            val2 = GET_ARG_VALUE(argc, argv, i, 2);
            BREAK_ON_NOT_A_VALUE(val2, s, "No linear address!\n");
            
            Params->CommandSpi.CycleCommand = (UINT8)strtoul(val1, NULL, 16);
            Params->CommandSpi.LinearAddress = strtoul(val2, NULL, 16);

            // just expect a third value for this commands
            if ( Params->CommandSpi.CycleCommand == FCYCLE_READ 
              || Params->CommandSpi.CycleCommand == FCYCLE_WRITE 
              || Params->CommandSpi.CycleCommand == FCYCLE_READ_SFDP
              || Params->CommandSpi.CycleCommand == FCYCLE_JEDEC_ID
              || Params->CommandSpi.CycleCommand == FCYCLE_WRITE_STATUS )
            {
                val3 = GET_ARG_VALUE(argc, argv, i, 3);
                BREAK_ON_NOT_A_VALUE(val3, s, "No size|bytes!\n");

                i++;
            }
            i+=2;

            if ( Params->CommandSpi.CycleCommand == FCYCLE_WRITE 
              || Params->CommandSpi.CycleCommand == FCYCLE_WRITE_STATUS  )
            {
                UINT32 dataSize = 0;
                Params->CommandSpi.Data = NULL;
                Params->CommandSpi.DataSize = 0;

                // warning C6011: Dereferencing NULL pointer 'val3'.
                // warning C6387: 'val3' could be '0':
DISABLE_WARNING( 6011 6387 )
                if ( val3[0] == 'f' && val3[1] == ':' )
                {
                    val3 += 2;

                    s = parseFile(val3, &Params->CommandSpi.Data, (PULONG)&dataSize);
                }
                else
                {
                    s = parsePlainBytes(val3, &Params->CommandSpi.Data, (PULONG)&dataSize, UINT32_MAX);
                }
                Params->CommandSpi.DataSize = dataSize;
            }
            else if ( Params->CommandSpi.CycleCommand == FCYCLE_READ 
                   || Params->CommandSpi.CycleCommand == FCYCLE_READ_SFDP
                   || Params->CommandSpi.CycleCommand == FCYCLE_JEDEC_ID )
            {
                Params->CommandSpi.DataSize = strtoul(val3, NULL, 16);
            }
            else 
            {
                Params->CommandSpi.DataSize = 0x40;
            }
DEFAULT_WARNING( 6011 6387 )

            Params->Flags.Mode = MODE_COMMAND_SPI;
        }
        else if ( IS_3C_ARG(arg, 'obs') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No obs value!\n");

            Params->OutBufferSize = strtoul(val1, NULL, 16);

            i++;
        }
        else if ( IS_2C_ARG(arg, 'pp') )
        {
            Params->Flags.PrintPlain = 1;
        }
        else if ( IS_4C_ARG(arg, 'mmio') )
        {
            Params->Flags.DumpBiosMode = DUMP_BIOS_MODE_MMIO;
        }
        else if ( IS_3C_ARG(arg, 'reg') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No reg value!\n");

            UINT32 regions = strtoul(val1, NULL, 16);
            // sanitize
            //regions &= DEFAULT_REGION_MASK;
            if ( regions > MAX_REGION_MASK )
            {
                EPrint("Not a valid region mask!\n");
                s = ERROR_INVALID_PARAMETER;
                break;
            }
            Params->Flags.Regions = regions;
            i++;
        }
        //else if ( IS_2C_ARG(arg, 'mm') )
        //{
        //    Params->PciIo.Flags.ReadMode = READ_CFG_MODE_MM;
        //}
        //else if ( IS_4C_ARG(arg, 'port') )
        //{
        //    Params->PciIo.Flags.ReadMode = READ_CFG_MODE_PORT;
        //}
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
        else if ( IS_2C_ARG(arg, 'ft') )
        {
            val1 = GET_ARG_VALUE(argc, argv, i, 1);
            BREAK_ON_NOT_A_VALUE(val1, s, "No from value!\n");

            val2 = GET_ARG_VALUE(argc, argv, i, 2);
            BREAK_ON_NOT_A_VALUE(val2, s, "No to value!\n");

            UINT32 from = strtoul(val1, NULL, 16);
            UINT32 to = strtoul(val2, NULL, 16);

            Params->SpiIo.Base = from;
            Params->SpiIo.Limit = to;

            i += 2;
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

    if ( !Params->Flags.Mode
      && !Params->LogDir )
    {
        EPrint("No mode selected!\n");
        s = ERROR_INVALID_PARAMETER;
    }

    if ( Params->Flags.Mode == MODE_DUMP_SPI )
    {
        if ( Params->Flags.DumpBiosMode == DUMP_BIOS_MODE_NONE )
        {
            Params->Flags.DumpBiosMode = DUMP_BIOS_MODE_SPI;
        }
        if ( Params->PciIo.BarId > 5 )
        {
            EPrint("Invalid bar id: 0x%x\n", Params->PciIo.BarId);
            s = ERROR_INVALID_PARAMETER;
        }
        //if ( Params->Flags.Regions == (UINT32)-1 )
        //{
        //    Params->Flags.Regions = DEFAULT_REGION_MASK;
        //}
    }

    if ( Params->Flags.Mode == MODE_DUMP_SPI )
    {
        //if ( Params->Flags.DumpBiosMode == DUMP_BIOS_MODE_PTE )
        //{
        //    EPrint("Pte mode for bios dumping not supported with exe based approach!\n");
        //    s = ERROR_INVALID_PARAMETER;
        //}
    }

    if ( Params->Flags.Mode == MODE_COMMAND_SPI )
    {
        // check CycleCommand
        // max is 4 bits
        if ( Params->CommandSpi.CycleCommand > 0b1111 )
        {
            EPrint("Invalid cycle command: 0x%x\n", Params->CommandSpi.CycleCommand);
            s = ERROR_INVALID_PARAMETER;
        }
        else if ( Params->CommandSpi.CycleCommand > FCYCLE_RPMC_OP2 )
        {
            DPrint("Unknown cycle command: 0x%x\n", Params->CommandSpi.CycleCommand);
            //s = ERROR_INVALID_PARAMETER;
        }
        
        if ( Params->PciIo.BarId > 5 )
        {
            EPrint("Invalid bar id: 0x%x\n", Params->PciIo.BarId);
            s = ERROR_INVALID_PARAMETER;
        }

        // check DataByteCount
        if ( Params->CommandSpi.DataSize )
        {
            // check 4 byte alignment
            //if ( (Params->CommandSpi.DataSize % 4) != 0 )
            //{
            //    EPrint("Invalid Data Alignment!\n");
            //    s = ERROR_INVALID_PARAMETER;
            //}

            //if ( Params->CommandSpi.DataSize > 0b111111 )
            //{
            //    EPrint("Invalid DataByteCount: 0x%x\n", Params->CommandSpi.DataByteCount);
            //    s = ERROR_INVALID_PARAMETER;
            //}
        }
    }

    if ( !Params->Flags.Print
      && !Params->Flags.WriteToFile )
    {
        Params->Flags.Print = 1;
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
    printf("    DumpBiosMode: 0x%x\n", Params->Flags.DumpBiosMode);
    printf("    Mode: 0x%x\n", Params->Flags.Mode);
    printf("    Print: 0x%x\n", Params->Flags.Print);
    printf("    PrintPlain: 0x%x\n", Params->Flags.PrintPlain);
    printf("    Regions: 0x%x\n", Params->Flags.Regions);
    printf("    Verbose: 0x%x\n", Params->Flags.Verbose);
    printf("    WriteToFile: 0x%x\n", Params->Flags.WriteToFile);
    if ( Params->Flags.Mode == MODE_COMMAND_SPI )
    {
        printf("  CommandSpi\n");
        printf("    CycleCommand: 0x%x\n", Params->CommandSpi.CycleCommand);
        printf("    LinearAddress: 0x%x\n", Params->CommandSpi.LinearAddress);
        printf("    DataSize: 0x%x\n", Params->CommandSpi.DataSize);
        if ( Params->CommandSpi.CycleCommand == FCYCLE_WRITE )
        {
            printf("    Data:\n");
            PrintMemCol8(Params->CommandSpi.Data, Params->CommandSpi.DataSize, 0);
        }
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
            "[/dspi]\n"
            "      [/bdfb <bus> <device> <function> <bar>]\n"
            "      [/rcbo <offset>]\n"
            "      [/cspi <command> <address> [<size|bytes>]]\n"
            "      [/mmio]\n"
            //"      [/mm]\n"
            //"      [/port]\n"
            "      [/reg <mask>] | [/ft <from> <to>]\n"
            "      [/obs <size>]\n"
            "      [/lp <path>]\n"
            "      [/wtf]\n"
            "      [/ptc]\n"
            "      [/v]\n"
            "      [/h]\n", APP_NAME);
}

void printHelp()
{
    //PWCHAR defLogDir = DEF_LOG_DIR_NT;
    //defLogDir += 4;

    printVersion();
    printf("\n");
    printUsage();
    printf("\n");
    printf("/dspi : Try to dump spi flash memory via SPIBar reading cycle.\n");
    printf("   Automatically finds SPIBar if no other option is set.\n");
    printf("   Dumps all regions into separate blobs by default, if /ft is not used.\n");
    printf("/cspi : Issue an SPI cycle <command> at <address> with <size> or <bytes>.\n");
    printf("  FCYCLE_READ (0x0) : Read <size> bytes.\n");
    printf("  FCYCLE_WRITE (0x2) : Write <bytes> from a given byte string (AABBCCDD) or a file (f:filename).\n");
    printf("  FCYCLE_BLOCK_ERASE (0x3) : 4k Block Erase at <address>.\n");
    printf("  FCYCLE_SEC_ERASE (0x4) : 64k Sector erase at <address>.\n");
    printf("  FCYCLE_READ_SFDP (0x5) : Read SFDP.\n");
    printf("  FCYCLE_JEDEC_ID (0x6) : Read JEDEC ID.\n");
    printf("  FCYCLE_WRITE_STATUS (0x7) : write status.\n");
    printf("  FCYCLE_READ_STATUS (0x8) : read status.\n");
    printf("  FCYCLE_RPMC_OP1 (0x9) : RPMC Op1.\n");
    printf("  FCYCLE_RPMC_OP2 (0xA) : RPMC Op2.\n");
    printf("/bdfb : Manually set the <bus> <device> <function> <bar> where the address of the spi interface ought to be found.\n");
    printf("/rcbo : Manually set the offset to RCBA to get the spi interface. Commonly it's 0x3800. RCBA itself is retrieved automatically from device 00:1f:00 at offset 0xf0.\n");
    printf("/mmio: Dump bios with mmio physical read, instead of spi dump.\n");
    printf("/reg: Region mask for desired dumps:\n"
           "1=fd, 2=bios, 4=me, 8=gb, 0x10=pd, 0x20=reg[5], ..., 0x200=reg[9].\n"
           "Default=0x%x\n", 
            DEFAULT_REGION_MASK);
    printf("/ft: Dump spi memory <from> address <to> address. "
           "Takes precedence over the '/reg' parameter. "
           "Use this to dump the whole bios into one blob.\n"
           "Resulting size should be multiple of 0x40 bytes..\n");
    printf("\n");
    printf("Options:\n");
    printf("/obs : Output buffer size.\n");
    printf("/lp : Path to the parent directory of the created log files. Default: %s.\n", DEF_LOG_DIR_A);
    printf("\n");
    printf("Flags:\n");
    printf("/ptc : Print to console.\n");
    printf("/wtf : Write to file.\n");
    printf("/pp : Print plain bytes output buffer.\n");
    printf("/v : Verbose output.\n");
    printf("/h : Print this.\n");
    printf("\n");
    printf("Remarks:\n");
    printf("All values given for bdfs, ports, values and stuff are interpreted as hex values, except if noted otherwise.\n");
    printf("\n");
}
