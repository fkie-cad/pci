#pragma once



//#include "../common/strings.h"
#include "../pci/pciids.h"
#include "../pci/classCodesStrings.h"



typedef 
NTSTATUS
LOG_DEVICE_CONFIG(
    _In_ PVOID Buffer, 
    _In_ ULONG BufferSize, 
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
);
typedef LOG_DEVICE_CONFIG *PLOG_DEVICE_CONFIG;

#define REGISTER_LOG_DEVICE_CONFIG(__name__) \
    LOG_DEVICE_CONFIG __name__; \
    __pragma(alloc_text (PAGE, __name__))




NTSTATUS logPciConfig(
    _In_ PVOID Buffer, 
    _In_ ULONG BufferSize, 
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
);

#define PAM_DRAM_STATUS_STR(__v__) \
      (__v__==0b00)?"DRAM Disabled. All reads are sent to DRAM. All writes are forwarded to DMI." \
    : (__v__==0b01)?"Read Only. All reads are sent to DRAM. All writes are forwarded to DMI." \
    : (__v__==0b10)?"Write Only. All writes are sent to DRAM. All reads are serviced by DMI." \
    : (__v__==0b11)?"Normal DRAM Operation. All reads and writes are serviced by DRAM." \
    : "Unknown"

#define HOST_BRIDGE_DRAM_PCIEXBAR_LENGTH_STR(__v__) \
      (__v__==0b000)?"256MB (buses 0-255). Bits 38:28 are decoded in the PCI Express Base Address Field." \
    : (__v__==0b001)?"128MB (buses 0-127). Bits 38:27 are decoded in the PCI Express Base Address Field." \
    : (__v__==0b010)?"64MB (buses 0-63). Bits 38:26 are decoded in the PCI Express Base Address Field." \
    : (__v__==0b011)?"512MB (buses 0-512). Bits 38:29 are decoded in the PCI Express Base Address Field." \
    : (__v__==0b100)?"1024MB (buses 0-1024). Bits 38:30 are decoded in the PCI Express Base Address Field." \
    : (__v__==0b101)?"2048MB (buses 0-2048). Bits 38:31 are decoded in the PCI Express Base Address Field." \
    : (__v__==0b110)?"4096MB (buses 0-4096). Bits 38:32 are decoded in the PCI Express Base Address Field." \
    : (__v__==0b111)?"Reserved" \
    : "Unknown"

#define HOST_BRIDGE_DRAM_LAC_HEN_STR(__v__) \
      (__v__==0b00)?"No memory hole." \
    : (__v__==0b01)?"Memory hole from 15MB to 16MB." \
    : "Unknown"

#define HOST_BRIDGE_DRAM_CAPID1_DMFC_DDR3_STR(__v__) \
      (__v__==0b00)?"MC capable of DDR3 2667 (2667 is the upper limit)" \
    : (__v__==0b01)?"MC capable of up to DDR3 2667" \
    : (__v__==0b10)?"MC capable of up to DDR3 2400" \
    : (__v__==0b11)?"MC capable of up to DDR3 2133" \
    : (__v__==0b100)?"MC capable of up to DDR3 1867" \
    : (__v__==0b101)?"MC capable of up to DDR3 1600" \
    : (__v__==0b110)?"MC capable of up to DDR3 1333" \
    : (__v__==0b111)?"MC capable of up to DDR3 1067 " \
    : "Unknown"


