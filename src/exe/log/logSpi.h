#pragma once

#include "../../inc/jedec.h"

NTSTATUS logSpiRegister(
    _In_ PVOID SpiRegister,
    _In_ ULONG SpiRegisterSize,
    _In_ PLOG Log
)
{
    NTSTATUS status = 0;

    ULONG i;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;

    CHAR bin8[BIN_8_STR_BUFFER_SIZE] = { 0 };

    PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP map = (PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP)SpiRegister;

    (SpiRegisterSize);

    FEnter();
    
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                          "Intel 300 series SPI register address map\r\n"
                          "BFPREG [%zx]: 0x%x\r\n"
                          "  RB: 0x%x\r\n"
                          "  Reserved1: 0x%x\r\n"
                          "  RL: 0x%x\r\n"
                          "  Reserved2: 0x%x\r\n"
                          "HSFSTS_CTL [%zx]: 0x%x\r\n"
                          "  FDONE: 0x%x\r\n"
                          "  FCERR: 0x%x\r\n"
                          "  H_AEL: 0x%x\r\n"
                          "  Reserved1: 0x%x\r\n"
                          "  H_SCIP: 0x%x\r\n"
                          "  Reserved2: 0x%x\r\n"
                          "  WRSDIS: 0x%x\r\n"
                          "  PRR34_LOCKDN: 0x%x\r\n"
                          "  FDOPSS: 0x%x\r\n"
                          "  FDV: 0x%x\r\n"
                          "  FLOCKDN: 0x%x\r\n"
                          "  FGO: 0x%x\r\n"
                          "  FCYCLE: 0x%x\r\n"
                          "  WET: 0x%x\r\n"
                          "  Reserved3: 0x%x\r\n"
                          "  FDBC: 0x%x\r\n"
                          "  Reserved4: 0x%x\r\n"
                          "  FSMIE: 0x%x\r\n"
                          "FADDR [%zx]: 0x%x\r\n"
                          "  FLA: 0x%x\r\n"
                          "  Reserved: 0x%x\r\n"
                          "DLOCK [%zx]: 0x%x\r\n"
                          "  BMWAGLOCKDN: 0x%x\r\n"
                          "  BMRAGLOCKDN: 0x%x\r\n"
                          "  SBMWAGLOCKDN: 0x%x\r\n"
                          "  SBMRAGLOCKDN: 0x%x\r\n"
                          "  SPARE7: 0x%x\r\n"
                          "  SPARE6: 0x%x\r\n"
                          "  SPARE5: 0x%x\r\n"
                          "  SPARE4: 0x%x\r\n"
                          "  PR0LOCKDN: 0x%x\r\n"
                          "  PR1LOCKDN: 0x%x\r\n"
                          "  PR2LOCKDN: 0x%x\r\n"
                          "  PR3LOCKDN: 0x%x\r\n"
                          "  PR4LOCKDN: 0x%x\r\n"
                          "  SPARE3: 0x%x\r\n"
                          "  SPARE2: 0x%x\r\n"
                          "  SPARE1: 0x%x\r\n"
                          "  SSEQLOCKDN: 0x%x\r\n"
                          "  Reserved: 0x%x\r\n",
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, BFPREG), *(PUINT32)&map->BFPREG,
                                map->BFPREG.RB, 
                                map->BFPREG.Reserved1,
                                map->BFPREG.RL,
                                map->BFPREG.Reserved2,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, HSFSTS_CTL), *(PUINT32)&map->HSFSTS_CTL,
                                map->HSFSTS_CTL.FDONE,
                                map->HSFSTS_CTL.FCERR,
                                map->HSFSTS_CTL.H_AEL,
                                map->HSFSTS_CTL.Reserved1,
                                map->HSFSTS_CTL.H_SCIP,
                                map->HSFSTS_CTL.Reserved2,
                                map->HSFSTS_CTL.WRSDIS,
                                map->HSFSTS_CTL.PRR34_LOCKDN,
                                map->HSFSTS_CTL.FDOPSS,
                                map->HSFSTS_CTL.FDV,
                                map->HSFSTS_CTL.FLOCKDN,
                                map->HSFSTS_CTL.FGO,
                                map->HSFSTS_CTL.FCYCLE,
                                map->HSFSTS_CTL.WET,
                                map->HSFSTS_CTL.Reserved3,
                                map->HSFSTS_CTL.FDBC,
                                map->HSFSTS_CTL.Reserved4,
                                map->HSFSTS_CTL.FSMIE,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FADDR), *(PUINT32)&map->FADDR,
                                map->FADDR.FLA,
                                map->FADDR.Reserved,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, DLOCK), *(PUINT32)&map->DLOCK,
                                map->DLOCK.BMWAGLOCKDN,
                                map->DLOCK.BMRAGLOCKDN,
                                map->DLOCK.SBMWAGLOCKDN,
                                map->DLOCK.SBMRAGLOCKDN,
                                map->DLOCK.SPARE7,
                                map->DLOCK.SPARE6,
                                map->DLOCK.SPARE5,
                                map->DLOCK.SPARE4,
                                map->DLOCK.PR0LOCKDN,
                                map->DLOCK.PR1LOCKDN,
                                map->DLOCK.PR2LOCKDN,
                                map->DLOCK.PR3LOCKDN,
                                map->DLOCK.PR4LOCKDN,
                                map->DLOCK.SPARE3,
                                map->DLOCK.SPARE2,
                                map->DLOCK.SPARE1,
                                map->DLOCK.SSEQLOCKDN,
                                map->DLOCK.Reserved);
    for ( i = 0; i < FDATA_COUNT; i++ )
    {
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                              "FDATA[0x%x] [%zx]: 0x%08x\r\n",
                                i, offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FDATA[i]), map->FDATA[i]);
    }
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "FRACC [%zx]: 0x%x\r\n",
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FRACC), *(PUINT32)&map->FRACC);
    uint8ToBin((UINT8)map->FRACC.BRRA, bin8);
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "  BRRA: 0x%x (0b%s)\r\n",
                              map->FRACC.BRRA, bin8);
    uint8ToBin((UINT8)map->FRACC.BRWA, bin8);
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "  BRWA: 0x%x (0b%s)\r\n",
                              map->FRACC.BRWA, bin8);
    uint8ToBin((UINT8)map->FRACC.BMRAG, bin8);
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "  BMRAG: 0x%x (0b%s)\r\n",
                              map->FRACC.BMRAG, bin8);
    uint8ToBin((UINT8)map->FRACC.BMWAG, bin8);
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "  BMWAG: 0x%x (0b%s)\r\n",
                              map->FRACC.BMWAG, bin8);
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "FREG[0] [%zx]: 0x%08x (Base: 0x%04x, Limit: 0x%04x) [%s]\r\n"
                            "FREG[1] [%zx]: 0x%08x (Base: 0x%04x, Limit: 0x%04x) [%s]\r\n"
                            "FREG[2] [%zx]: 0x%08x (Base: 0x%04x, Limit: 0x%04x) [%s]\r\n"
                            "FREG[3] [%zx]: 0x%08x (Base: 0x%04x, Limit: 0x%04x) [%s]\r\n"
                            "FREG[4] [%zx]: 0x%08x (Base: 0x%04x, Limit: 0x%04x) [%s]\r\n"
                            "FREG[5] [%zx]: 0x%08x (Base: 0x%04x, Limit: 0x%04x) [%s]\r\n"
                            "Reserved1 [%zx]: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x\r\n"
                            "                %02x %02x %02x %02x %02x %02x %02x %02x-%02x\r\n"
                            "FPR0 [%zx]: (Base: 0x%04x, RPE: %x, Limit: 0x%04x, WPE: %x)\r\n"
                            "FPR1 [%zx]: (Base: 0x%04x, RPE: %x, Limit: 0x%04x, WPE: %x)\r\n"
                            "FPR2 [%zx]: (Base: 0x%04x, RPE: %x, Limit: 0x%04x, WPE: %x)\r\n"
                            "FPR3 [%zx]: (Base: 0x%04x, RPE: %x, Limit: 0x%04x, WPE: %x)\r\n"
                            "FPR4 [%zx]: (Base: 0x%04x, RPE: %x, Limit: 0x%04x, WPE: %x)\r\n"
                            "GPR0 [%zx]: (Base: 0x%04x, RPE: %x, Limit: 0x%04x, WPE: %x)\r\n"
                            "Reserved2 [%zx]: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x\r\n"
                            "                %02x %02x %02x %02x %02x\r\n"
                            "SFRACC [%zx]: 0x%x\r\n"
                            "   Reserved: 0x%x\r\n"
                            "   SECONDARYBIOS_MRAG: 0x%x\r\n"
                            "   SECONDARYBIOS_MWAG: 0x%x\r\n"
                            "FDOC [%zx]: 0x%x\r\n"
                            "  Reserveed1: 0x%x\r\n"
                            "  FDSI: 0x%x\r\n"
                            "  FDSS: 0x%x (%s)\r\n"
                            "  Reserved2: 0x%x\r\n"
                            "FDOD [%zx]: 0x%x\r\n"
                            "Reserved3 [%zx]: 0x%x\r\n"
                            "AFC [%zx]: 0x%x\r\n"
                            "  SPFP: 0x%x\r\n"
                            "  Reserved1: 0x%x\r\n"
                            "SFDP0_VSCC0 [%zx]: 0x%x\r\n"
                            "  Reserved1: 0x%x\r\n"
                            "  WG: 0x%x\r\n"
                            "  WSR: 0x%x\r\n"
                            "  WEWS: 0x%x\r\n"
                            "  QER: 0x%x\r\n"
                            "  EO_4k: 0x%x\r\n"
                            "  EO_64k: 0x%x\r\n"
                            "  SOFT_RST_SUPPORTED: 0x%x\r\n"
                            "  SUSPEND_RESUME_SUPPORTED: 0x%x\r\n"
                            "  DEEP_PWRDN_SUPPORTED: 0x%x\r\n"
                            "  RPMC_SUPPORTED: 0x%x\r\n"
                            "  EO_4k_VALID: 0x%x\r\n"
                            "  EO_64k_VALID: 0x%x\r\n"
                            "  VCL: 0x%x\r\n"
                            "  CPPTV: 0x%x\r\n"
                            "SFDP1_VSCC1 [%zx]: 0x%x\r\n"
                            "  Reserved1: 0x%x\r\n"
                            "  WG: 0x%x\r\n"
                            "  WSR: 0x%x\r\n"
                            "  WEWS: 0x%x\r\n"
                            "  QER: 0x%x\r\n"
                            "  EO_4k: 0x%x\r\n"
                            "  EO_64k: 0x%x\r\n"
                            "  SOFT_RST_SUPPORTED: 0x%x\r\n"
                            "  SUSPEND_RESUME_SUPPORTED: 0x%x\r\n"
                            "  DEEP_PWRDN_SUPPORTED: 0x%x\r\n"
                            "  RPMC_SUPPORTED: 0x%x\r\n"
                            "  EO_4k_VALID: 0x%x\r\n"
                            "  EO_64k_VALID: 0x%x\r\n"
                            "  VCL: 0x%x\r\n"
                            "  CPPTV: 0x%x\r\n"
                            "PTINX [%zx]: 0x%x\r\n"
                            "PTDATA [%zx]: 0x%x\r\n"
                            "SBRS [%zx]: 0x%x\r\n",
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FREG[0]), *(PUINT32)&map->FREG[0], map->FREG[0].RB,map->FREG[0].RL,FD_REGION_NAME(0),
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FREG[1]), *(PUINT32)&map->FREG[1], map->FREG[1].RB,map->FREG[1].RL,FD_REGION_NAME(1),
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FREG[2]), *(PUINT32)&map->FREG[2], map->FREG[2].RB,map->FREG[2].RL,FD_REGION_NAME(2),
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FREG[3]), *(PUINT32)&map->FREG[3], map->FREG[3].RB,map->FREG[3].RL,FD_REGION_NAME(3),
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FREG[4]), *(PUINT32)&map->FREG[4], map->FREG[4].RB,map->FREG[4].RL,FD_REGION_NAME(4),
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FREG[5]), *(PUINT32)&map->FREG[5], map->FREG[5].RB,map->FREG[5].RL,FD_REGION_NAME(5),
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, Reserved1), map->Reserved1[0],map->Reserved1[1],map->Reserved1[2],map->Reserved1[3],map->Reserved1[4],map->Reserved1[5],map->Reserved1[6],map->Reserved1[7],map->Reserved1[8],map->Reserved1[0x9],map->Reserved1[0xa],map->Reserved1[0xb],map->Reserved1[0xc],map->Reserved1[0xd],map->Reserved1[0xe],map->Reserved1[0xf],map->Reserved1[0x10],map->Reserved1[0x11],map->Reserved1[0x12],map->Reserved1[0x13],map->Reserved1[0x14],map->Reserved1[0x15],map->Reserved1[0x16],map->Reserved1[0x17],
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FPR0), map->FPR0.PRB,map->FPR0.RPE,map->FPR0.PRL,map->FPR0.WPE,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FPR1), map->FPR1.PRB,map->FPR1.RPE,map->FPR1.PRL,map->FPR1.WPE,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FPR2), map->FPR2.PRB,map->FPR2.RPE,map->FPR2.PRL,map->FPR2.WPE,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FPR3), map->FPR3.PRB,map->FPR3.RPE,map->FPR3.PRL,map->FPR3.WPE,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FPR4), map->FPR4.PRB,map->FPR4.RPE,map->FPR4.PRL,map->FPR4.WPE,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, GPR0), map->GPR0.PRB,map->GPR0.RPE,map->GPR0.PRL,map->GPR0.WPE,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, Reserved2), map->Reserved2[0],map->Reserved2[1],map->Reserved2[2],map->Reserved2[3],map->Reserved2[4],map->Reserved2[5],map->Reserved2[6],map->Reserved2[7],map->Reserved2[8],map->Reserved2[0x9],map->Reserved2[0xa],map->Reserved2[0xb],map->Reserved2[0xc],map->Reserved2[0xd],map->Reserved2[0xe],map->Reserved2[0xf],map->Reserved2[0x10],map->Reserved2[0x11],map->Reserved2[0x12],map->Reserved2[0x13],
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, SFRACC), *(PUINT32)&map->SFRACC,
                              map->SFRACC.Reserved,
                              map->SFRACC.SECONDARYBIOS_MRAG,
                              map->SFRACC.SECONDARYBIOS_MWAG,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FDOC), *(PUINT32)&map->FDOC,
                                map->FDOC.Reserved1,
                                map->FDOC.FDSI,
                                map->FDOC.FDSS, FDSS_STR(map->FDOC.FDSS),
                                map->FDOC.Reserved2,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FDOD), *(PUINT32)&map->FDOD,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, Reserved3), map->Reserved3,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, AFC), *(PUINT32)&map->AFC,
                                map->AFC.SPFP,
                                map->AFC.Reserved1,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, SFDP0_VSCC0), *(PUINT32)&map->SFDP0_VSCC0,
                                map->SFDP0_VSCC0.Reserved1,
                                map->SFDP0_VSCC0.WG,
                                map->SFDP0_VSCC0.WSR,
                                map->SFDP0_VSCC0.WEWS,
                                map->SFDP0_VSCC0.QER,
                                map->SFDP0_VSCC0.EO_4k,
                                map->SFDP0_VSCC0.EO_64k,
                                map->SFDP0_VSCC0.SOFT_RST_SUPPORTED,
                                map->SFDP0_VSCC0.SUSPEND_RESUME_SUPPORTED,
                                map->SFDP0_VSCC0.DEEP_PWRDN_SUPPORTED,
                                map->SFDP0_VSCC0.RPMC_SUPPORTED,
                                map->SFDP0_VSCC0.EO_4k_VALID,
                                map->SFDP0_VSCC0.EO_64k_VALID,
                                map->SFDP0_VSCC0.VCL,
                                map->SFDP0_VSCC0.CPPTV,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, SFDP1_VSCC1), *(PUINT32)&map->SFDP1_VSCC1,
                                map->SFDP1_VSCC1.Reserved1,
                                map->SFDP1_VSCC1.WG,
                                map->SFDP1_VSCC1.WSR,
                                map->SFDP1_VSCC1.WEWS,
                                map->SFDP1_VSCC1.QER,
                                map->SFDP1_VSCC1.EO_4k,
                                map->SFDP1_VSCC1.EO_64k,
                                map->SFDP1_VSCC1.SOFT_RST_SUPPORTED,
                                map->SFDP1_VSCC1.SUSPEND_RESUME_SUPPORTED,
                                map->SFDP1_VSCC1.DEEP_PWRDN_SUPPORTED,
                                map->SFDP1_VSCC1.RPMC_SUPPORTED,
                                map->SFDP1_VSCC1.EO_4k_VALID,
                                map->SFDP1_VSCC1.EO_64k_VALID,
                                map->SFDP1_VSCC1.VCL,
                                map->SFDP1_VSCC1.CPPTV,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, PTINX), map->PTINX,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, PTDATA), map->PTDATA,
                            offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, SBRS), *(PUINT32)&map->SBRS);