FORCEINLINE
NTSTATUS logHostBridgeDRAMConfig(
    _In_ PVOID Buffer, 
    _In_ ULONG BufferSize, 
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
)
{
    PHOST_BRIDGE_DRAM_CFG cfg = (PHOST_BRIDGE_DRAM_CFG)Buffer;
    
    if ( BufferSize < 0x100 )
        return 0;

    NTSTATUS status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
        "PXPEPBAR [%02zx]: 0x%llx (PCI Express* Egress Port Base Address)\r\n"
        "  PXPEPBAREN: 0x%llx\r\n"
        "  Reserved1: 0x%llx\r\n"
        "  PXPEPBAR: 0x%llx (0x%x)\r\n"
        "  Reserved2: 0x%llx\r\n"
        "MCHBAR [%02zx]: 0x%llx (Host Memory Mapped Register Range Base)\r\n"
        "  MCHBAREN: 0x%llx\r\n"
        "  Reserved1: 0x%llx\r\n"
        "  MCHBAR: 0x%llx (0x%x)\r\n"
        "  Reserved2: 0x%llx\r\n"
        "GMCH [%02zx]: 0x%x (GMCH Graphics Control Register)\r\n"
        "  GGCLCK: 0x%x\r\n"
        "  IVD: 0x%x\r\n"
        "  VAMEN: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "  GGMS: 0x%x\r\n"
        "  GMS: 0x%x\r\n"
        "DEVEN [%02zx]: 0x%x (Device Enable)\r\n"
        "  D0EN: 0x%x\r\n"
        "  D1F2EN: 0x%x\r\n"
        "  D1F1EN: 0x%x\r\n"
        "  D1F0EN: 0x%x\r\n"
        "  D2EN: 0x%x\r\n"
        "  D3EN: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "  D4EN: 0x%x\r\n"
        "  Reserved2: 0x%x\r\n"
        "  D5EN: 0x%x\r\n"
        "  Reserved3: 0x%x\r\n"
        "  D6EN: 0x%x\r\n"
        "  D7EN: 0x%x\r\n"
        "  D8EN: 0x%x\r\n"
        "  Reserved4: 0x%x\r\n"
        "PAVPC [%02zx]: 0x%x (Protected Audio Video Path Control)\r\n"
        "  PCME: 0x%x\r\n"
        "  PAVPE: 0x%x\r\n"
        "  PAVPLCK: 0x%x\r\n"
        "  HVYMODSEL: 0x%x\r\n"
        "  OVTATTACK: 0x%x\r\n"
        "  RSVD1: 0x%x\r\n"
        "  ASMFEN: 0x%x\r\n"
        "  RSVD2: 0x%x\r\n"
        "  PCMBASE: 0x%x\r\n"
        "DPR [%02zx]: 0x%x (DMA Protected Range)\r\n"
        "  LOCK: 0x%x\r\n"
        "  PRS: 0x%x\r\n"
        "  EPM: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "  DPRSIZE: 0x%x\r\n"
        "  Reserved2: 0x%x\r\n"
        "  TopOfDPR: 0x%x\r\n"
        "PCIEXBAR [%02zx]: 0x%llx (PCI Express Register Range Base Address)\r\n"
        "  PCIEXBAREN: 0x%llx\r\n"
        "  LENGTH: 0x%llx (%s)\r\n"
        "  Reserved1: 0x%llx\r\n"
        "  ADMSK64: 0x%llx\r\n"
        "  ADMSK128: 0x%llx\r\n"
        "  PCIEXBAR: 0x%llx (0x%llx)\r\n"
        "  RSVD: 0x%llx\r\n",
        offsetof(HOST_BRIDGE_DRAM_CFG, PXPEPBAR), *(PUINT64)&cfg->PXPEPBAR,
            cfg->PXPEPBAR.PXPEPBAREN,
            cfg->PXPEPBAR.Reserved1,
            cfg->PXPEPBAR.PXPEPBAR, ((UINT32)(*(PUINT64)&cfg->PXPEPBAR)&(~1)),
            cfg->PXPEPBAR.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, MCHBAR), *(PUINT64)&cfg->MCHBAR,
            cfg->MCHBAR.MCHBAREN,
            cfg->MCHBAR.Reserved1,
            cfg->MCHBAR.MCHBAR, ((UINT32)(*(PUINT64)&cfg->MCHBAR)&(~1)),
            cfg->MCHBAR.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, GMCH), *(PUINT32)&cfg->GMCH,
            cfg->GMCH.GGCLCK,
            cfg->GMCH.IVD,
            cfg->GMCH.VAMEN,
            cfg->GMCH.Reserved1,
            cfg->GMCH.GGMS,
            cfg->GMCH.GMS,
        offsetof(HOST_BRIDGE_DRAM_CFG, DEVEN), *(PUINT32)&cfg->DEVEN,
            cfg->DEVEN.D0EN,
            cfg->DEVEN.D1F2EN,
            cfg->DEVEN.D1F1EN,
            cfg->DEVEN.D1F0EN,
            cfg->DEVEN.D2EN,
            cfg->DEVEN.D3EN,
            cfg->DEVEN.Reserved1,
            cfg->DEVEN.D4EN,
            cfg->DEVEN.Reserved2,
            cfg->DEVEN.D5EN,
            cfg->DEVEN.Reserved3,
            cfg->DEVEN.D6EN,
            cfg->DEVEN.D7EN,
            cfg->DEVEN.D8EN,
            cfg->DEVEN.Reserved4,
        offsetof(HOST_BRIDGE_DRAM_CFG, PAVPC), *(PUINT32)&cfg->PAVPC,
            cfg->PAVPC.PCME,
            cfg->PAVPC.PAVPE,
            cfg->PAVPC.PAVPLCK,
            cfg->PAVPC.HVYMODSEL,
            cfg->PAVPC.OVTATTACK,
            cfg->PAVPC.RSVD1,
            cfg->PAVPC.ASMFEN,
            cfg->PAVPC.RSVD2,
            cfg->PAVPC.PCMBASE,
        offsetof(HOST_BRIDGE_DRAM_CFG, DPR), *(PUINT32)&cfg->DPR,
            cfg->DPR.LOCK,
            cfg->DPR.PRS,
            cfg->DPR.EPM,
            cfg->DPR.Reserved1,
            cfg->DPR.DPRSIZE,
            cfg->DPR.Reserved2,
            cfg->DPR.PAVPC,
        offsetof(HOST_BRIDGE_DRAM_CFG, PCIEXBAR), *(PUINT64)&cfg->PCIEXBAR,
            cfg->PCIEXBAR.PCIEXBAREN,
            cfg->PCIEXBAR.LENGTH, HOST_BRIDGE_DRAM_PCIEXBAR_LENGTH_STR(cfg->PCIEXBAR.LENGTH),
            cfg->PCIEXBAR.Reserved1,
            cfg->PCIEXBAR.ADMSK64,
            cfg->PCIEXBAR.ADMSK128,
            cfg->PCIEXBAR.PCIEXBAR,cfg->PCIEXBAR.PCIEXBAR<<28,
            cfg->PCIEXBAR.RSVD);
        
    FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);
    
    status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
        "DMIBAR [%02zx]: 0x%llx (Root Complex Register Range Base Address)\r\n"
        "  DMIBAREN: 0x%llx\r\n"
        "  Reserved1: 0x%llx\r\n"
        "  DMIBAR: 0x%llx (0x%x)\r\n"
        "  Reserved2: 0x%llx\r\n"
        "MESEG_BASE [%02zx]: 0x%llx (Manageability Engine Base Address Register)\r\n"
        "  Reserved1: 0x%llx\r\n"
        "  MEBASE: 0x%llx\r\n"
        "  Reserved2: 0x%llx\r\n"
        "MESEG_MASK [%02zx]: 0x%llx (Manageability Engine Limit Address Register)\r\n"
        "  Reserved1: 0x%llx\r\n"
        "  MELCK: 0x%llx\r\n"
        "  ME_STLEN_EN: 0x%llx\r\n"
        "  Reserved2: 0x%llx\r\n"
        "  MEMASK: 0x%llx\r\n"
        "  Reserved3: 0x%llx\r\n"
        "PAM0 [%02zx]: 0x%x (Programmable Attribute Map 0)\r\n"
        "  Lock: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "  HIENABLE: 0x%x (%s)\r\n"
        "  Reserved2: 0x%x\r\n"
        "PAM1 [%02zx]: 0x%x (Programmable Attribute Map 1)\r\n"
        "  LOENABLE: 0x%x (%s)\r\n"
        "  Reserved1: 0x%x\r\n"
        "  HIENABLE: 0x%x (%s)\r\n"
        "  Reserved2: 0x%x\r\n"
        "PAM2 [%02zx]: 0x%x (Programmable Attribute Map 2)\r\n"
        "  LOENABLE: 0x%x (%s)\r\n"
        "  Reserved1: 0x%x\r\n"
        "  HIENABLE: 0x%x (%s)\r\n"
        "  Reserved2: 0x%x\r\n"
        "PAM3 [%02zx]: 0x%x (Programmable Attribute Map 3)\r\n"
        "  LOENABLE: 0x%x (%s)\r\n"
        "  Reserved1: 0x%x\r\n"
        "  HIENABLE: 0x%x (%s)\r\n"
        "  Reserved2: 0x%x\r\n"
        "PAM4 [%02zx]: 0x%x (Programmable Attribute Map 4)\r\n"
        "  LOENABLE: 0x%x (%s)\r\n"
        "  Reserved1: 0x%x\r\n"
        "  HIENABLE: 0x%x (%s)\r\n"
        "  Reserved2: 0x%x\r\n"
        "PAM5 [%02zx]: 0x%x (Programmable Attribute Map 5)\r\n"
        "  LOENABLE: 0x%x (%s)\r\n"
        "  Reserved1: 0x%x\r\n"
        "  HIENABLE: 0x%x (%s)\r\n"
        "  Reserved2: 0x%x\r\n"
        "PAM6 [%02zx]: 0x%x (Programmable Attribute Map 6)\r\n"
        "  LOENABLE: 0x%x (%s)\r\n"
        "  Reserved1: 0x%x\r\n"
        "  HIENABLE: 0x%x (%s)\r\n"
        "  Reserved2: 0x%x\r\n",
        offsetof(HOST_BRIDGE_DRAM_CFG, DMIBAR), *(PUINT64)&cfg->DMIBAR,
            cfg->DMIBAR.DMIBAREN,
            cfg->DMIBAR.Reserved1,
            cfg->DMIBAR.DMIBAR, ((UINT32)(*(PUINT64)&cfg->DMIBAR)&(~1)),
            cfg->DMIBAR.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, MESEG_BASE), *(PUINT64)&cfg->MESEG_BASE,
            cfg->MESEG_BASE.Reserved1,
            cfg->MESEG_BASE.MEBASE,
            cfg->MESEG_BASE.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, MESEG_MASK), *(PUINT64)&cfg->MESEG_MASK,
            cfg->MESEG_MASK.Reserved1,
            cfg->MESEG_MASK.MELCK,
            cfg->MESEG_MASK.ME_STLEN_EN,
            cfg->MESEG_MASK.Reserved2,
            cfg->MESEG_MASK.MEMASK,
            cfg->MESEG_MASK.Reserved3,
        offsetof(HOST_BRIDGE_DRAM_CFG, PAM0), *(PUINT8)&cfg->PAM0,
            cfg->PAM0.Lock,
            cfg->PAM0.Reserved1,
            cfg->PAM0.HIENABLE, PAM_DRAM_STATUS_STR(cfg->PAM0.HIENABLE),
            cfg->PAM0.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, PAM1), *(PUINT8)&cfg->PAM1,
            cfg->PAM1.LOENABLE, PAM_DRAM_STATUS_STR(cfg->PAM1.LOENABLE),
            cfg->PAM1.Reserved1,
            cfg->PAM1.HIENABLE, PAM_DRAM_STATUS_STR(cfg->PAM1.HIENABLE),
            cfg->PAM1.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, PAM2), *(PUINT8)&cfg->PAM2,
            cfg->PAM2.LOENABLE, PAM_DRAM_STATUS_STR(cfg->PAM2.LOENABLE),
            cfg->PAM2.Reserved1,
            cfg->PAM2.HIENABLE, PAM_DRAM_STATUS_STR(cfg->PAM2.HIENABLE),
            cfg->PAM2.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, PAM3), *(PUINT8)&cfg->PAM3,
            cfg->PAM3.LOENABLE, PAM_DRAM_STATUS_STR(cfg->PAM3.LOENABLE),
            cfg->PAM3.Reserved1,
            cfg->PAM3.HIENABLE, PAM_DRAM_STATUS_STR(cfg->PAM3.HIENABLE),
            cfg->PAM3.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, PAM4), *(PUINT8)&cfg->PAM4,
            cfg->PAM4.LOENABLE, PAM_DRAM_STATUS_STR(cfg->PAM4.LOENABLE),
            cfg->PAM4.Reserved1,
            cfg->PAM4.HIENABLE, PAM_DRAM_STATUS_STR(cfg->PAM4.HIENABLE),
            cfg->PAM4.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, PAM5), *(PUINT8)&cfg->PAM5,
            cfg->PAM5.LOENABLE, PAM_DRAM_STATUS_STR(cfg->PAM5.LOENABLE),
            cfg->PAM5.Reserved1,
            cfg->PAM5.HIENABLE, PAM_DRAM_STATUS_STR(cfg->PAM5.HIENABLE),
            cfg->PAM5.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, PAM6), *(PUINT8)&cfg->PAM6,
            cfg->PAM6.LOENABLE, PAM_DRAM_STATUS_STR(cfg->PAM6.LOENABLE),
            cfg->PAM6.Reserved1,
            cfg->PAM6.HIENABLE, PAM_DRAM_STATUS_STR(cfg->PAM6.HIENABLE),
            cfg->PAM6.Reserved2
        );

    FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);

    status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
        "LAC [%02zx]: 0x%x (Legacy Access Control)\r\n"
        "  MDAP10: 0x%x\r\n"
        "  MDAP11: 0x%x\r\n"
        "  MDAP12: 0x%x\r\n"
        "  MDAP60: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "  HEN: 0x%x (%s)\r\n"
        "SMRAMC [%02zx]: 0x%x (System Management RAM Control)\r\n"
        "  C_BASE_SEG: 0x%x\r\n"
        "  G_SMRAME: 0x%x\r\n"
        "  D_LCK: 0x%x\r\n"
        "  D_CLS: 0x%x\r\n"
        "  D_OPEN: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "Reserved1 [%02zx]: %02x %02x %02x %02x %02x %02x\r\n"
        "REMAPBASE [%02zx]: 0x%llx (Remap Base Address Register)\r\n"
        "  LOCK: 0x%llx\r\n"
        "  Reserved1: 0x%llx\r\n"
        "  REMAPBASE: 0x%llx\r\n"
        "  Reserved2: 0x%llx\r\n"
        "REMAPLIMIT [%02zx]: 0x%llx (Remap Limit Address Register)\r\n"
        "  LOCK: 0x%llx\r\n"
        "  Reserved1: 0x%llx\r\n"
        "  REMAPLMT: 0x%llx\r\n"
        "  Reserved2: 0x%llx\r\n"
        "TOM [%02zx]: 0x%llx (Top of Memory)\r\n"
        "  LOCK: 0x%llx\r\n"
        "  Reserved1: 0x%llx\r\n"
        "  TOM: 0x%llx\r\n"
        "  Reserved2: 0x%llx\r\n"
        "TOUUD [%02zx]: 0x%llx (Top of Upper Usable DRAM)\r\n"
        "  LOCK: 0x%llx\r\n"
        "  Reserved1: 0x%llx\r\n"
        "  TOUUD: 0x%llx\r\n"
        "  Reserved2: 0x%llx\r\n"
        "BDSM [%02zx]: 0x%x (Base Data of Stolen Memory)\r\n"
        "  LOCK: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "  BDSM: 0x%x\r\n"
        "BGSM [%02zx]: 0x%x (Base of GTT stolen Memory)\r\n"
        "  LOCK: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "  BGSM: 0x%x (0x%x)\r\n"
        "TSEGMB [%02zx]: 0x%x (TSEG Memory Base)\r\n"
        "  LOCK: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "  TSEGMB: 0x%x\r\n"
        "TOLUD [%02zx]: 0x%x (Top of Low Usable DRAM)\r\n"
        "  LOCK: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "  TOLUD: 0x%x\r\n"
        "Reserved2 [%02zx]: %02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "ERRSTS [%02zx]: 0x%x (Error Status)\r\n"
        "  DSERR: 0x%x\r\n"
        "  DMERR: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "ERRCMD [%02zx]: 0x%x (Error Command)\r\n"
        "  DSERR: 0x%x\r\n"
        "  DMERR: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "SMICMD [%02zx]: 0x%x (SMI Command)\r\n"
        "  DSESMI: 0x%x\r\n"
        "  DMESMI: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n"
        "SCICMD [%02zx]: 0x%x (SCI Command)\r\n"
        "  DSESCI: 0x%x\r\n"
        "  DMESCI: 0x%x\r\n"
        "  Reserved1: 0x%x\r\n",
        offsetof(HOST_BRIDGE_DRAM_CFG, LAC), *(PUINT8)&cfg->LAC,
            cfg->LAC.MDAP10,
            cfg->LAC.MDAP11,
            cfg->LAC.MDAP12,
            cfg->LAC.MDAP60,
            cfg->LAC.Reserved1,
            cfg->LAC.HEN, HOST_BRIDGE_DRAM_LAC_HEN_STR(cfg->LAC.HEN),
        offsetof(HOST_BRIDGE_DRAM_CFG, SMRAMC), *(PUINT8)&cfg->SMRAMC,
            cfg->SMRAMC.C_BASE_SEG,
            cfg->SMRAMC.G_SMRAME,
            cfg->SMRAMC.D_LCK,
            cfg->SMRAMC.D_CLS,
            cfg->SMRAMC.D_OPEN,
            cfg->SMRAMC.Reserved1,
        offsetof(HOST_BRIDGE_DRAM_CFG, Reserved1), 
                cfg->Reserved1[0],cfg->Reserved1[1],cfg->Reserved1[2],cfg->Reserved1[3],cfg->Reserved1[4],cfg->Reserved1[5],
        offsetof(HOST_BRIDGE_DRAM_CFG, REMAPBASE), *(PUINT64)&cfg->REMAPBASE,
            cfg->REMAPBASE.LOCK,
            cfg->REMAPBASE.Reserved1,
            cfg->REMAPBASE.REMAPBASE,
            cfg->REMAPBASE.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, REMAPLIMIT), *(PUINT64)&cfg->REMAPLIMIT,
            cfg->REMAPLIMIT.LOCK,
            cfg->REMAPLIMIT.Reserved1,
            cfg->REMAPLIMIT.REMAPLMT,
            cfg->REMAPLIMIT.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, TOM), *(PUINT64)&cfg->TOM,
            cfg->TOM.LOCK,
            cfg->TOM.Reserved1,
            cfg->TOM.TOM,
            cfg->TOM.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, TOUUD), *(PUINT64)&cfg->TOUUD,
            cfg->TOUUD.LOCK,
            cfg->TOUUD.Reserved1,
            cfg->TOUUD.TOUUD,
            cfg->TOUUD.Reserved2,
        offsetof(HOST_BRIDGE_DRAM_CFG, BDSM), *(PUINT32)&cfg->BDSM,
            cfg->BDSM.LOCK,
            cfg->BDSM.Reserved1,
            cfg->BDSM.BDSM,
        offsetof(HOST_BRIDGE_DRAM_CFG, BGSM), *(PUINT32)&cfg->BGSM,
            cfg->BGSM.LOCK,
            cfg->BGSM.Reserved1,
            cfg->BGSM.BGSM,cfg->BGSM.BGSM<<20,
        offsetof(HOST_BRIDGE_DRAM_CFG, TSEGMB), *(PUINT32)&cfg->TSEGMB,
            cfg->TSEGMB.LOCK,
            cfg->TSEGMB.Reserved1,
            cfg->TSEGMB.TSEGMB,
        offsetof(HOST_BRIDGE_DRAM_CFG, TOLUD), *(PUINT32)&cfg->TOLUD,
            cfg->TOLUD.LOCK,
            cfg->TOLUD.Reserved1,
            cfg->TOLUD.TOLUD,
        offsetof(HOST_BRIDGE_DRAM_CFG, Reserved2), 
            cfg->Reserved2[0],cfg->Reserved2[1],cfg->Reserved2[2],cfg->Reserved2[3],cfg->Reserved2[4],cfg->Reserved2[5],cfg->Reserved2[6],cfg->Reserved2[7],
        offsetof(HOST_BRIDGE_DRAM_CFG, ERRSTS), *(PUINT32)&cfg->ERRSTS,
            cfg->ERRSTS.DSERR,
            cfg->ERRSTS.DMERR,
            cfg->ERRSTS.Reserved1,
        offsetof(HOST_BRIDGE_DRAM_CFG, ERRCMD), *(PUINT32)&cfg->ERRCMD,
            cfg->ERRCMD.DSERR,
            cfg->ERRCMD.DMERR,
            cfg->ERRCMD.Reserved1,
        offsetof(HOST_BRIDGE_DRAM_CFG, SMICMD), *(PUINT32)&cfg->SMICMD,
            cfg->SMICMD.DSESMI,
            cfg->SMICMD.DMESMI,
            cfg->SMICMD.Reserved1,
        offsetof(HOST_BRIDGE_DRAM_CFG, SCICMD), *(PUINT32)&cfg->SCICMD,
            cfg->SCICMD.DSESCI,
            cfg->SCICMD.DMESCI,
            cfg->SCICMD.Reserved1);
        
    FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);

    status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
        "Reserved3 [%02zx]: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x\r\n"
        "SKPD [%02zx]: 0x%x (Scratchpad Data)\r\n"
        "  SKPD: 0x%x\r\n"
        "Reserved4 [%02zx]: 0x%x\r\n"
        "CAPID0 [%02zx]: 0x%x (Capabilities A)\r\n"
        "  Reserved1: 0x%x\r\n"
        "  PDCD: 0x%x\r\n"
        "  X2APIC_EN: 0x%x\r\n"
        "  DDPCD: 0x%x\r\n"
        "  Reserved2: 0x%x\r\n"
        "  VTDD: 0x%x\r\n"
        "  Reserved3: 0x%x\r\n"
        "  ECCDIS: 0x%x\r\n"
        "  Reserved4: 0x%x\r\n"
        "CAPID1 [%02zx]: 0x%x (Capabilities B)\r\n"
        "  Reserved1: 0x%x\r\n"
        "  LPDDR3_EN: 0x%x\r\n"
        "  Reserved2: 0x%x\r\n"
        "  DMFC_DDR3: 0x%x (%s)\r\n"
        "  Reserved3: 0x%x\r\n"
        "  GMM_DIS: 0x%x\r\n"
        "  Reserved4: 0x%x\r\n"
        "  DMIG3DIS: 0x%x\r\n"
        "  Reserved5: 0x%x\r\n"
        "  ADDGFXCAP: 0x%x\r\n"
        "  ADDGFXEN: 0x%x\r\n"
        "  Reserved6: 0x%x\r\n"
        "  PEGG3_DIS: 0x%x\r\n"
        "  Reserved7: 0x%x\r\n"
        "  CACHESZ: 0x%x\r\n"
        "  SMT: 0x%x\r\n"
        "  Reserved8: 0x%x\r\n"
        "  IMGU_DIS: 0x%x\r\n"
        "CAPID2 [%02zx]: 0x%x (Capabilities C)\r\n"
        "  Reserved1: 0x%x\r\n"
        "  DMFC_LPDDR3: 0x%x\r\n"
        "  DMFC_DDR4: 0x%x\r\n"
        "  Reserved2: 0x%x\r\n",
        offsetof(HOST_BRIDGE_DRAM_CFG, Reserved3), 
            cfg->Reserved3[0],cfg->Reserved3[1],cfg->Reserved3[2],cfg->Reserved3[3],cfg->Reserved3[4],cfg->Reserved3[5],cfg->Reserved3[6],cfg->Reserved3[7],cfg->Reserved3[0x8],cfg->Reserved3[0x9],cfg->Reserved3[0xa],cfg->Reserved3[0xb],
        offsetof(HOST_BRIDGE_DRAM_CFG, SKPD), *(PUINT32)&cfg->SKPD,
            cfg->SKPD.SKPD,
        offsetof(HOST_BRIDGE_DRAM_CFG, Reserved4), *(PUINT32)&cfg->Reserved4,
        offsetof(HOST_BRIDGE_DRAM_CFG, CAPID0), *(PUINT32)&cfg->CAPID0,
            cfg->CAPID0.Reserved1,
            cfg->CAPID0.PDCD,
            cfg->CAPID0.X2APIC_EN,
            cfg->CAPID0.DDPCD,
            cfg->CAPID0.Reserved2,
            cfg->CAPID0.VTDD,
            cfg->CAPID0.Reserved3,
            cfg->CAPID0.ECCDIS,
            cfg->CAPID0.Reserved4,
        offsetof(HOST_BRIDGE_DRAM_CFG, CAPID1), *(PUINT32)&cfg->CAPID1,
            cfg->CAPID1.Reserved1,
            cfg->CAPID1.LPDDR3_EN,
            cfg->CAPID1.Reserved2,
            cfg->CAPID1.DMFC_DDR3, HOST_BRIDGE_DRAM_CAPID1_DMFC_DDR3_STR(cfg->CAPID1.DMFC_DDR3),
            cfg->CAPID1.Reserved3,
            cfg->CAPID1.GMM_DIS,
            cfg->CAPID1.Reserved4,
            cfg->CAPID1.DMIG3DIS,
            cfg->CAPID1.Reserved5,
            cfg->CAPID1.ADDGFXCAP,
            cfg->CAPID1.ADDGFXEN,
            cfg->CAPID1.Reserved6,
            cfg->CAPID1.PEGG3_DIS,
            cfg->CAPID1.Reserved7,
            cfg->CAPID1.CACHESZ,
            cfg->CAPID1.SMT,
            cfg->CAPID1.Reserved8,
            cfg->CAPID1.IMGU_DIS,
        offsetof(HOST_BRIDGE_DRAM_CFG, CAPID2), *(PUINT32)&cfg->CAPID2,
            cfg->CAPID2.Reserved1,
            cfg->CAPID2.DMFC_LPDDR3,
            cfg->CAPID2.DMFC_DDR4,
            cfg->CAPID2.Reserved2);
    
    return status;
};


#define BIOS_LPC_IOD_CX_STR(__v__) \
      (__v__ == 0b000) ? "3F8h - 3FFh (COM 1)" \
    : (__v__ == 0b001) ? "2F8h - 2FFh (COM 2)" \
    : (__v__ == 0b010) ? "220h - 227h" \
    : (__v__ == 0b011) ? "228h - 22Fh" \
    : (__v__ == 0b100) ? "238h - 23Fh" \
    : (__v__ == 0b101) ? "2E8h - 2EFh (COM 4)" \
    : (__v__ == 0b110) ? "338h - 33Fh" \
    : (__v__ == 0b111) ? "3E8h - 3EFh (COM 3)" \
    : "Unknown"

#define BIOS_LPC_IOD_LPT_STR(__v__) \
      (__v__ == 0b000) ? "378h - 37Fh and 778h - 77Fh" \
    : (__v__ == 0b001) ? "278h - 27Fh (port 279h is read only) and 678h - 67Fh" \
    : (__v__ == 0b010) ? "3BCh - 3BEh and 7BCh - 7BEh" \
    : (__v__ == 0b011) ? "Reserved" \
    : "Unknown"

#define BIOS_LPC_IOD_FDD_STR(__v__) \
      (__v__ == 0b000) ? "3F0h - 3F5h, 3F7h (Primary)" \
    : (__v__ == 0b001) ? "370h - 375h, 377h (Secondary)" \
    : "Unknown"

#define BIOS_LPC_PCCTL_PCLKVLD_CFG_STR(__v__) \
     (__v__==0b00) ? "1 flop stage of delay from STP_PCI# (default)" \
    :(__v__==0b01) ? "No delay (edges match STP_PCI#" \
    :(__v__==0b10) ? "2 flop stages of delay from STP_PCI#" \
    :(__v__==0b11) ? "Tie high (indicate that LPC clock is always valid)" \
    : "Unknown"

#define BIOS_SPI_BBS_STR(__bbs__) \
      (__bbs__ == 0) ? "SPI" \
    : (__bbs__ == 1) ? "LPC" \
    : "Unknown"


FORCEINLINE
UINT8 getSFPWclocks(UINT8 SFPW)
{
    switch ( SFPW )
    {
        case 0: return 4;
        case 1: return 6;
        case 2: return 8;
        default: return 0;
    }
}

FORCEINLINE
NTSTATUS logThermalSubsystemConfig(
    _In_ PVOID Buffer, 
    _In_ ULONG BufferSize, 
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
)
{
    PTHERMAL_SUBSYSTEM_CFG cfg = (PTHERMAL_SUBSYSTEM_CFG)Buffer;
    
    (LogFile);
    (LogBuffer);
    (LogBufferSize);

    if ( BufferSize < 0x100 )
        return 0;

    NTSTATUS status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                "TBARB [%02zx]: 0x%08x (BIOS Assigned Thermal Base Address)\r\n"
                "  SPTYPEN: 0x%x (Space Type Enable)\r\n"
                "  ADDRNG: 0x%x (Address Range)\r\n"
                "  PREF: 0x%x (Prefetchable)\r\n"
                "  Reserved: 0x%x\r\n"
                "  TBA: 0x%x (Thermal Base Address)\r\n"
                "TBARBH [%02zx]: 0x%08x (BIOS Assigned Thermal High Address)\r\n"
                "  TBAH: 0x%x (Thermal Base Address High)\r\n"
                "CB [%02zx]: 0x%02x (Control Bits)\r\n"
                "  UR [0]: 0x%02x (Reporting Enable)\r\n"
                "  RES_7_1 [7:1]: 0x%02x (Reserved)\r\n"
                    ,
                offsetof(THERMAL_SUBSYSTEM_CFG, TBARB), *(PUINT32)&cfg->TBARB,
                    cfg->TBARB.SPTYPEN,
                    cfg->TBARB.ADDRNG,
                    cfg->TBARB.PREF,
                    cfg->TBARB.RES_11_4,
                    cfg->TBARB.TBA,
                offsetof(THERMAL_SUBSYSTEM_CFG, TBARBH), *(PUINT32)&cfg->TBARBH,
                    cfg->TBARBH.TBAH,
                offsetof(THERMAL_SUBSYSTEM_CFG, CB), *(PUINT8)&cfg->CB,
                    cfg->CB.URRE,
                    cfg->CB.RES_7_1);

    FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);

    return status;
};

REGISTER_LOG_DEVICE_CONFIG(logSMBusConfig);
NTSTATUS logSMBusConfig(
    _In_ PVOID Buffer, 
    _In_ ULONG BufferSize, 
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
)
{
    PAGED_CODE();

    PSMBUS_INTERFACE cfg = (PSMBUS_INTERFACE)Buffer;
    
    (LogFile);
    (LogBuffer);
    (LogBufferSize);

    if ( BufferSize < 0x100 )
        return 0;

    NTSTATUS status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                "HCFG [%02zx]: 0x%08x (Host Configuration)\r\n"
                "  HSTEN: 0x%x ()\r\n"
                "  SSEN: 0x%x ()\r\n"
                "  I2CEN: 0x%x ()\r\n"
                "  SSRESET: 0x%x ()\r\n"
                "  SPDWD: 0x%x (SPD Write Disable)\r\n"
                "  RES_7_5: 0x%x ()\r\n"
                "  RES_31_8: 0x%x ()\r\n"
                "RES_4F_44: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x\r\n"
                "TCOBASE [%02zx]: 0x%08x (TCO Base Address)\r\n"
                "  IOS: 0x%x (I/O Space)\r\n"
                "  RES_4_1: 0x%x ()\r\n"
                "  TCOBA: 0x%x (TCO Base Address)\r\n"
                "  RES_31_16: 0x%x ()\r\n"
                "TCOCTL [%02zx]: 0x%08x ()\r\n"
                "  TCO_BASE_LOCK [0]: 0x%x (TCO Base Lock)\r\n"
                "  RES_7_1 [0]: 0x%x ()\r\n"
                "  TCO_BASE_EN [0]: 0x%x (TCO Base Enable)\r\n"
                "  RES_7_1 [7:1]: 0x%x ()\r\n"
                "RES_63_58: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x\r\n"
                "HTIM [%02zx]: 0x%08x (Host SMBus Timing)\r\n"
                "  RES_31_0 [31:0]: 0x%x ()\r\n"
                "RES_7F_68: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
                "           %02x %02x %02x %02x %02x %02x %02x %02x\r\n"
                "SMBSM [%02zx]: 0x%08x (SMBus Power Gating)\r\n"
                "  RES_17_0 [17:0]: 0x%x\r\n"
                "  PGCBDCGDIS [18]: 0x%x (SMBus Dynamic Clock Gating)\r\n"
                "  RES_31_19 [31:19]: 0x%x\r\n"
                    ,
                offsetof(SMBUS_INTERFACE, HCFG), *(PUINT32)&cfg->HCFG,
                    cfg->HCFG.HSTEN,
                    cfg->HCFG.SSEN,
                    cfg->HCFG.I2CEN,
                    cfg->HCFG.SSRESET,
                    cfg->HCFG.SPDWD,
                    cfg->HCFG.RES_7_5,
                    cfg->HCFG.RES_31_8,
                cfg->RES_4F_44[0x0],cfg->RES_4F_44[0x1],cfg->RES_4F_44[0x2],cfg->RES_4F_44[0x3],cfg->RES_4F_44[0x4],cfg->RES_4F_44[0x5],cfg->RES_4F_44[0x6],cfg->RES_4F_44[0x7],cfg->RES_4F_44[0x8],cfg->RES_4F_44[0x9],cfg->RES_4F_44[0xA],cfg->RES_4F_44[0xB],
                offsetof(SMBUS_INTERFACE, TCOBASE), *(PUINT32)&cfg->TCOBASE,
                    cfg->TCOBASE.IOS,
                    cfg->TCOBASE.RES_4_1,
                    cfg->TCOBASE.TCOBA,
                    cfg->TCOBASE.RES_31_16,
                offsetof(SMBUS_INTERFACE, TCOCTL), *(PUINT32)&cfg->TCOCTL,
                    cfg->TCOCTL.TCO_BASE_LOCK,
                    cfg->TCOCTL.RES_7_1,
                    cfg->TCOCTL.TCO_BASE_EN,
                    cfg->TCOCTL.RES_31_9,
                cfg->RES_63_58[0x0],cfg->RES_63_58[0x1],cfg->RES_63_58[0x2],cfg->RES_63_58[0x3],cfg->RES_63_58[0x4],cfg->RES_63_58[0x5],cfg->RES_63_58[0x6],cfg->RES_63_58[0x7],cfg->RES_63_58[0x8],cfg->RES_63_58[0x9],cfg->RES_63_58[0xA],cfg->RES_63_58[0xB],
                offsetof(SMBUS_INTERFACE, HTIM), *(PUINT32)&cfg->HTIM,
                    cfg->HTIM.RES_31_0,
                cfg->RES_7F_68[0x0],cfg->RES_7F_68[0x1],cfg->RES_7F_68[0x2],cfg->RES_7F_68[0x3],cfg->RES_7F_68[0x4],cfg->RES_7F_68[0x5],cfg->RES_7F_68[0x6],cfg->RES_7F_68[0x7],cfg->RES_7F_68[0x8],cfg->RES_7F_68[0x9],cfg->RES_7F_68[0xA],cfg->RES_7F_68[0xB],cfg->RES_7F_68[0xC],cfg->RES_7F_68[0xD],cfg->RES_7F_68[0xE],cfg->RES_7F_68[0xF],
                    cfg->RES_7F_68[0x10],cfg->RES_7F_68[0x11],cfg->RES_7F_68[0x12],cfg->RES_7F_68[0x13],cfg->RES_7F_68[0x14],cfg->RES_7F_68[0x15],cfg->RES_7F_68[0x16],cfg->RES_7F_68[0x17],
                offsetof(SMBUS_INTERFACE, SMBSM), *(PUINT32)&cfg->SMBSM,
                    cfg->SMBSM.RES_17_0,
                    cfg->SMBSM.PGCBDCGDIS,
                    cfg->SMBSM.RES_31_19
    );

    FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);

    return status;
};