//clean:
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    return status;
}

NTSTATUS logFlashDescriptor(
    _In_ PVOID Buffer,
    _In_ UINT32 BufferSize,
    _In_ PLOG Log
)
{
    NTSTATUS status = 0;

    PUINT8 fdBase = (PUINT8)Buffer;
    PUINT32 buffer32 = NULL;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;

    ULONG i;
    
    UINT32 componentOffset;
    UINT32 nrComponents;
    PFLASH_DESCRIPTOR_COMPONENT_SECTION fdcs = NULL;

    UINT32 registerOffset;
    UINT32 nrRegisters;
    PFLASH_DESCRIPTOR_REGION_SECTION fdrs = NULL;

    UINT32 masterOffset;
    UINT32 nrMasters;
    PFLASH_DESCRIPTOR_MASTER_SECTION fdms = NULL;

    UINT32 ichStrapOffset;
    UINT32 nrIchStraps;

    UINT32 gmchStrapOffset;
    UINT32 nrGmchStraps;

    UINT32 flumap1Offset;
    UINT32 flumap1Size;

    UINT32 oemOffset;
    UINT32 oemSize;

    CHAR bin32_0[BIN_32_STR_BUFFER_SIZE];
    CHAR bin32_1[BIN_32_STR_BUFFER_SIZE];
    CHAR bin32_2[BIN_32_STR_BUFFER_SIZE];

    FEnter();

    PFLASH_DESCRIPTOR fd = (PFLASH_DESCRIPTOR)(fdBase + ICH_FLASH_DESCRIPTOR_SIG_OFFSET);
    if ( fd->Signature != FLASH_DESRIPTOR_MODE_SIG )
    {
        fd = (PFLASH_DESCRIPTOR)(fdBase + PCH_FLASH_DESCRIPTOR_SIG_OFFSET);
        if ( fd->Signature != FLASH_DESRIPTOR_MODE_SIG )
        {
            status = STATUS_UNSUCCESSFUL;
            EPrint("Wrong signature 0x%x! (0x%x)\n", fd->Signature, status);
            goto clean;
        }
    }

    uint32ToBin(*(PUINT32)&fd->FLMAP0, bin32_0);
    uint32ToBin(*(PUINT32)&fd->FLMAP1, bin32_1);
    uint32ToBin(*(PUINT32)&fd->FLMAP2, bin32_2);
    
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "Flash Descriptor\r\n"
                                "  Signature: 0x%x\r\n"
                                "  FLMAP0: 0x%x (0b%s)\r\n"
                                "    FCBA: 0x%x\r\n"
                                "    NC: 0x%x\r\n"
                                "    Reserved1: 0x%x\r\n"
                                "    FRBA: 0x%x\r\n"
                                "    NR: 0x%x\r\n"
                                "    Reserved2: 0x%x\r\n"
                                "  FLMAP1: 0x%x (0b%s)\r\n"
                                "    FMBA: 0x%x\r\n"
                                "    NM: 0x%x\r\n"
                                "    Reserved1: 0x%x\r\n"
                                "    FISBA: 0x%x\r\n"
                                "    ISL: 0x%x\r\n"
                                "  FLMAP2: 0x%x (0b%s)\r\n"
                                "    FMSBA: 0x%x\r\n"
                                "    MSL: 0x%x\r\n"
                                "    Reserved1: 0x%x\r\n"
                                "  \r\n",
                                fd->Signature,
                                *(PUINT32)&fd->FLMAP0, bin32_0,
                                    fd->FLMAP0.FCBA,
                                    fd->FLMAP0.NC,
                                    fd->FLMAP0.Reserved1,
                                    fd->FLMAP0.FRBA,
                                    fd->FLMAP0.NR,
                                    fd->FLMAP0.Reserved2,
                                *(PUINT32)&fd->FLMAP1, bin32_1,
                                    fd->FLMAP1.FMBA,
                                    fd->FLMAP1.NM,
                                    fd->FLMAP1.Reserved1,
                                    fd->FLMAP1.FISBA,
                                    fd->FLMAP1.ISL,
                                *(PUINT32)&fd->FLMAP2, bin32_2,
                                    fd->FLMAP2.FMSBA,
                                    fd->FLMAP2.MSL,
                                    fd->FLMAP2.Reserved1);
    
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    componentOffset = (fd->FLMAP0.FCBA<<4);
    nrComponents= fd->FLMAP0.NC+1;

    if ( !componentOffset 
        || componentOffset + sizeof(FLASH_DESCRIPTOR_COMPONENT_SECTION)*nrComponents > BufferSize )
        goto logRegisters;

    fdcs = (PFLASH_DESCRIPTOR_COMPONENT_SECTION)(fdBase+componentOffset);

    for ( i = 0; i < nrComponents; i++ )
    {
        uint32ToBin(*(PUINT32)&fdcs->FLCOMP, bin32_0);
        uint32ToBin(*(PUINT32)&fdcs->FLILL, bin32_1);
        uint32ToBin(*(PUINT32)&fdcs->FLPB, bin32_2);

        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "Component[%u]\r\n"
                                "  FLCOMP: 0x%x (0b%s)\r\n"
                                "    Component1Density: 0x%x (%s)\r\n"
                                "    Component2Density: 0x%x (%s)\r\n"
                                "    Reserved1: 0x%x\r\n"
                                "    ReadClockFrequency: 0x%x (%s)\r\n"
                                "    FastReadSupport: 0x%x\r\n"
                                "    FastReadClockFrequency: 0x%x (%s)\r\n"
                                "    WriteClockFrequency: 0x%x (%s)\r\n"
                                "    ReadIDAndStatusClockFrequency: 0x%x (%s)\r\n"
                                "    Reserved: 0x%x\r\n"
                                "  FLILL: 0x%x (0b%s)\r\n"
                                "    InvalidInstruction0: 0x%x\r\n"
                                "    InvalidInstruction1: 0x%x\r\n"
                                "    InvalidInstruction2: 0x%x\r\n"
                                "    InvalidInstruction3: 0x%x\r\n"
                                "  FLPB: 0x%x (0b%s)\r\n"
                                "    FPBA: 0x%x\r\n"
                                "    Reserved: 0x%x\r\n",
                                i,
                            *(PUINT32)&fdcs->FLCOMP, bin32_0,
                                fdcs->FLCOMP.Component1Density, COMPONENT_DENSITY_STR(fdcs->FLCOMP.Component1Density),
                                fdcs->FLCOMP.Component2Density, COMPONENT_DENSITY_STR(fdcs->FLCOMP.Component2Density),
                                fdcs->FLCOMP.Reserved1,
                                fdcs->FLCOMP.ReadClockFrequency, CLOCK_FREQ_STR(fdcs->FLCOMP.ReadClockFrequency),
                                fdcs->FLCOMP.FastReadSupport,
                                fdcs->FLCOMP.FastReadClockFrequency, CLOCK_FREQ_STR(fdcs->FLCOMP.FastReadClockFrequency),
                                fdcs->FLCOMP.WriteClockFrequency, CLOCK_FREQ_STR(fdcs->FLCOMP.WriteClockFrequency),
                                fdcs->FLCOMP.ReadIDAndStatusClockFrequency, CLOCK_FREQ_STR(fdcs->FLCOMP.ReadIDAndStatusClockFrequency),
                                fdcs->FLCOMP.Reserved,
                            *(PUINT32)&fdcs->FLILL, bin32_1,
                                fdcs->FLILL.InvalidInstruction0,
                                fdcs->FLILL.InvalidInstruction1,
                                fdcs->FLILL.InvalidInstruction2,
                                fdcs->FLILL.InvalidInstruction3,
                            *(PUINT32)&fdcs->FLPB, bin32_2,
                                fdcs->FLPB.FPBA,
                                fdcs->FLPB.Reserved);

        if ( logBufferRestSize < 0x100 )
            FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

        ++fdcs;
    }
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                          "\r\n");
    
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    
logRegisters:
    registerOffset = (fd->FLMAP0.FRBA<<4);
    //nrRegisters = fd->FLMAP0.NR+1;
    //if ( nrRegisters == 1 )
    //    nrRegisters = MAX_NUM_FLREG;
    // testing value, since values of NR, NM are undestood
    nrRegisters = MAX_NUM_FLREG;

    if ( !registerOffset 
        || registerOffset + sizeof(FLASH_DESCRIPTOR_REGION_SECTION)*nrRegisters > BufferSize )
        goto logMasters;

    fdrs = (PFLASH_DESCRIPTOR_REGION_SECTION)(fdBase+registerOffset);
    
    for ( i = 0; i < nrRegisters; i++ )
    {
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "Register[%u] 0x%x [%s] => 0x%x : 0x%x [0x%x]\r\n"
                                "  Base: 0x%x\r\n"
                                "  Reserved1: 0x%x\r\n"
                                "  Limit: 0x%x\r\n"
                                "  Reserved2: 0x%x\r\n",
                                i, *(PUINT32)&fdrs->FLREG[i], FD_REGION_NAME(i),
                                REGION_BASE_ADDRESS(fdrs->FLREG[i]), REGION_LIMIT(fdrs->FLREG[i]), REGION_SIZE(fdrs->FLREG[i]), 
                                fdrs->FLREG[i].RB, 
                                fdrs->FLREG[i].Reserved1, 
                                fdrs->FLREG[i].RL, 
                                fdrs->FLREG[i].Reserved2);

        if ( logBufferRestSize < 0x100 )
            FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                          "\r\n");
    
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    
logMasters:
    masterOffset = (fd->FLMAP1.FMBA<<4);
    nrMasters = fd->FLMAP1.NM+1;

    if ( !masterOffset 
        || masterOffset + sizeof(FLASH_DESCRIPTOR_MASTER_SECTION)*nrMasters > BufferSize )
        goto logIchStraps;

    fdms = (PFLASH_DESCRIPTOR_MASTER_SECTION)(fdBase+masterOffset);
    
    for ( i = 0; i < nrMasters; i++ )
    {
        uint32ToBin(*(PUINT32)&fdms->FlmStr[i], bin32_0);

        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "Master[%u] 0x%x (0b%s) [%s]\r\n"
                                "  RequesterID[15:0]: 0x%x\r\n"
                                "  FlashDescriptorReadAccess[16]: 0x%x\r\n"
                                "  HostCPUBIOSRegionReadAccess[17]: 0x%x\r\n"
                                "  MERegionReadAccess[18]: 0x%x\r\n"
                                "  GbERegionReadAccess[19]: 0x%x\r\n"
                                "  PlatformDataRegionReadAccess[20]: 0x%x\r\n"
                                //"  ECRegionReadAccess: 0x%x\r\n"
                                "  RES_23_21[23:21]: 0x%x\r\n"
                                "  FlashDescriptorRegionWriteAccess[24]: 0x%x\r\n"
                                "  HostCPUBIOSMasterRegionWriteAccess[25]: 0x%x\r\n"
                                "  MERegionWriteAccess[26]: 0x%x\r\n"
                                "  GbERegionWriteAccess[27]: 0x%x\r\n"
                                "  PlatformDataRegionWriteAccess[28]: 0x%x\r\n"
                                //"  ECWriteAccess: 0x%x\r\n"
                                "  RES_31_29[31:29]: 0x%x\r\n"
                                "  \r\n",
                                i, *(PUINT32)&fdms->FlmStr[i], bin32_0, MASTER_REGION_NAME(i),
                                fdms->FlmStr[i].RequesterID,
                                fdms->FlmStr[i].FlashDescriptorReadAccess,
                                fdms->FlmStr[i].HostCPUBIOSRegionReadAccess,
                                fdms->FlmStr[i].MERegionReadAccess,
                                fdms->FlmStr[i].GbERegionReadAccess,
                                fdms->FlmStr[i].PlatformDataRegionReadAccess,
                                //fdms->FlmStr[i].ECRegionReadAccess,
                                fdms->FlmStr[i].RES_23_21,
                                fdms->FlmStr[i].FlashDescriptorRegionWriteAccess,
                                fdms->FlmStr[i].HostCPUBIOSMasterRegionWriteAccess,
                                fdms->FlmStr[i].MERegionWriteAccess,
                                fdms->FlmStr[i].GbERegionWriteAccess,
                                fdms->FlmStr[i].PlatformDataRegionWriteAccess,
                                //fdms->FlmStr[i].ECWriteAccess,
                                fdms->FlmStr[i].RES_31_29);

        if ( logBufferRestSize < 0x100 )
            FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                          "\r\n");
    
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    
logIchStraps:
    ichStrapOffset = (fd->FLMAP1.FISBA<<4);
    nrIchStraps = fd->FLMAP1.ISL;

    if ( !ichStrapOffset 
        || ichStrapOffset + sizeof(UINT32)*nrIchStraps > BufferSize )
        goto logGmchStrap;
    
    buffer32 = (PUINT32)(fdBase+ichStrapOffset);

    for ( i = 0; i < nrIchStraps; i++ )
    {
        uint32ToBin(buffer32[i], bin32_0);

        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "IchStrap[%u]: 0x%08x (0b%s)\r\n",
                                i, buffer32[i], bin32_0);

        if ( logBufferRestSize < 0x100 )
            FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                          "\r\n");
    
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    
logGmchStrap:
    gmchStrapOffset = (fd->FLMAP2.FMSBA<<4);
    nrGmchStraps = fd->FLMAP2.MSL;

    if ( !gmchStrapOffset 
        || gmchStrapOffset + sizeof(UINT32)*nrGmchStraps > BufferSize )
        goto logFLUMAP1;
    
    buffer32 = (PUINT32)(fdBase+gmchStrapOffset);
    
    for ( i = 0; i < nrGmchStraps; i++ )
    {
        uint32ToBin(buffer32[i], bin32_0);

        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "GmchStrap[%u]: 0x%08x (0b%s)\r\n",
                                i, buffer32[i], bin32_0);

        if ( logBufferRestSize < 0x100 )
            FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                          "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);