FORCEINLINE
PCHAR HIDMInterruptMode(UINT32 Mode)
{
    switch ( Mode )
    {
        case 0: return "Legacy or MSI interrupt";
        case 1: return "SCI";
        case 2: return "SMI";
        default: return "Unknown";
    }
}

REGISTER_LOG_DEVICE_CONFIG(logIMEIConfig);
NTSTATUS logIMEIConfig(
    _In_ PVOID Buffer, 
    _In_ ULONG BufferSize, 
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
)
{
    PAGED_CODE();

    PINTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG cfg = (PINTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG)Buffer;
    
    (LogFile);
    (LogBuffer);
    (LogBufferSize);

    if ( BufferSize < 0x100 )
        return 0;

    NTSTATUS status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                "HFSTS1 [%02zx]: 0x%08x (Host Firmware Status Register 1)\r\n"
                "  FS_HA [31:0]: 0x%x ()\r\n"
                "RES_47_44: %08x\r\n"
                "HFSTS2 [%02zx]: 0x%08x (Host Firmware Status Register 2)\r\n"
                "  GSS1 [31:0]: 0x%x (Host Firmware Status)\r\n"
                "HECI1_H_GS1 [%02zx]: 0x%08x (Host General Status)\r\n"
                "  H_GS1 [31:0]: 0x%x ()\r\n"
                "RES_5F_50: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
                "HFSTS3 [%02zx]: 0x%08x (Host Firmware Status Register 3)\r\n"
                "  GSS2 [31:0]: 0x%x (Host Firmware Status)\r\n"
                "HFSTS4 [%02zx]: 0x%08x (Host Firmware Status Register 4)\r\n"
                "  GSS3 [31:0]: 0x%x (Host Firmware Status)\r\n"
                "HFSTS5 [%02zx]: 0x%08x (Host Firmware Status Register 5)\r\n"
                "  GSS4 [31:0]: 0x%x (Host Firmware Status)\r\n"
                "HFSTS6 [%02zx]: 0x%08x (Host Firmware Status Register 6)\r\n"
                "  GSS5 [31:0]: 0x%x (Host Firmware Status)\r\n"
                "HECI1_H_GS2 [%02zx]: 0x%08x (Host General Status 2)\r\n"
                "  H_GS2 [31:0]: 0x%x (Host General Status 2)\r\n"
                "HECI1_H_GS3 [%02zx]: 0x%08x (Host General Status 3)\r\n"
                "  H_GS3 [31:0]: 0x%x (Host General Status 3)\r\n"
                "RES_97_78: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
                "           %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
                "           %02x %02x %02x %02x %02x %02x %02x %02x\r\n"
                "HECI1_HIDM [%02zx]: 0x%08x (Interrupt Delivery Mode)\r\n"
                "  HIDM [1:0]: 0x%x (%s) (HECI Interrupt Delivery Mode)\r\n"
                "  HIDM_L [1]: 0x%x (HIDM Lock)\r\n"
                "  HIDM_7_3 [7:3]: 0x%x\r\n"
                "  RES_31_8 [31:8]: 0x%x\r\n"
                    ,
                offsetof(INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, HFSTS1), *(PUINT32)&cfg->HFSTS1,
                    cfg->HFSTS1.FS_HA,
                cfg->RES_47_44,
                offsetof(INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, HFSTS2), *(PUINT32)&cfg->HFSTS2,
                    cfg->HFSTS2.GSS1,
                offsetof(INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, HECI1_H_GS1), *(PUINT32)&cfg->HECI1_H_GS1,
                    cfg->HECI1_H_GS1.H_GS1,
                cfg->RES_5F_50[0x0],cfg->RES_5F_50[0x1],cfg->RES_5F_50[0x2],cfg->RES_5F_50[0x3],cfg->RES_5F_50[0x4],cfg->RES_5F_50[0x5],cfg->RES_5F_50[0x6],cfg->RES_5F_50[0x7],cfg->RES_5F_50[0x8],cfg->RES_5F_50[0x9],cfg->RES_5F_50[0xA],cfg->RES_5F_50[0xB],cfg->RES_5F_50[0xC],cfg->RES_5F_50[0xD],cfg->RES_5F_50[0xE],cfg->RES_5F_50[0xF],
                offsetof(INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, HFSTS3), *(PUINT32)&cfg->HFSTS3,
                    cfg->HFSTS3.GSS2,
                offsetof(INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, HFSTS4), *(PUINT32)&cfg->HFSTS4,
                    cfg->HFSTS4.GSS3,
                offsetof(INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, HFSTS5), *(PUINT32)&cfg->HFSTS5,
                    cfg->HFSTS5.GSS4,
                offsetof(INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, HFSTS6), *(PUINT32)&cfg->HFSTS6,
                    cfg->HFSTS6.GSS5,
                offsetof(INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, HECI1_H_GS2), *(PUINT32)&cfg->HECI1_H_GS2,
                    cfg->HECI1_H_GS2.H_GS2,
                offsetof(INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, HECI1_H_GS3), *(PUINT32)&cfg->HECI1_H_GS3,
                    cfg->HECI1_H_GS3.H_GS3,
                cfg->RES_9F_78[0x0],cfg->RES_9F_78[0x1],cfg->RES_9F_78[0x2],cfg->RES_9F_78[0x3],cfg->RES_9F_78[0x4],cfg->RES_9F_78[0x5],cfg->RES_9F_78[0x6],cfg->RES_9F_78[0x7],cfg->RES_9F_78[0x8],cfg->RES_9F_78[0x9],cfg->RES_9F_78[0xA],cfg->RES_9F_78[0xB],cfg->RES_9F_78[0xC],cfg->RES_9F_78[0xD],cfg->RES_9F_78[0xE],cfg->RES_9F_78[0xF],
                cfg->RES_9F_78[0x10],cfg->RES_9F_78[0x11],cfg->RES_9F_78[0x12],cfg->RES_9F_78[0x13],cfg->RES_9F_78[0x14],cfg->RES_9F_78[0x15],cfg->RES_9F_78[0x16],cfg->RES_9F_78[0x17],cfg->RES_9F_78[0x18],cfg->RES_9F_78[0x19],cfg->RES_9F_78[0x1A],cfg->RES_9F_78[0x1B],cfg->RES_9F_78[0x1C],cfg->RES_9F_78[0x1D],cfg->RES_9F_78[0x1E],cfg->RES_9F_78[0x1F],
                cfg->RES_9F_78[0x20],cfg->RES_9F_78[0x21],cfg->RES_9F_78[0x22],cfg->RES_9F_78[0x23],cfg->RES_9F_78[0x24],cfg->RES_9F_78[0x25],cfg->RES_9F_78[0x26],cfg->RES_9F_78[0x27],
                offsetof(INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, HECI1_HIDM), *(PUINT32)&cfg->HECI1_HIDM,
                    cfg->HECI1_HIDM.HIDM, HIDMInterruptMode(cfg->HECI1_HIDM.HIDM),
                    cfg->HECI1_HIDM.HIDM_L,
                    cfg->HECI1_HIDM.RES_7_3,
                    cfg->HECI1_HIDM.RES_31_8
    );

    FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);

    return status;
};

REGISTER_LOG_DEVICE_CONFIG(logLpcConfig);
NTSTATUS logLpcConfig(
    _In_ PVOID Buffer, 
    _In_ ULONG BufferSize, 
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
)
{
    PAGED_CODE();

    PLPC_CFG cfg = (PLPC_CFG)Buffer;
    
    (LogFile);
    (LogBuffer);
    (LogBufferSize);

    if ( BufferSize < 0x100 )
        return 0;

    NTSTATUS status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
        "Reserved1 [%02zx]: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x\r\n"
        "SCNT [%02zx]: 0x%02x (Serial IRQ Control)\r\n"
        "  SFPW [1:0]: 0x%x (%u clocks) (Start Frame Pulse Width)\r\n"
        "  FS [5:2]: 0x%x (Frame Size)\r\n"
        "  MD [6]: 0x%x (Mode)\r\n"
        "  EN [7]: 0x%x (Enable)\r\n"
        "Reserved2 [%02zx]: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x\r\n"
        "IOD [%02zx]: 0x%04x (I/O Decode Ranges)\r\n"
        "  ComA [2:0]: 0x%x (%s)\r\n"
        "  Reserved1 [3]: 0x%x\r\n"
        "  ComB [6:4]: 0x%x (%s)\r\n"
        "  Reserved2 [7]: 0x%x\r\n"
        "  LPT [9:8]: 0x%x (%s)\r\n"
        "  Reserved3 [11:10]: 0x%x\r\n"
        "  FDD [12]: 0x%x (%s)\r\n"
        "  Reserved4 [15:13]: 0x%x\r\n"
        "IOE [%02zx]: 0x%04x\r\n"
        "  CAE [0]: 0x%x (Com Port A Enable: Enables decoding of the COMA range to LPC.)\r\n"
        "  CBE [1]: 0x%x (Com Port B Enable: Enables decoding of the COMB range to LPC.)\r\n"
        "  PPE [2]: 0x%x (Parallel Port Enable: Enables decoding of the LPT range to LPC.)\r\n"
        "  DDE [3]: 0x%x (Floppy Drive Enable: Enables decoding of the FDD range to LPC)\r\n"
        "  Reserved1 [7:4]: 0x%x\r\n"
        "  LGE [8]: 0x%x (Low Gameport Enable: Enables decoding of the I/O locations 200h to 207h to LPC.)\r\n"
        "  HGE [9]: 0x%x (High Gameport Enable: Enables decoding of the I/O locations 208h to 20Fh to LPC.)\r\n"
        "  KE [10]: 0x%x (Keyboard Enable: Enables decoding of the keyboard I/O locations 60h, 64h to LPC.)\r\n"
        "  ME1 [11]: 0x%x (Microcontroller Enable #1: Enables decoding of I/O locations 62h, 66h to LPC.)\r\n"
        "  SE [12]: 0x%x (SuperI/O Enable decoding of I/O locations 2Eh,2Fh to LPC)\r\n"
        "  ME2 [13]: 0x%x (Microcontroller Enable #2 decoding of I/O locations 4Eh, 4Fh to LPC)\r\n"
        "  Reserved2 [15:14]: 0x%x\r\n"
            ,
        offsetof(LPC_CFG, Reserved1), 
        cfg->Reserved1[0x00],cfg->Reserved1[0x01],cfg->Reserved1[0x02],cfg->Reserved1[0x03],cfg->Reserved1[0x04],cfg->Reserved1[0x05],cfg->Reserved1[0x06],cfg->Reserved1[0x07],cfg->Reserved1[0x08],cfg->Reserved1[0x09],cfg->Reserved1[0x0a],cfg->Reserved1[0x0b],cfg->Reserved1[0x0c],cfg->Reserved1[0x0d],cfg->Reserved1[0x0e],cfg->Reserved1[0x0f],
        cfg->Reserved1[0x10],cfg->Reserved1[0x11],cfg->Reserved1[0x12],cfg->Reserved1[0x13],cfg->Reserved1[0x14],cfg->Reserved1[0x15],cfg->Reserved1[0x16],cfg->Reserved1[0x17],cfg->Reserved1[0x18],cfg->Reserved1[0x19],cfg->Reserved1[0x1a],cfg->Reserved1[0x1b],cfg->Reserved1[0x1c],cfg->Reserved1[0x1d],cfg->Reserved1[0x1e],cfg->Reserved1[0x1f],
        cfg->Reserved1[0x20],cfg->Reserved1[0x21],cfg->Reserved1[0x22],cfg->Reserved1[0x23],
        offsetof(LPC_CFG, SCNT), *(PUINT32)&cfg->SCNT,
            cfg->SCNT.SFPW, getSFPWclocks(cfg->SCNT.SFPW),
            cfg->SCNT.FS,
            cfg->SCNT.MD,
            cfg->SCNT.EN,
        offsetof(LPC_CFG, Reserved2), 
        cfg->Reserved2[0x0],cfg->Reserved2[0x1],cfg->Reserved2[0x2],cfg->Reserved2[0x3],cfg->Reserved2[0x4],cfg->Reserved2[0x5],cfg->Reserved2[0x6],cfg->Reserved2[0x7],cfg->Reserved2[0x8],cfg->Reserved2[0x9],cfg->Reserved2[0xa],cfg->Reserved2[0xb],cfg->Reserved2[0xc],cfg->Reserved2[0xd],cfg->Reserved2[0xe],cfg->Reserved2[0xf],
        cfg->Reserved2[0x10],cfg->Reserved2[0x11],cfg->Reserved2[0x12],cfg->Reserved2[0x13],cfg->Reserved2[0x14],cfg->Reserved2[0x15],cfg->Reserved2[0x16],cfg->Reserved2[0x17],cfg->Reserved2[0x18],cfg->Reserved2[0x19],cfg->Reserved2[0x1a],
        offsetof(LPC_CFG, IOD), *(PUINT32)&cfg->IOD,
            cfg->IOD.CA, BIOS_LPC_IOD_CX_STR(cfg->IOD.CA),
            cfg->IOD.Reserved1,
            cfg->IOD.CB, BIOS_LPC_IOD_CX_STR(cfg->IOD.CB),
            cfg->IOD.Reserved2,
            cfg->IOD.LPT, BIOS_LPC_IOD_LPT_STR(cfg->IOD.LPT),
            cfg->IOD.Reserved3,
            cfg->IOD.FDD, BIOS_LPC_IOD_FDD_STR(cfg->IOD.FDD),
            cfg->IOD.Reserved4,
        offsetof(LPC_CFG, IOE), *(PUINT32)&cfg->IOE,
            cfg->IOE.CAE,
            cfg->IOE.CBE,
            cfg->IOE.PPE,
            cfg->IOE.DDE,
            cfg->IOE.Reserved1,
            cfg->IOE.LGE,
            cfg->IOE.HGE,
            cfg->IOE.KE,
            cfg->IOE.ME1,
            cfg->IOE.SE,
            cfg->IOE.ME2,
            cfg->IOE.Reserved2);

    FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);

    CHAR lgir1Mask[BIN_8_STR_BUFFER_SIZE] = { 0 };
    CHAR lgir2Mask[BIN_8_STR_BUFFER_SIZE] = { 0 };
    CHAR lgir3Mask[BIN_8_STR_BUFFER_SIZE] = { 0 };
    CHAR lgir4Mask[BIN_8_STR_BUFFER_SIZE] = { 0 };

    uint8ToBin((UINT8)cfg->LGIR1.ADDRESS_MASK, lgir1Mask);
    uint8ToBin((UINT8)cfg->LGIR2.ADDRESS_MASK, lgir2Mask);
    uint8ToBin((UINT8)cfg->LGIR3.ADDRESS_MASK, lgir3Mask);
    uint8ToBin((UINT8)cfg->LGIR4.ADDRESS_MASK, lgir4Mask);

    status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
        "LGIR1 [%02zx]: 0x%x (LPC Generic IO Range 1)\r\n"
        "  LPC_DECODE_ENABLE [0]: 0x%x\r\n"
        "  Reserved1 [1]: 0x%x\r\n"
        "  ADDRESS [15:2]: 0x%x (0x%08x)\r\n"
        "  Reserved2 [17:16]: 0x%x\r\n"
        "  ADDRESS_MASK [23:18]: 0x%x (%s)\r\n"
        "  Reserved3 [31:24]: 0x%x\r\n"
        "LGIR2 [%02zx]: 0x%x (LPC Generic IO Range 2)\r\n"
        "  LPC_DECODE_ENABLE [0]: 0x%x\r\n"
        "  Reserved1 [1]: 0x%x\r\n"
        "  ADDRESS [15:2]: 0x%x (0x%08x)\r\n"
        "  Reserved2 [17:16]: 0x%x\r\n"
        "  ADDRESS_MASK [23:18]: 0x%x (%s)\r\n"
        "  Reserved3 [31:24]: 0x%x\r\n"
        "LGIR3 [%02zx]: 0x%x (LPC Generic IO Range 3)\r\n"
        "  LPC_DECODE_ENABLE [0]: 0x%x\r\n"
        "  Reserved1 [1]: 0x%x\r\n"
        "  ADDRESS [15:2]: 0x%x (0x%08x)\r\n"
        "  Reserved2 [17:16]: 0x%x\r\n"
        "  ADDRESS_MASK [23:18]: 0x%x (%s)\r\n"
        "  Reserved3 [31:24]: 0x%x\r\n"
        "LGIR4 [%02zx]: 0x%x (LPC Generic IO Range 4)\r\n"
        "  LPC_DECODE_ENABLE [0]: 0x%x\r\n"
        "  Reserved1 [1]: 0x%x\r\n"
        "  ADDRESS [15:2]: 0x%x (0x%08x)\r\n"
        "  Reserved2 [17:16]: 0x%x\r\n"
        "  ADDRESS_MASK [23:18]: 0x%x (%s)\r\n"
        "  Reserved3 [31:24]: 0x%x\r\n",
        offsetof(LPC_CFG, LGIR1), *(PUINT32)&cfg->LGIR1,
            cfg->LGIR1.LPC_DECODE_ENABLE,
            cfg->LGIR1.Reserved1,
            cfg->LGIR1.ADDRESS, (cfg->LGIR1.ADDRESS<<2),
            cfg->LGIR1.Reserved2,
            cfg->LGIR1.ADDRESS_MASK, lgir1Mask,
            cfg->LGIR1.Reserved3,
        offsetof(LPC_CFG, LGIR2), *(PUINT32)&cfg->LGIR2,
            cfg->LGIR2.LPC_DECODE_ENABLE,
            cfg->LGIR2.Reserved1,
            cfg->LGIR2.ADDRESS, (cfg->LGIR2.ADDRESS<<2),
            cfg->LGIR2.Reserved2,
            cfg->LGIR2.ADDRESS_MASK, lgir2Mask,
            cfg->LGIR2.Reserved3,
        offsetof(LPC_CFG, LGIR3), *(PUINT32)&cfg->LGIR3,
            cfg->LGIR3.LPC_DECODE_ENABLE,
            cfg->LGIR3.Reserved1,
            cfg->LGIR3.ADDRESS, (cfg->LGIR3.ADDRESS<<2),
            cfg->LGIR3.Reserved2,
            cfg->LGIR3.ADDRESS_MASK, lgir3Mask,
            cfg->LGIR3.Reserved3,
        offsetof(LPC_CFG, LGIR4), *(PUINT32)&cfg->LGIR4,
            cfg->LGIR4.LPC_DECODE_ENABLE,
            cfg->LGIR4.Reserved1,
            cfg->LGIR4.ADDRESS, (cfg->LGIR4.ADDRESS<<2),
            cfg->LGIR4.Reserved2,
            cfg->LGIR4.ADDRESS_MASK, lgir4Mask,
            cfg->LGIR4.Reserved3);
    
    FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);

    status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
        "ULKMC [%02zx]: 0x%x (USB Legacy Keyboard/Mouse Control)\r\n"
        "  S60REN [0]: 0x%x (SMI on Port 60 Reads Enable)\r\n"
        "  S60WEN [1]: 0x%x (SMI on Port 60 Writes Enable)\r\n"
        "  S64REN [2]: 0x%x (SMI on Port 64 Reads Enable)\r\n"
        "  S64WEN [3]: 0x%x (SMI on Port 64 Writes Enable)\r\n"
        "  Reserved1 [4]: 0x%x ()\r\n"
        "  A20PASSEN [5]: 0x%x (A20Gate Pass-Through Enable)\r\n"
        "  PSTATE [6]: 0x%x (Pass Through State)\r\n"
        "  SMIATENDPS [7]: 0x%x (SMI at End of Pass-through Enable)\r\n"
        "  TRAPBY60R [8]: 0x%x (SMI Caused by Port 60 Read)\r\n"
        "  TRAPBY60W [9]: 0x%x (SMI Caused by Port 60 Write)\r\n"
        "  TRAPBY64R [10]: 0x%x (SMI Caused by Port 64 Read)\r\n"
        "  TRAPBY64W [11]: 0x%x (SMI Caused by Port 64 Write)\r\n"
        "  Reserved2 [14:12]: 0x%x\r\n"
        "  SMIBYENDPS [15]: 0x%x (SMI Caused by End of Pass-through)\r\n"
        "  Reserved3 [31:16]: 0x%x\r\n"
        "LGMR [%02zx]: 0x%x (64KB memory block that will be decoded to LPC as standard LPC Memory Cycle if enabled)\r\n"
        "  LMRD_EN [0]: 0x%x\r\n"
        "  Reserved1 [15:1]: 0x%x\r\n"
        "  MA [31:16]: 0x%x (0x%08x)\r\n"
        "Reserved3 [%02zx]: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x\r\n"
        "FWH ID Select 1 [%02zx]: 0x%08x (IDSEL fields the LPC Bridge uses for memory cycles going to the FWH.)\r\n"
        "  IC0 [3:0]: 0x%x (C0-C7 IDSEL (IC0): IDSEL to use in FWH cycle for range enabled by BDE.EC0.)\r\n"
        "  IC8 [7:4]: 0x%x (C8-CF IDSEL (IC8): IDSEL to use in FWH cycle for range enabled by BDE.EC8)\r\n"
        "  ID0 [11:8]: 0x%x (D0-D7 IDSEL (ID0): IDSEL to use in FWH cycle for range enabled by BDE.ED0)\r\n"
        "  ID8 [15:12]: 0x%x (D8-DF IDSEL (ID8): IDSEL to use in FWH cycle for range enabled by BDE.ED8.)\r\n"
        "  IE0 [19:16]: 0x%x (E0-E7 IDSEL (IE0): IDSEL to use in FWH cycle for range enabled by BDE.EE0)\r\n"
        "  IE8 [23:20]: 0x%x (E8-EF IDSEL (IE8): IDSEL to use in FWH cycle for range enabled by BDE.EE8)\r\n"
        "  IF0 [27:24]: 0x%x (F0-F7 IDSEL (IF0): IDSEL to use in FWH cycle for range enabled by BDE.EF0)\r\n"
        "  IF8 [31:28]: 0x%x (F8-FF IDSEL (IF8): IDSEL to use in FWH cycle for range enabled by BDE.EF8)\r\n"
        "FWH ID Select 2 [%02zx]: 0x%04x (additional IDSEL fields the LPC Bridge uses for memory cycles going to the FWH)\r\n"
        "  I40 [3:0]: 0x%x (40-4F IDSEL (I40): IDSEL to use in FWH cycle for range enabled by BDE.E40)\r\n"
        "  I50 [7:4]: 0x%x (50-5F IDSEL (I50): IDSEL to use in FWH cycle for range enabled by BDE.E50)\r\n"
        "  I60 [11:8]: 0x%x (60-6F IDSEL (I60): IDSEL to use in FWH cycle for range enabled by BDE.E60)\r\n"
        "  I70 [15:12]: 0x%x (70-7F IDSEL (I70): IDSEL to use in FWH cycle for range enabled by BDE.E70)\r\n"
        "Reserved4 [%02zx]: 0x%x\r\n",
        offsetof(LPC_CFG, ULKMC), *(PUINT32)&cfg->ULKMC,
            cfg->ULKMC.S60REN,
            cfg->ULKMC.S60WEN,
            cfg->ULKMC.S64REN,
            cfg->ULKMC.S64WEN,
            cfg->ULKMC.Reserved1,
            cfg->ULKMC.A20PASSEN,
            cfg->ULKMC.PSTATE,
            cfg->ULKMC.SMIATENDPS,
            cfg->ULKMC.TRAPBY60R,
            cfg->ULKMC.TRAPBY60W,
            cfg->ULKMC.TRAPBY64R,
            cfg->ULKMC.TRAPBY64W,
            cfg->ULKMC.Reserved2,
            cfg->ULKMC.SMIBYENDPS,
            cfg->ULKMC.Reserved3,
        offsetof(LPC_CFG, LGMR), *(PUINT32)&cfg->LGMR,
            cfg->LGMR.LMRD_EN,
            cfg->LGMR.Reserved1,
            cfg->LGMR.MA, (cfg->LGMR.MA<<16),
        offsetof(LPC_CFG, Reserved3), 
        cfg->Reserved3[0x00],cfg->Reserved3[0x01],cfg->Reserved3[0x02],cfg->Reserved3[0x03],cfg->Reserved3[0x04],cfg->Reserved3[0x05],cfg->Reserved3[0x06],cfg->Reserved3[0x07],cfg->Reserved3[0x08],cfg->Reserved3[0x09],cfg->Reserved3[0x0a],cfg->Reserved3[0x0b],cfg->Reserved3[0x0c],cfg->Reserved3[0x0d],cfg->Reserved3[0x0e],cfg->Reserved3[0x0f],
        cfg->Reserved3[0x10],cfg->Reserved3[0x11],cfg->Reserved3[0x12],cfg->Reserved3[0x13],cfg->Reserved3[0x14],cfg->Reserved3[0x15],cfg->Reserved3[0x16],cfg->Reserved3[0x17],cfg->Reserved3[0x18],cfg->Reserved3[0x19],cfg->Reserved3[0x1a],cfg->Reserved3[0x1b],cfg->Reserved3[0x1c],cfg->Reserved3[0x1d],cfg->Reserved3[0x1e],cfg->Reserved3[0x1f],
        cfg->Reserved3[0x20],cfg->Reserved3[0x21],cfg->Reserved3[0x22],cfg->Reserved3[0x23],cfg->Reserved3[0x24],cfg->Reserved3[0x25],cfg->Reserved3[0x26],cfg->Reserved3[0x27],cfg->Reserved3[0x28],cfg->Reserved3[0x29],cfg->Reserved3[0x2a],cfg->Reserved3[0x2b],cfg->Reserved3[0x2c],cfg->Reserved3[0x2d],cfg->Reserved3[0x2e],cfg->Reserved3[0x2f],
        cfg->Reserved3[0x30],cfg->Reserved3[0x31],cfg->Reserved3[0x32],cfg->Reserved3[0x33],
        offsetof(LPC_CFG, FS1), *(PUINT32)&cfg->FS1,
            cfg->FS1.IC0,
            cfg->FS1.IC8,
            cfg->FS1.ID0,
            cfg->FS1.ID8,
            cfg->FS1.IE0,
            cfg->FS1.IE8,
            cfg->FS1.IF0,
            cfg->FS1.IF8,
        offsetof(LPC_CFG, FS2), *(PUINT32)&cfg->FS2,
            cfg->FS2.I40,
            cfg->FS2.I50,
            cfg->FS2.I60,
            cfg->FS2.I70,
        offsetof(LPC_CFG, Reserved4), cfg->Reserved4);
    
    FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);

    status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
        "BDE [%02zx]: 0x%04x (BIOS Decode Enable)\r\n"
        "  E40 [0]: 0x%x (40-4F: 1MB, Data space: FF400000h - FF4FFFFFh, Feature space: FF000000h - FF0FFFFFh)\r\n"
        "  E50 [1]: 0x%x (50-5F: 1MB, Data space: FF500000h - FF5FFFFFh, Feature space: FF100000h - FF1FFFFFh)\r\n"
        "  E60 [2]: 0x%x (60-6F: 1MB, Data space: FF600000h - FF6FFFFFh, Feature Space: FF200000h - FF2FFFFFh)\r\n"
        "  E70 [3]: 0x%x (70-7F: 1MB, Data space: FF700000h - FF7FFFFFh, Feature space: FF300000h - FF3FFFFFh)\r\n"
        "  Reserved1 [5:4]: 0x%x\r\n"
        "  LEE [6]: 0x%x (Legacy E Segment Enable: legacy 64KB range at E0000h - EFFFFh)\r\n"
        "  LFE [7]: 0x%x (Legacy F Segment Enable: legacy 64KB range at F0000h - FFFFFh)\r\n"
        "  EC0 [8]: 0x%x (C0-C7: 512K, Data space: FFC00000h - FFC7FFFFh, Feature space: FF800000h - FF87FFFFh)\r\n"
        "  EC8 [9]: 0x%x (C8-CF: 512K, Data space: FFC80000h - FFCFFFFFh, Feature space: FF880000h - FF8FFFFFh)\r\n"
        "  ED0 [10]: 0x%x (D0-D7: 512K, Data space: FFD00000h - FFD7FFFFh, Feature space: FF900000h - FF97FFFFh)\r\n"
        "  ED8 [11]: 0x%x (D8-DF: Enables decoding of 512K, Data space: FFD80000h - FFDFFFFFh, Feature space: FF980000h - FF9FFFFFh)\r\n"
        "  EE0 [12]: 0x%x (E0-E8: 512K, Data space: FFE00000h - FFE7FFFFh, Feature Space: FFA00000h - FFA7FFFFh)\r\n"
        "  EE8 [13]: 0x%x (E8-EF: 512K, Data space: FFE80000h - FFEFFFFFh, Feature space: FFA80000h - FFAFFFFFh)\r\n"
        "  EF0 [14]: 0x%x (F0-F8: 512K, Data space: FFF00000h - FFF7FFFFh, Feature space: FFB00000h - FFB7FFFFh)\r\n"
        "  EF8 [15]: 0x%x (F8-FF: 512K, Data space: FFF80000h - FFFFFFFFh, Feature space: FFB80000h - FFBFFFFFh)\r\n"
        "  Reserved2 [31:16]: 0x%x\r\n"
        "BC [%02zx]: 0x%02x (BIOS Control)\r\n"
        "  WPD [0]: 0x%x (Write Protect Disable)\r\n"
        "  LE [1]: 0x%x (Lock Enable: setting the WP bit will cause SMI)\r\n"
        "  Reserved1 [3:2]: 0x%x\r\n"
        "  TS [4]: 0x%x (Top Swap)\r\n"
        "  EISS [5]: 0x%x (Enable InSMM.STS, the BIOS region is not writable until SMM sets the InSMM.STS bit. Today BIOS Flash is writable if WPD is a 1.)\r\n"
        "  BBS [6]: 0x%x (%s) (Boot BIOS Destination)\r\n"
        "  BILD [7]: 0x%x (BIOS Interface Lock-Down, prevents BC.TS and BC.BBS from being changed)\r\n"
        "Reserved5 [%02zx]: %02x %02x %02x\r\n"
        "PCCTL [%02zx]: 0x%04x (PCI Clock Control )\r\n"
        "  CLKRUN_EN [0]: 0x%x (Clock Run Enable)\r\n"
        "  Reserved1 [1]: 0x%x ()\r\n"
        "  PCLKVLD_CFG [3:2]: 0x%x (%s) (LPC Clock Valid Configuration)\r\n"
        "  STP_PCI_OVR [4]: 0x%x (Stop PCI# Override)\r\n"
        "  STP_PCI_VAL [5]: 0x%x (Stop PCI# Value)\r\n"
        "  CLKRUN_VAL [6]: 0x%x (CLKRUN# Pin Output Value)\r\n"
        "  CLKRUN_EN_VAL [7]: 0x%x (CLKRUN# Buffer Enable Value)\r\n"
        "  CLKRUN_OVR [8]: 0x%x (CLKRUN# Override)\r\n"
        "  CLKRUN_EN_OVR [9]: 0x%x (CLKRUN# Buffer Enable Override)\r\n"
        "  Reserved2 [31:10]: 0x%x\r\n"
        "Reserved6 [%02zx]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "RCBA [%02zx]: 0x%08x\r\n"
        "  Enable [0]: 0x%x\r\n"
        "  Reserved1 [13:1]: 0x%x\r\n"
        "  BA [31:14]: 0x%x (0x%x)\r\n",
        offsetof(LPC_CFG, BDE), *(PUINT32)&cfg->BDE,
            cfg->BDE.E40,
            cfg->BDE.E50,
            cfg->BDE.E60,
            cfg->BDE.E70,
            cfg->BDE.Reserved1,
            cfg->BDE.LEE,
            cfg->BDE.LFE,
            cfg->BDE.EC0,
            cfg->BDE.EC8,
            cfg->BDE.ED0,
            cfg->BDE.ED8,
            cfg->BDE.EE0,
            cfg->BDE.EE8,
            cfg->BDE.EF0,
            cfg->BDE.EF8,
            cfg->BDE.Reserved2,
        offsetof(LPC_CFG, BC), *(PUINT32)&cfg->BC,
            cfg->BC.WPD,
            cfg->BC.LE,
            cfg->BC.Reserved1,
            cfg->BC.TS,
            cfg->BC.EISS,
            cfg->BC.BBS, BIOS_SPI_BBS_STR(cfg->BC.BBS),
            cfg->BC.BILD,
        offsetof(LPC_CFG, Reserved5), 
        cfg->Reserved5[0],cfg->Reserved5[1],cfg->Reserved5[2],
        offsetof(LPC_CFG, PCCTL), *(PUINT32)&cfg->PCCTL,
            cfg->PCCTL.CLKRUN_EN,
            cfg->PCCTL.Reserved1,
            cfg->PCCTL.PCLKVLD_CFG, BIOS_LPC_PCCTL_PCLKVLD_CFG_STR(cfg->PCCTL.PCLKVLD_CFG),
            cfg->PCCTL.STP_PCI_OVR,
            cfg->PCCTL.STP_PCI_VAL,
            cfg->PCCTL.CLKRUN_VAL,
            cfg->PCCTL.CLKRUN_EN_VAL,
            cfg->PCCTL.CLKRUN_OVR,
            cfg->PCCTL.CLKRUN_EN_OVR,
            cfg->PCCTL.Reserved2,
        offsetof(LPC_CFG, Reserved6), cfg->Reserved6[0],cfg->Reserved6[0x1],cfg->Reserved6[0x2],cfg->Reserved6[0x3],cfg->Reserved6[0x4],cfg->Reserved6[0x5],cfg->Reserved6[0x6],cfg->Reserved6[0x7],cfg->Reserved6[0x8],cfg->Reserved6[0x9],cfg->Reserved6[0xa],cfg->Reserved6[0xb],
        offsetof(LPC_CFG, RCBA), *(PUINT32)&cfg->RCBA,
            cfg->RCBA.Enable,
            cfg->RCBA.Reserved1,
            cfg->RCBA.BA, (cfg->RCBA.BA<<0xe));

    return status;
};