logFLUMAP1:
    flumap1Offset = 0xEFC;
    flumap1Size = 8;

    if ( (SIZE_T)flumap1Offset + (SIZE_T)flumap1Size > BufferSize )
        goto logOem;
    
    buffer32 = (PUINT32)(fdBase+flumap1Offset);
    PFLUMAP1 flumap1 = (PFLUMAP1)buffer32;
    
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "flumap1:\r\n"
                            "  VTBA[7:0]: 0x%02x (0x%02x)\r\n"
                            "  VTL[15:8]: 0x%02x\r\n"
                            "  RES_31_16[31:16]: 0x%04x\r\n",
                            flumap1->VTBA, (flumap1->VTBA<<4),
                            flumap1->VTL,
                            flumap1->RES_31_16);

    UINT32 vtba = flumap1->VTBA<<4;
    // rounding the count up to whole 2-dword entries : "+1 & 1"
    // because a well formatted vtl has two uint32
    if ( !vtba || (SIZE_T)vtba + (SIZE_T)((flumap1->VTL + 1u) & ~1u) * sizeof(UINT32) > BufferSize )
        goto logOem;
    buffer32 = (PUINT32)(fdBase+vtba);
    
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "VSCC:\r\n");
    for ( i = 0; i < flumap1->VTL; i+=2 )
    {
        PJEDEC_ID jedec_id = (PJEDEC_ID)&buffer32[i];
        PVSCC vscc = (PVSCC)&buffer32[i+1];
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "  [%u] JEDEC_ID (0x%08x) (%s):\r\n"
                                "        VENDOR_ID[7:0]: 0x%02x\r\n"
                                "        DEVICE_ID[23:8]: 0x%02x 0x%02x\r\n"
                                "        RES_31_24[31:24]: 0x%02x\r\n"
                                "      VSCC (0%08x):\r\n"
                                "        UBES[1:0]: 0x%x (%s)\r\n"
                                "        UWG[2]: 0x%x (%s)\r\n"
                                "        UWSR[3]: 0x%x\r\n"
                                "        UWEWS[4]: 0x%x\r\n"
                                "        RES_7_5[7:5]: 0x%x\r\n"
                                "        UEO[15:8]: 0x%x\r\n"
                                "        LBES[17:16]: 0x%x (%s)\r\n"
                                "        LWG[18]: 0x%x (%s)\r\n"
                                "        LWSR[19]: 0x%x\r\n"
                                "        LWEWS[20]: 0x%x\r\n"
                                "        RES_23_21[23:21]: 0x%x\r\n"
                                "        LEO[31:24]: 0x%x\r\n",
                                (i/2), buffer32[i], jdecIdToString(buffer32[i]),
                                jedec_id->VENDOR_ID,jedec_id->DEVICE_ID_0,jedec_id->DEVICE_ID_1,jedec_id->RES_31_24, 
                                buffer32[i+1],
                                vscc->UBES, UBES_STR(vscc->UBES), vscc->UWG, UWG_STR(vscc->UWG), vscc->UWSR, vscc->UWEWS, vscc->RES_7_5, vscc->UEO, vscc->LBES, LBES_STR(vscc->LBES), vscc->LWG, LWG_STR(vscc->LWG), vscc->LWSR, vscc->LWEWS, vscc->RES_23_21, vscc->LEO
                            );

        if ( logBufferRestSize < 0x200 )
            FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                          "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

logOem:
    oemOffset = 0xf00;
    oemSize = 0x100;

    if ( (SIZE_T)oemOffset + (SIZE_T)oemSize > BufferSize )
        goto clean;
    
    buffer32 = (PUINT32)(fdBase+oemOffset);
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "OEM Section:\r\n");
    LogBytes(NULL, NULL, buffer32, oemSize, (PVOID)oemOffset, TRUE, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    //RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                          //"\r\n");
    //FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

clean:
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "\r\n\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    return status;
}