#define BIOS_SPI_SRC_STR(__src__) \
    (__src__ == 0)?"No prefetching, but caching enabled" \
    : (__src__ == 0b01)?"No prefetching and no caching" \
    : (__src__ == 0b10)?"Prefetching and Caching enabled" \
    : "Illegal"

FORCEINLINE
NTSTATUS logSpiConfig(
    _In_ PVOID Buffer, 
    _In_ ULONG BufferSize, 
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
)
{
    PSPI_CFG cfg = (PSPI_CFG)Buffer;
    
    (LogFile);
    (LogBuffer);
    (LogBufferSize);

    if ( BufferSize < 0x100 )
        return 0;

    NTSTATUS status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                    "Unsupported Request Status (UR_STS_CTL) [%02zx]: 0x%x\r\n" 
                    "  URRE: 0x%x\r\n" 
                    "  URD: 0x%x\r\n" 
                    "  Reserved1: 0x%x\r\n" 
                    "BIOS Decode Enable (BDE) [%02zx]: 0x%x\r\n" 
                    "  E40: 0x%x\r\n" 
                    "  E50: 0x%x\r\n" 
                    "  E60: 0x%x\r\n" 
                    "  E70: 0x%x\r\n" 
                    "  Reserved1: 0x%x\r\n" 
                    "  LEE: 0x%x\r\n" 
                    "  LFE: 0x%x\r\n" 
                    "  EC0: 0x%x\r\n" 
                    "  EC8: 0x%x\r\n" 
                    "  ED0: 0x%x\r\n" 
                    "  ED8: 0x%x\r\n" 
                    "  EE0: 0x%x\r\n" 
                    "  EE8: 0x%x\r\n" 
                    "  EF0: 0x%x\r\n" 
                    "  EF8: 0x%x\r\n" 
                    "  Reserved2: 0x%x\r\n" 
                    "BIOS Control (BIOS_SPI_BC) [%02zx]: 0x%x\r\n" 
                    "  WPD: 0x%x\r\n"
                    "  LE: 0x%x\r\n"
                    "  SRC: 0x%x (%s)\r\n"
                    "  TSS: 0x%x\r\n"
                    "  EISS: 0x%x\r\n"
                    "  BBS: 0x%x (%s)\r\n"
                    "  BILD: 0x%x\r\n"
                    "  SPI_SYNC_SS: 0x%x\r\n"
                    "  Reserved1: 0x%x\r\n"
                    "  SPI_ASYNC_SS: 0x%x\r\n"
                    "  ASE_BWP: 0x%x\r\n"
                    "  Reserved2: 0x%x\r\n",
                    offsetof(SPI_CFG, UR_STS_CTL), *(PUINT32)&cfg->UR_STS_CTL, 
                        cfg->UR_STS_CTL.URRE, 
                        cfg->UR_STS_CTL.URD, 
                        cfg->UR_STS_CTL.Reserved1, 
                    offsetof(SPI_CFG, BDE), *(PUINT32)&cfg->BDE, 
                        cfg->BDE.E40, 
                        cfg->BDE.E50, 
                        cfg->BDE.E60, 
                        cfg->BDE.E70, 
                        cfg->BDE.Reserved1, 
                        cfg->BDE.LEE, 
                        cfg->BDE.LFE, 
                        cfg->BDE.EC0, 
                        cfg->BDE.EC8, 
                        cfg->BDE.ED0, 
                        cfg->BDE.ED8, 
                        cfg->BDE.EE0, 
                        cfg->BDE.EE8, 
                        cfg->BDE.EF0, 
                        cfg->BDE.EF8, 
                        cfg->BDE.Reserved2, 
                    offsetof(SPI_CFG, BC), *(PUINT32)&cfg->BC, 
                        cfg->BC.WPD, 
                        cfg->BC.LE, 
                        cfg->BC.SRC, BIOS_SPI_SRC_STR(cfg->BC.SRC),
                        cfg->BC.TSS, 
                        cfg->BC.EISS, 
                        cfg->BC.BBS, BIOS_SPI_BBS_STR(cfg->BC.BBS),
                        cfg->BC.BILD, 
                        cfg->BC.SPI_SYNC_SS, 
                        cfg->BC.Reserved1, 
                        cfg->BC.SPI_ASYNC_SS, 
                        cfg->BC.ASE_BWP, 
                        cfg->BC.Reserved2);

    return status;
};



REGISTER_LOG_DEVICE_CONFIG(logGBeConfig);
NTSTATUS logGBeConfig(
    _In_ PVOID Buffer, 
    _In_ ULONG BufferSize, 
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
)
{
    PAGED_CODE();

    PINTEGRATED_GBE cfg = (PINTEGRATED_GBE)Buffer;
    
    (LogFile);
    (LogBuffer);
    (LogBufferSize);

    if ( BufferSize < 0x100 )
        return 0;

    NTSTATUS status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
        "RES_9F_40 [%02zx]: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "                %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x\r\n"
        "LANDISCTRL [%02zx]: %08x\r\n"
        "  LD [0]: %x\r\n"
        "  RES_31_1 [31:1]: %x\r\n"
        "LOCKLANDIS [%02zx]: %08x\r\n"
        "  LLD [0]: %x\r\n"
        "  RES_31_1 [31:1]: %x\r\n"
        "LTRCAP [%02zx]: %08x\r\n"
        "  MSL [9:0]: %x\r\n"
        "  MSLS [12:10]: %x\r\n"
        "  RES_15_13 [15:13]: %x\r\n"
        "  MNSL [25:16]: %x\r\n"
        "  MNSLS [28:26]: %x\r\n"
        "  RES_31_29 [31:29]: %x\r\n"
            ,
        offsetof(INTEGRATED_GBE, RES_9F_40), 
        cfg->RES_9F_40[0x00],cfg->RES_9F_40[0x01],cfg->RES_9F_40[0x02],cfg->RES_9F_40[0x03],cfg->RES_9F_40[0x04],cfg->RES_9F_40[0x05],cfg->RES_9F_40[0x06],cfg->RES_9F_40[0x07],cfg->RES_9F_40[0x08],cfg->RES_9F_40[0x09],cfg->RES_9F_40[0x0a],cfg->RES_9F_40[0x0b],cfg->RES_9F_40[0x0c],cfg->RES_9F_40[0x0d],cfg->RES_9F_40[0x0e],cfg->RES_9F_40[0x0f],
        cfg->RES_9F_40[0x10],cfg->RES_9F_40[0x11],cfg->RES_9F_40[0x12],cfg->RES_9F_40[0x13],cfg->RES_9F_40[0x14],cfg->RES_9F_40[0x15],cfg->RES_9F_40[0x16],cfg->RES_9F_40[0x17],cfg->RES_9F_40[0x18],cfg->RES_9F_40[0x19],cfg->RES_9F_40[0x1a],cfg->RES_9F_40[0x1b],cfg->RES_9F_40[0x1c],cfg->RES_9F_40[0x1d],cfg->RES_9F_40[0x1e],cfg->RES_9F_40[0x1f],
        cfg->RES_9F_40[0x20],cfg->RES_9F_40[0x21],cfg->RES_9F_40[0x22],cfg->RES_9F_40[0x23],cfg->RES_9F_40[0x24],cfg->RES_9F_40[0x25],cfg->RES_9F_40[0x26],cfg->RES_9F_40[0x27],cfg->RES_9F_40[0x28],cfg->RES_9F_40[0x29],cfg->RES_9F_40[0x2a],cfg->RES_9F_40[0x2b],cfg->RES_9F_40[0x2c],cfg->RES_9F_40[0x2d],cfg->RES_9F_40[0x2e],cfg->RES_9F_40[0x2f],
        cfg->RES_9F_40[0x30],cfg->RES_9F_40[0x31],cfg->RES_9F_40[0x32],cfg->RES_9F_40[0x33],cfg->RES_9F_40[0x34],cfg->RES_9F_40[0x35],cfg->RES_9F_40[0x36],cfg->RES_9F_40[0x37],cfg->RES_9F_40[0x38],cfg->RES_9F_40[0x39],cfg->RES_9F_40[0x3a],cfg->RES_9F_40[0x3b],cfg->RES_9F_40[0x3c],cfg->RES_9F_40[0x3d],cfg->RES_9F_40[0x3e],cfg->RES_9F_40[0x3f],
        cfg->RES_9F_40[0x40],cfg->RES_9F_40[0x41],cfg->RES_9F_40[0x42],cfg->RES_9F_40[0x43],cfg->RES_9F_40[0x44],cfg->RES_9F_40[0x45],cfg->RES_9F_40[0x46],cfg->RES_9F_40[0x47],cfg->RES_9F_40[0x48],cfg->RES_9F_40[0x49],cfg->RES_9F_40[0x4a],cfg->RES_9F_40[0x4b],cfg->RES_9F_40[0x4c],cfg->RES_9F_40[0x4d],cfg->RES_9F_40[0x4e],cfg->RES_9F_40[0x4f],
        cfg->RES_9F_40[0x50],cfg->RES_9F_40[0x51],cfg->RES_9F_40[0x52],cfg->RES_9F_40[0x53],cfg->RES_9F_40[0x54],cfg->RES_9F_40[0x55],cfg->RES_9F_40[0x56],cfg->RES_9F_40[0x57],cfg->RES_9F_40[0x58],cfg->RES_9F_40[0x59],cfg->RES_9F_40[0x5a],cfg->RES_9F_40[0x5b],cfg->RES_9F_40[0x5c],cfg->RES_9F_40[0x5d],cfg->RES_9F_40[0x5e],cfg->RES_9F_40[0x5f],
        offsetof(INTEGRATED_GBE, LANDISCTRL), *(PUINT32)&cfg->LANDISCTRL,
            cfg->LANDISCTRL.LD,
            cfg->LANDISCTRL.RES_31_1,
        offsetof(INTEGRATED_GBE, LOCKLANDIS), *(PUINT32)&cfg->LOCKLANDIS,
            cfg->LOCKLANDIS.LLD,
            cfg->LOCKLANDIS.RES_31_1,
        offsetof(INTEGRATED_GBE, LTRCAP), *(PUINT32)&cfg->LTRCAP,
            cfg->LTRCAP.MSL,
            cfg->LTRCAP.MSLS,
            cfg->LTRCAP.RES_15_13,
            cfg->LTRCAP.MNSL,
            cfg->LTRCAP.MNSLS,
            cfg->LTRCAP.RES_31_29);

    FlushLogFileBuffer(LogFile, LogBuffer, LogBufferSize, LogBufferPtr, LogBufferRestSize);

    return status;
};



NTSTATUS logCfg(
    _In_ PBDF BDF,
    _In_ PVOID Buffer,
    _In_ SIZE_T BufferSize,
    _In_ PLOG Log
)
{
    NTSTATUS status = 0;
    NTSTATUS logStatus = 0;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;

    FEnter();

    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                          "BDF: %02x:%02x:%02x\r\n"
                          "Bytes:\r\n",
                          BDF->Bus, BDF->Device, BDF->Function);
    logStatus = LogBytes(NULL, NULL, Buffer, (ULONG)BufferSize, 0, TRUE, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    if ( BufferSize >= 0x40 )
    {
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                              "\r\n");
        logStatus = logPciConfig(Buffer, (ULONG)BufferSize, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
        
        if ( ((PPCI_CONFIG)Buffer)->VendorId == PCI_VENDORID_INTEL )
        {
            switch ( BDF->Bus )
            {
                case 0:
                {
                    switch ( BDF->Device )
                    {
                        case 0:
                        {
                            switch ( BDF->Function )
                            {
                                // 00:00:00
                                case 0:
                                {
                                    logStatus = logHostBridgeDRAMConfig(Buffer, (ULONG)BufferSize, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
                                    break;
                                }
                                default:
                                    break;
                            }
                            break;
                        }
                        case 0x12:
                            switch ( BDF->Function )
                            {
                                // 00:12:00
                                case 0:
                                {
                                    logStatus = logThermalSubsystemConfig(Buffer, (ULONG)BufferSize, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
                                    break;
                                }
                                default:
                                    break;
                            }
                            break;
                        case 0x16:
                            switch ( BDF->Function )
                            {
                                // 00:16:00
                                // 00:16:01
                                // 00:16:04
                                // 00:16:05
                                case 0:
                                case 1:
                                case 4:
                                case 5:
                                {
                                    logStatus = logIMEIConfig(Buffer, (ULONG)BufferSize, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
                                    break;
                                }
                                default:
                                    break;
                            }
                            break;
                        case 0x1f:
                        {
                            switch ( BDF->Function )
                            {
                                // 00:1f:00
                                case 0:
                                {
                                    logStatus = logLpcConfig(Buffer, (ULONG)BufferSize, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
                                    break;
                                }
                                // 00:1f:04
                                case 4:
                                {
                                    logStatus = logSMBusConfig(Buffer, (ULONG)BufferSize, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
                                    break;
                                }
                                // 00:1f:05
                                case 5:
                                {
                                    logStatus = logSpiConfig(Buffer, (ULONG)BufferSize, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
                                    break;
                                }
                                // 00:1f:06
                                case 6:
                                {
                                    logStatus = logGBeConfig(Buffer, (ULONG)BufferSize, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
                                    break;
                                }
                                default:
                                    break;
                            }
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        logStatus = RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "\r\n\r\n\r\n");
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }

//clean:
    if ( logStatus )
        DPrint("logStatus: 0x%x\n", logStatus);

    FLeave();
    return status;
}

NTSTATUS logPciConfig(
    _In_ PVOID Buffer, 
    _In_ ULONG BufferSize, 
    _In_ HANDLE LogFile,
    _In_ PCHAR LogBuffer,
    _In_ ULONG LogBufferSize,
    _Inout_ PCHAR *LogBufferPtr,
    _In_ PSIZE_T LogBufferRestSize
)
{
    NTSTATUS status = 0;

    PPCI_CONFIG cfg = (PPCI_CONFIG)Buffer;

    PBAR bar = NULL;
    PBAR nextBar = NULL;
    PBAR lastBar = NULL;
    
    (LogFile);
    (LogBuffer);
    (LogBufferSize);

    if ( BufferSize < 0x40 )
        return status;
    if ( isReadDenied(Buffer, 0x40) )
        return status;
    //if ( pciConfig->VendorId == PCI_INVALID_VENDORID )
    //    return 0;
    
    status = RtlStringCchPrintfExA(
                (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                "VendorId: 0x%04x (%s)\r\n"
                "DeviceId: 0x%04x (%s)\r\n"
                "Command: 0x%04x\r\n"
                "  IoSpace [0]: 0x%x\r\n"
                "  MemorySpace [1]: 0x%x\r\n"
                "  BusMaster [2]: 0x%x\r\n"
                "  SpecialCycles [3]: 0x%x\r\n"
                "  MemoryWriteAndInvalidateEnable [4]: 0x%x\r\n"
                "  VGAPaletteSnoop [5]: 0x%x\r\n"
                "  ParityErrorResponse [6]: 0x%x\r\n"
                "  WaitCycle [7]: 0x%x\r\n"
                "  SERREnable [8]: 0x%x\r\n"
                "  FastBackToBackEnable [9]: 0x%x\r\n"
                "  InterruptDisable [10]: 0x%x\r\n"
                "  Reserved [15:11]: 0x%x\r\n"
                "Status: 0x%04x\r\n"
                "  Reserved [2:0]: 0x%x\r\n"
                "  InterruptStatus [3]: 0x%x\r\n"
                "  CapabilitiesList [4]: 0x%x\r\n"
                "  MHzCapable66 [5]: 0x%x\r\n"
                "  UdfSupported [6]: 0x%x\r\n"
                "  FastBackToBackCapable [7]: 0x%x\r\n"
                "  MasterDataParityError [8]: 0x%x\r\n"
                "  DEVSELTiming [10:9]: 0x%x\r\n"
                "  SignaledTargetAbort [11]: 0x%x\r\n"
                "  ReceivedTargetAbort [12]: 0x%x\r\n"
                "  ReceivedMasterAbort [13]: 0x%x\r\n"
                "  SignaledSystemError [14]: 0x%x\r\n"
                "  DetectedParityError [15]: 0x%x\r\n"
                "RevisionID: 0x%x\r\n"
                "ProgIF: 0x%x\r\n"
                "Subclass: 0x%x (%s)\r\n"
                "ClassCode: 0x%x (%s)\r\n"
                "CacheLineSize: 0x%x\r\n"
                "LatencyTimer: 0x%x\r\n"
                "HeaderType: 0x%x\r\n"
                "  Id [6:0]: 0x%x (%s)\r\n"
                "  MultiFunction [7]: 0x%x\r\n"
                "BIST: 0x%x\r\n"
                "  CompletionCode [3:0]: 0x%x\r\n"
                "  Reserved [5:4]: 0x%x\r\n"
                "  StartBIST [6]: 0x%x\r\n"
                "  BISTCapable [7]: 0x%x\r\n",
                cfg->VendorId, vendorIdStr(cfg->VendorId),
                cfg->DeviceId, deviceIdStr(cfg->VendorId, cfg->DeviceId),
                cfg->Command.Value,
                    cfg->Command.IoSpace,
                    cfg->Command.MemorySpace,
                    cfg->Command.BusMaster,
                    cfg->Command.SpecialCycles,
                    cfg->Command.MemoryWriteAndInvalidateEnable,
                    cfg->Command.VGAPaletteSnoop,
                    cfg->Command.ParityErrorResponse,
                    cfg->Command.WaitCycle,
                    cfg->Command.SERREnable,
                    cfg->Command.FastBackToBackEnable,
                    cfg->Command.InterruptDisable,
                    cfg->Command.Reserved2,
                cfg->Status.Value,
                    cfg->Status.Reserved_2_1,
                    cfg->Status.InterruptStatus,
                    cfg->Status.CapabilitiesList,
                    cfg->Status.MHzCapable66,
                    cfg->Status.UdfSupported,
                    cfg->Status.FastBackToBackCapable,
                    cfg->Status.MasterDataParityError,
                    cfg->Status.DEVSELTiming,
                    cfg->Status.SignaledTargetAbort,
                    cfg->Status.ReceivedTargetAbort,
                    cfg->Status.ReceivedMasterAbort,
                    cfg->Status.SignaledSystemError,
                    cfg->Status.DetectedParityError,
                cfg->RevisionID,
                cfg->ProgIF,
                cfg->Subclass, subClassStr(cfg->ClassCode, cfg->Subclass),
                cfg->ClassCode, classCodeStr(cfg->ClassCode),
                cfg->CacheLineSize,
                cfg->LatencyTimer,
                cfg->HeaderType.Value,
                    cfg->HeaderType.Id, HEADER_TYPE_STR(cfg->HeaderType.Id),
                    cfg->HeaderType.MultiFunction,
                cfg->BIST.Value,
                    cfg->BIST.CompletionCode,
                    cfg->BIST.Reserved,
                    cfg->BIST.StartBIST,
                    cfg->BIST.BISTCapable);
                
    if ( cfg->HeaderType.Id == PCI_HEADER_GENERAL_DEVICE )
    {
        PPCI_CONFIG_GENERAL pciCfgGeneral = (PPCI_CONFIG_GENERAL)Buffer;

        for ( ULONG i = 0; i < PCI_TYPE0_ADDRESSES; i++ )
        {
            bar = (PBAR)&pciCfgGeneral->Bar[i];
            nextBar = (i+1<PCI_TYPE0_ADDRESSES)?(PBAR)&pciCfgGeneral->Bar[i+1]:NULL;
            
            RtlStringCchPrintfExA(
                        (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                        "Bar[%u]: 0x%08x", 
                        i, bar->Value);
            if ( bar->Value
                && ( !lastBar
                    || lastBar->BarType!=BAR_TYPE_MEMORY_SPACE 
                    || lastBar->MemorySpace.Type!=MEMORY_SPACE_BAR_64_BIT ) )
            {
                RtlStringCchPrintfExA(
                            (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                            " (%s, %s, 0x%llx)", 
                            BAR_TYPE_STR(bar), BAR_ADDRESS_WIDTH(bar), (UINT64)getBarBaseAddress(bar, nextBar));
            }
            RtlStringCchPrintfExA(
                        (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                    "\r\n");
            lastBar = bar;
        }
        RtlStringCchPrintfExA(
                    (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                    "CardbusCISPointer: 0x%x\r\n"
                    "SubsystemVendorID: 0x%x (%s)\r\n"
                    "SubsystemID: 0x%x (%s)\r\n"
                    "ERBA: 0x%x\r\n"
                    "  Enable: 0x%x\r\n"
                    "  Reserved: 0x%x\r\n"
                    "  Address: 0x%x\r\n"
                    "CapabilitiesPointer: 0x%x\r\n"
                    "Reserved1: %x %x %x\r\n"
                    "Reserved2: 0x%x\r\n"
                    "InterruptLine: 0x%x\r\n"
                    "InterruptPIN: 0x%x\r\n"
                    "MinGrant: 0x%x\r\n"
                    "MaxLatency: 0x%x\r\n",
                    pciCfgGeneral->CardbusCISPointer,
                    pciCfgGeneral->SubsystemVendorID, vendorIdStr(pciCfgGeneral->SubsystemVendorID), 
                    pciCfgGeneral->SubsystemID, deviceIdStr(pciCfgGeneral->SubsystemVendorID, pciCfgGeneral->SubsystemID),
                    *(PUINT32)&pciCfgGeneral->ERBA,
                        pciCfgGeneral->ERBA.Enable,
                        pciCfgGeneral->ERBA.Reserved,
                        pciCfgGeneral->ERBA.BaseAddress,
                    pciCfgGeneral->CapabilitiesPointer,
                    pciCfgGeneral->Reserved1[0],pciCfgGeneral->Reserved1[1],pciCfgGeneral->Reserved1[2],
                    pciCfgGeneral->Reserved2,
                    pciCfgGeneral->InterruptLine,
                    pciCfgGeneral->InterruptPIN,
                    pciCfgGeneral->MinGrant,
                    pciCfgGeneral->MaxLatency);
    }
    else if ( cfg->HeaderType.Id == PCI_HEADER_PCI_TO_PCI_BRIDGE )
    {
        PPCI_CONFIG_PCI_BRIDGE pciCfgPciBridge = (PPCI_CONFIG_PCI_BRIDGE)Buffer;
        
        for ( ULONG i = 0; i < PCI_TYPE1_ADDRESSES; i++ )
        {
            bar = (PBAR)&pciCfgPciBridge->Bar[i];
            nextBar = (i+1<PCI_TYPE1_ADDRESSES)?(PBAR)&pciCfgPciBridge->Bar[i+1]:NULL;

            RtlStringCchPrintfExA(
                        (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                        "Bar[%u]: 0x%08x", 
                        i, bar->Value);
            if ( bar->Value
                && ( !lastBar
                    || lastBar->BarType!=BAR_TYPE_MEMORY_SPACE 
                    || lastBar->MemorySpace.Type!=MEMORY_SPACE_BAR_64_BIT ) )
            {
                RtlStringCchPrintfExA(
                            (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                            " (%s, %s, 0x%llx)", 
                            BAR_TYPE_STR(bar), BAR_ADDRESS_WIDTH(bar), (UINT64)getBarBaseAddress(bar, nextBar));
            }
            RtlStringCchPrintfExA(
                        (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                    "\r\n");
            lastBar = bar;
        }

        RtlStringCchPrintfExA(
                    (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                    "PrimaryBusNumber: 0x%x\r\n"
                    "SecondaryBusNumber: 0x%x\r\n"
                    "SubordinateBusNumber: 0x%x\r\n"
                    "SecondaryLatencyTimer: 0x%x\r\n"
                    "IOBase: 0x%x\r\n"
                    "IOLimit: 0x%x\r\n"
                    "SecondaryStatus: 0x%x\r\n"
                    "MemoryBase: 0x%x\r\n"
                    "MemoryLimit: 0x%x\r\n"
                    "PrefetchableMemoryBase: 0x%x\r\n"
                    "PrefetchableMemoryLimit: 0x%x\r\n"
                    "PrefetchableBaseUpper32Bits: 0x%x\r\n"
                    "PrefetchableLimitUpper32Bits: 0x%x\r\n"
                    "IOBaseUpper16Bits: 0x%x\r\n"
                    "IOLimitUpper16Bits: 0x%x\r\n"
                    "CapabilitiesPointer: 0x%x\r\n"
                    "Reserved1: %x %x %x\r\n"
                    "ERBA: 0x%x\r\n"
                    "  Enable:1: 0x%x\r\n"
                    "  Reserved:10: 0x%x\r\n"
                    "  Address:21: 0x%x\r\n"
                    "InterruptLine: 0x%x\r\n"
                    "InterruptPIN: 0x%x\r\n"
                    "BridgeControl: 0x%x\r\n",
                    pciCfgPciBridge->PrimaryBusNumber,
                    pciCfgPciBridge->SecondaryBusNumber,
                    pciCfgPciBridge->SubordinateBusNumber,
                    pciCfgPciBridge->SecondaryLatencyTimer,
                    pciCfgPciBridge->IOBase,
                    pciCfgPciBridge->IOLimit,
                    pciCfgPciBridge->SecondaryStatus,
                    pciCfgPciBridge->MemoryBase,
                    pciCfgPciBridge->MemoryLimit,
                    pciCfgPciBridge->PrefetchableMemoryBase,
                    pciCfgPciBridge->PrefetchableMemoryLimit,
                    pciCfgPciBridge->PrefetchableBaseUpper32Bits,
                    pciCfgPciBridge->PrefetchableLimitUpper32Bits,
                    pciCfgPciBridge->IOBaseUpper16Bits,
                    pciCfgPciBridge->IOLimitUpper16Bits,
                    pciCfgPciBridge->CapabilitiesPointer,
                    pciCfgPciBridge->Reserved1[0], pciCfgPciBridge->Reserved1[1], pciCfgPciBridge->Reserved1[2],
                    *(PUINT32)&pciCfgPciBridge->ERBA,
                        pciCfgPciBridge->ERBA.Enable,
                        pciCfgPciBridge->ERBA.Reserved,
                        pciCfgPciBridge->ERBA.BaseAddress,
                    pciCfgPciBridge->InterruptLine,
                    pciCfgPciBridge->InterruptPIN,
                    pciCfgPciBridge->BridgeControl);
    }
    else if ( cfg->HeaderType.Id == PCI_HEADER_PCI_TO_CARDBUS_BRIDGE
              && BufferSize >= 0x48 )
    {
        PPCI_CONFIG_PCI_CB_BRIDGE pciConfigPciCbBridge = (PPCI_CONFIG_PCI_CB_BRIDGE)Buffer;

        status = RtlStringCchPrintfExA(
                    (*LogBufferPtr), (*LogBufferRestSize), LogBufferPtr, LogBufferRestSize, 0,
                    "{\r\n"
                    "  CardBusSocket: 0x%x\r\n"
                    "  ExCaBaseAddress: 0x%x\r\n"
                    "}\r\n"
                    "OffsetOfCapabilitiesList: 0x%x\r\n"
                    "Reserved: 0x%x\r\n"
                    "SecondaryStatus: 0x%x\r\n"
                    "PCIBBusNumber: 0x%x\r\n"
                    "CardBusBusNumber: 0x%x\r\n"
                    "SubordinateBusNumber: 0x%x\r\n"
                    "CardBusLatencyTimer: 0x%x\r\n"
                    "MemoryBaseAddress0: 0x%x\r\n"
                    "MemoryLimit0: 0x%x\r\n"
                    "MemoryBaseAddress1: 0x%x\r\n"
                    "MemoryLimit1: 0x%x\r\n"
                    "IOBaseAddress0: 0x%x\r\n"
                    "IOLimit0: 0x%x\r\n"
                    "IOBaseAddress1: 0x%x\r\n"
                    "IOLimit1: 0x%x\r\n"
                    "InterruptLine: 0x%x\r\n"
                    "InterruptPIN: 0x%x\r\n"
                    "BridgeControl: 0x%x\r\n"
                    "SubsystemDeviceID: 0x%x (%s)\r\n"
                    "SubsystemVendorID: 0x%x (%s)\r\n"
                    "PCCardLegacyModeBaseAddress: 0x%x\r\n",
                    pciConfigPciCbBridge->CardBusSocket,
                    pciConfigPciCbBridge->ExCaBaseAddress, 
                    pciConfigPciCbBridge->OffsetOfCapabilitiesList, 
                    pciConfigPciCbBridge->Reserved, 
                    pciConfigPciCbBridge->SecondaryStatus, 
                    pciConfigPciCbBridge->PCIBBusNumber, 
                    pciConfigPciCbBridge->CardBusBusNumber, 
                    pciConfigPciCbBridge->SubordinateBusNumber, 
                    pciConfigPciCbBridge->CardBusLatencyTimer, 
                    pciConfigPciCbBridge->MemoryBaseAddress0, 
                    pciConfigPciCbBridge->MemoryLimit0, 
                    pciConfigPciCbBridge->MemoryBaseAddress1, 
                    pciConfigPciCbBridge->MemoryLimit1, 
                    pciConfigPciCbBridge->IOBaseAddress0, 
                    pciConfigPciCbBridge->IOLimit0, 
                    pciConfigPciCbBridge->IOBaseAddress1, 
                    pciConfigPciCbBridge->IOLimit1, 
                    pciConfigPciCbBridge->InterruptLine, 
                    pciConfigPciCbBridge->InterruptPIN, 
                    pciConfigPciCbBridge->BridgeControl, 
                    pciConfigPciCbBridge->SubsystemDeviceID, deviceIdStr(pciConfigPciCbBridge->SubsystemVendorID, pciConfigPciCbBridge->SubsystemDeviceID), 
                    pciConfigPciCbBridge->SubsystemVendorID, vendorIdStr(pciConfigPciCbBridge->SubsystemVendorID), 
                    pciConfigPciCbBridge->PCCardLegacyModeBaseAddress);
    }

    return status;
};
