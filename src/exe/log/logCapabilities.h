#pragma once

#include "../../inc/pci/capabilities.h"


#define CHECK_CAP_HDR_CFG_BUFFER_SIZE(_cb_, _ch_, _ct_, _cbs_) \
    if ( (SIZE_T)(_ch_) - (SIZE_T)(_cb_) + sizeof(_ct_) > (_cbs_) ) \
    { \
        status = STATUS_BUFFER_TOO_SMALL; \
        EPrint("CfgBufferSize too small! (0x%x)\n", status); \
        goto clean; \
    }


//111 1238 mW (e.g., 3.3 V at 375 mA)
//110 1056 mW (e.g., 3.3 V at 320 mA)
//101 891 mW (e.g., 3.3 V at 270 mA)
//100 726 mW (e.g., 3.3 V at 220 mA)
//011 528 mW (e.g., 3.3 V at 160 mA)
//010 330 mW (e.g., 3.3 V at 100 mA)
//001 182 mW (e.g., 3.3 V at 55 mA)
//000   0 mW (no Vaux power or self powered)
FORCEINLINE
PCHAR auxCurrentStr(_In_ UINT32 Value)
{
    switch ( Value )
    {
        case 0: return "0 mW";
        case 1: return "182 mW";
        case 2: return "330 mW";
        case 3: return "528 mW";
        case 4: return "726 mW";
        case 5: return "891 mW";
        case 6: return "1056 mW";
        case 7: return "1238 mW";
        default: return "Unknown";
    }
}

INT logPowerManagementCapability(
    _In_ PPCI_CAPABILITIES_HEADER CapHeader,
    _In_ PVOID CfgBuffer,
    _In_ ULONG CfgBufferSize,
    _In_ PLOG Log
)
{
    FEnter();

    NTSTATUS status = 0;

    PCHAR logBufferPtr = Log->Buffer;
    SIZE_T logBufferRestSize = Log->BufferSize;

    PPCI_CONFIG cfg = (PPCI_CONFIG)CfgBuffer;
    
    CHECK_CAP_HDR_CFG_BUFFER_SIZE(CfgBuffer, (CapHeader), FULL_PCI_PM_CAPABILITY, (CfgBufferSize))

    PFULL_PCI_PM_CAPABILITY cap = (PFULL_PCI_PM_CAPABILITY)CapHeader;
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "Capabilities: 0x%04x\r\n"
                            "  Version [2:0]: 0x%x\r\n"
                            "  PMEClock [3]: 0x%x\r\n"
                            "  Rsvd1 [4]: 0x%x\r\n"
                            "  DeviceSpecificInitialization [5]: 0x%x\r\n"
                            "  Aux_Current [8:6]: 0x%x (%s)\r\n"
                            "  D1 [9]: 0x%x\r\n"
                            "  D2 [10]: 0x%x\r\n"
                            "  PME_D0 [11]: 0x%x\r\n"
                            "  PME_D1 [12]: 0x%x\r\n"
                            "  PME_D2 [13]: 0x%x\r\n"
                            "  PME_D3_Hot [14]: 0x%x\r\n"
                            "  PME_D3_Cold [15]: 0x%x\r\n"
                            "ControlStatus: 0x%04x\r\n"
                            "  PowerState [1:0]: 0x%x\r\n"
                            "  Rsvd1 [2]: 0x%x\r\n"
                            "  NoSoftReset [3]: 0x%x\r\n"
                            "  Rsvd2 [7:4]: 0x%x\r\n"
                            "  PMEEnable [8]: 0x%x\r\n"
                            "  DataSelect [12:9]: 0x%x\r\n"
                            "  DataScale [14:13]: 0x%x\r\n"
                            "  PMEStatus [15]: 0x%x\r\n"
                            "BridgeSupport: 0x%02x\r\n"
                            "  Rsvd1 [5:0]: 0x%x\r\n"
                            "  D3HotSupportsStopClock [6]: 0x%x\r\n"
                            "  BusPowerClockControlEnabled [7]: 0x%x\r\n"
                            "Data: 0x%x\r\n"
                            "\r\n",
                            cap->PMC.AsUSHORT,
                            cap->PMC.Capabilities.Version,
                            cap->PMC.Capabilities.PMEClock,
                            cap->PMC.Capabilities.Rsvd1,
                            cap->PMC.Capabilities.DeviceSpecificInitialization,
                            cap->PMC.Capabilities.AuxCurrent, auxCurrentStr(cap->PMC.Capabilities.AuxCurrent),
                            cap->PMC.Capabilities.D1,
                            cap->PMC.Capabilities.D2,
                            cap->PMC.Capabilities.PME_D0,
                            cap->PMC.Capabilities.PME_D1,
                            cap->PMC.Capabilities.PME_D2,
                            cap->PMC.Capabilities.PME_D3_Hot,
                            cap->PMC.Capabilities.PME_D3_Cold,
                            cap->PMCSR.AsUSHORT,
                            cap->PMCSR.ControlStatus.PowerState,
                            cap->PMCSR.ControlStatus.Rsvd1,
                            cap->PMCSR.ControlStatus.NoSoftReset,
                            cap->PMCSR.ControlStatus.Rsvd2,
                            cap->PMCSR.ControlStatus.PMEEnable,
                            cap->PMCSR.ControlStatus.DataSelect,
                            cap->PMCSR.ControlStatus.DataScale,
                            cap->PMCSR.ControlStatus.PMEStatus,
                            cap->PMCSR_BSE.AsUCHAR,
                            cap->PMCSR_BSE.BridgeSupport.Rsvd1,
                            cap->PMCSR_BSE.BridgeSupport.D3HotSupportsStopClock,
                            cap->PMCSR_BSE.BridgeSupport.BusPowerClockControlEnabled,
                            cap->Data);
    
clean:
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                    "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    FLeave();
    return status;
}


FORCEINLINE
PCHAR capabilityIdStr(_In_ UCHAR ID)
{
    switch ( ID )
    {
        case PCI_CAPABILITY_ID_POWER_MANAGEMENT  : return "Power Management";
        case PCI_CAPABILITY_ID_AGP               : return "Accelerated Graphics Port (AGP)";
        case PCI_CAPABILITY_ID_VPD               : return "Vital Product Data (VPD)";
        case PCI_CAPABILITY_ID_SLOT_ID           : return "External expansion capabilities of a bridge device";
        case PCI_CAPABILITY_ID_MSI               : return "MSI functionality";
        case PCI_CAPABILITY_ID_CPCI_HOTSWAP      : return "CompactPCI HotSwap";
        case PCI_CAPABILITY_ID_PCIX              : return "PCI-X features";
        case PCI_CAPABILITY_ID_HYPERTRANSPORT    : return "Control and status for hyper transport (HT) technology links";
        case PCI_CAPABILITY_ID_VENDOR_SPECIFIC   : return "Vendor specific information";
        case PCI_CAPABILITY_ID_DEBUG_PORT        : return "Debug port";
        case PCI_CAPABILITY_ID_CPCI_RES_CTRL     : return "CompactPCI central resource control";
        case PCI_CAPABILITY_ID_SHPC              : return "Features of a hot-plug controller ";
        case PCI_CAPABILITY_ID_P2P_SSID          : return "Subsystem ID";
        case PCI_CAPABILITY_ID_AGP_TARGET        : return "Accelerated Graphics Port (AGP) 8x";
        case PCI_CAPABILITY_ID_SECURE            : return "Secure device";
        case PCI_CAPABILITY_ID_PCI_EXPRESS       : return "PCI express";
        case PCI_CAPABILITY_ID_MSIX              : return "MSI-X functionality";
        case PCI_CAPABILITY_ID_SATA_CONFIG       : return "Sata config";
        case PCI_CAPABILITY_ID_ADVANCED_FEATURES : return "Advanced features";
        case PCI_CAPABILITY_ID_FPB               : return "FPB";
        default : return "Unknown";
    }
}

FORCEINLINE
NTSTATUS logMSICapability(
    _In_ PPCI_CAPABILITIES_HEADER CapHeader,
    _In_ PVOID CfgBuffer,
    _In_ ULONG CfgBufferSize,
    _In_ PLOG Log
)
{
    FEnter();
    NTSTATUS status = 0;

    PCHAR logBufferPtr = Log->Buffer;
    SIZE_T logBufferRestSize = Log->BufferSize;

    PPCI_CONFIG cfg = (PPCI_CONFIG)CfgBuffer;

    PPCI_CAPABILITY_MSI cap = (PPCI_CAPABILITY_MSI)CapHeader;
    
    CHECK_CAP_HDR_CFG_BUFFER_SIZE(CfgBuffer, (CapHeader), PCI_CAPABILITY_MSI, (CfgBufferSize))
    
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                        "Message Control Register: 0x%04x\r\n"
                        "  ME [0]: 0x%x\r\n"
                        "  MMC [3:1]: 0x%x (0x%x)\r\n"
                        "  MME [6:4]: 0x%x (0x%x)\r\n"
                        "  ADD64 [7]: 0x%x\r\n"
                        "  PVM [8]: 0x%x\r\n"
                        "  EMDC [9]: 0x%x\r\n"
                        "  EMDE [10]: 0x%x\r\n"
                        "  Res_15_11 [15:11]: 0x%x\r\n",
                        cap->MMC.AsUSHORT,
                            cap->MMC.bits.ME,
                            cap->MMC.bits.MMC, cap->MMC.bits.ME?(1<<cap->MMC.bits.MMC):0, 
                            cap->MMC.bits.MME, cap->MMC.bits.ME?(1<<cap->MMC.bits.MME):0, 
                            cap->MMC.bits.ADD64,
                            cap->MMC.bits.PVM,
                            cap->MMC.bits.EMDC,
                            cap->MMC.bits.EMDE,
                            cap->MMC.bits.Res_15_11);

    UINT32 offset = 4;
    PUINT8 capBasePtr = (PUINT8)CapHeader;
    // If the MSI Enable bit is Set, 
    // the contents of this register specify the DWORD-aligned address (Address[31:02]) for the MSI transaction. 
    // Address[1:0] are set to 00b.
    // Default value of this field is undefined.
    //if ( cap->MMC.bits.ME )
    {
        PPCI_CAPABILITY_MSI_MMLA mmla = (PPCI_CAPABILITY_MSI_MMLA)(capBasePtr+offset);
    
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "Message Lower Address Register: 0x%08x\r\n"
                            "  Res_1_0 [1:0]: 0x%x\r\n"
                            "  MLA [31:2]: 0x%x (%08X)\r\n",
                            mmla->AsULONG,
                                mmla->bits.Res_1_0,
                                mmla->bits.MLA, (mmla->AsULONG & ~3));

        // This register is implemented only if the Function supports a 64-bit message address 
        // (64-bit Address Capable is Set (ADD64)).
        if ( cap->MMC.bits.ADD64)
        {
            offset += 4;
            PPCI_CAPABILITY_MSI_MMUA mmua = (PPCI_CAPABILITY_MSI_MMUA)(capBasePtr+offset);

            RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "Message Upper Address Register: 0x%08x\r\n"
                                "  MUA [31:0]: 0x%x\r\n",
                                mmua->AsULONG,
                                    mmua->bits.MUA);
        }

        // If the MSI Enable bit is Set, the Function sends a DWORD Memory Write transaction using Message Data for the lower 16 bits. 
        // All 4 Byte Enables are Set
        offset += 4;
        PPCI_CAPABILITY_MSI_MMD mmd = (PPCI_CAPABILITY_MSI_MMD)(capBasePtr+offset);

        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "Message Data Register: 0x%04x\r\n"
                            "  MD [15:0]: 0x%x\r\n"
                            "\r\n",
                            (UINT16)mmd->AsULONG,
                            mmd->bits.MD);
        // This register is optional. 
        // For the MSI Capability structures without Per-vector Masking, 
        // it must be implemented if the Extended Message Data Capable bit is Set; 
        // otherwise, it is outside the MSI Capability structure and undefined.
         if ( !cap->MMC.bits.PVM && cap->MMC.bits.EMDC )
         {
             RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "Extended Message Data Register: 0x%04x\r\n"
                            "  EMD [15:0]: 0x%x\r\n"
                            "\r\n",
                            (UINT16)(mmd->AsULONG>>0x10),
                            mmd->bits.EMD);
         }
    }

    // This register is optional. 
    // It is present if Per-Vector Masking Capable is Set. 
    // The offset of this register within the capability depends on the value of the 64-bit Address Capable bit.
    if ( cap->MMC.bits.PVM )
    {
        offset += 4;
        CHAR bin32Str[BIN_32_STR_BUFFER_SIZE];
        UINT32 maskingBits = *(PUINT32)(capBasePtr+offset);
        uint32ToBin(maskingBits, bin32Str);

        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                        "MaskingBits: 0x%x\r\n"
                        "  Bits [31:0]: %s\r\n",
                        maskingBits,
                        bin32Str);

        offset += 4;
        UINT32 pendingBits = *(PUINT32)(capBasePtr+offset);
        uint32ToBin(pendingBits, bin32Str);

        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                        "PendingBits: 0x%x\r\n"
                        "  Bits [31:0]: %s\r\n",
                        pendingBits,
                        bin32Str);
    }

clean:
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                    "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    FLeave();
    return status;
}

NTSTATUS logPciXCapability(
    _In_ HANDLE Device,
    _In_ PPCI_CAPABILITIES_HEADER CapHeader,
    _In_ PVOID CfgBuffer,
    _In_ ULONG CfgBufferSize,
    _In_ PLOG Log
)
{
    FEnter();

    NTSTATUS status = 0;

    PCHAR logBufferPtr = Log->Buffer;
    SIZE_T logBufferRestSize = Log->BufferSize;
    
    CHECK_CAP_HDR_CFG_BUFFER_SIZE(CfgBuffer, (CapHeader), PCI_X_CAPABILITY, (CfgBufferSize))

    PPCI_X_CAPABILITY cap = (PPCI_X_CAPABILITY)CapHeader;
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                        "Command: 0x%04x\r\n"
                        "  DataParityErrorRecoveryEnable [0]: 0x%x\r\n"
                        "  EnableRelaxedOrdering [1]: 0x%x\r\n"
                        "  MaxMemoryReadByteCount [3:2]: 0x%x\r\n"
                        "  MaxOutstandingSplitTransactions [6:4]: 0x%x\r\n"
                        "  Reserved [15:7]: 0x%x\r\n"
                        "Status: 0x%08x\r\n"
                        "  FunctionNumber [2:0]: 0x%x\r\n"
                        "  DeviceNumber [7:3]: 0x%x\r\n"
                        "  BusNumber [15:8]: 0x%x\r\n"
                        "  Device64Bit [16]: 0x%x\r\n"
                        "  Capable133MHz [17]: 0x%x\r\n"
                        "  SplitCompletionDiscarded [18]: 0x%x\r\n"
                        "  UnexpectedSplitCompletion [19]: 0x%x\r\n"
                        "  DeviceComplexity [20]: 0x%x\r\n"
                        "  DesignedMaxMemoryReadByteCount [22:21]: 0x%x\r\n"
                        "  DesignedMaxOutstandingSplitTransactions [25:23]: 0x%x\r\n"
                        "  DesignedMaxCumulativeReadSize [28:26]: 0x%x\r\n"
                        "  ReceivedSplitCompletionErrorMessage [29]: 0x%x\r\n"
                        "  CapablePCIX266 [30]: 0x%x\r\n"
                        "  CapablePCIX533 [31]: 0x%x\r\n"
                        "\r\n",
                        cap->Command.AsUSHORT,
                            cap->Command.bits.DataParityErrorRecoveryEnable,
                            cap->Command.bits.EnableRelaxedOrdering,
                            cap->Command.bits.MaxMemoryReadByteCount,
                            cap->Command.bits.MaxOutstandingSplitTransactions,
                            cap->Command.bits.Reserved,
                        cap->Status.AsULONG,
                            cap->Status.bits.FunctionNumber,
                            cap->Status.bits.DeviceNumber,
                            cap->Status.bits.BusNumber,
                            cap->Status.bits.Device64Bit,
                            cap->Status.bits.Capable133MHz,
                            cap->Status.bits.SplitCompletionDiscarded,
                            cap->Status.bits.UnexpectedSplitCompletion,
                            cap->Status.bits.DeviceComplexity,
                            cap->Status.bits.DesignedMaxMemoryReadByteCount,
                            cap->Status.bits.DesignedMaxOutstandingSplitTransactions,
                            cap->Status.bits.DesignedMaxCumulativeReadSize,
                            cap->Status.bits.ReceivedSplitCompletionErrorMessage,
                            cap->Status.bits.CapablePCIX266,
                            cap->Status.bits.CapablePCIX533);
    
clean:
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                    "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    FLeave();
    return status;
}

NTSTATUS logMSIXCapability(
    _In_ HANDLE Device,
    _In_ PPCI_CAPABILITIES_HEADER CapHeader,
    _In_ PVOID CfgBuffer,
    _In_ ULONG CfgBufferSize,
    _In_ PLOG Log
)
{
    FEnter();
    NTSTATUS status = 0;

    PCHAR logBufferPtr = Log->Buffer;
    SIZE_T logBufferRestSize = Log->BufferSize;

    PPCI_CONFIG cfg = (PPCI_CONFIG)CfgBuffer;

    PPCI_CAPABILITY_MSIX cap = (PPCI_CAPABILITY_MSIX)CapHeader;
    PUINT8 buffer = NULL;
    
    CHECK_CAP_HDR_CFG_BUFFER_SIZE(CfgBuffer, (CapHeader), PCI_CAPABILITY_MSIX, (CfgBufferSize))

    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                        "Message Control Register: 0x%04x\r\n"
                        "  TableSize [10:0]: 0x%x (0x%x)\r\n"
                        "  Res_13_11 [13:11]: 0x%x\r\n"
                        "  FunctionMask [14]: 0x%x\r\n"
                        "  MSXE [15]: 0x%x\r\n"
                        "Table Offset BIR: 0x%08x\r\n"
                        "  Table [2:0]: 0x%x\r\n"
                        "  Offset [31:3]: 0x%x (0x%x)\r\n"
                        "PBA Offset BIR: 0x%08x\r\n"
                        "  PBA [2:0]: 0x%x\r\n"
                        "  Offset [31:3]: 0x%x (0x%x)\r\n",
                        cap->MMC.AsUINT16,
                            cap->MMC.bits.TableSize, cap->MMC.bits.TableSize + 1,
                            cap->MMC.bits.Res_13_11,
                            cap->MMC.bits.FunctionMask,
                            cap->MMC.bits.MSXE,
                        cap->TableOffsetBIR.AsUINT32,
                            cap->TableOffsetBIR.bits.Table,
                            cap->TableOffsetBIR.bits.Offset, (cap->TableOffsetBIR.bits.Offset<<3),
                        cap->PBAOffsetBIR.AsUINT32,
                            cap->PBAOffsetBIR.bits.PBA,
                            cap->PBAOffsetBIR.bits.Offset, (cap->PBAOffsetBIR.bits.Offset<<3));

    UINT32 nrOfEntries = cap->MMC.bits.TableSize + 1;
    UINT32 tableSize = nrOfEntries * sizeof(MSIX_TABLE_STRUCTURE);
    UINT32 pbaSize = nrOfEntries * sizeof(MSIX_PBA_STRUCTURE);
    PBAR tableBar = NULL;
    PBAR tableNextBar = NULL;
    PBAR pbaBar = NULL;
    PBAR pbaNextBar = NULL;
    if ( cfg->HeaderType.Id == PCI_HEADER_GENERAL_DEVICE ) 
    {
        tableBar = (PBAR)&((PPCI_CONFIG_GENERAL)cfg)->Bar[cap->TableOffsetBIR.bits.Table];
        if ( cap->TableOffsetBIR.bits.Table+1 < PCI_TYPE0_ADDRESSES )
            tableNextBar = (PBAR)&((PPCI_CONFIG_GENERAL)cfg)->Bar[cap->TableOffsetBIR.bits.Table+1];
        pbaBar = (PBAR)&((PPCI_CONFIG_GENERAL)cfg)->Bar[cap->PBAOffsetBIR.bits.PBA];
        if ( cap->PBAOffsetBIR.bits.PBA+1 < PCI_TYPE0_ADDRESSES )
            pbaNextBar = (PBAR)&((PPCI_CONFIG_GENERAL)cfg)->Bar[cap->PBAOffsetBIR.bits.PBA+1];
    }
    else if ( cfg->HeaderType.Id == PCI_HEADER_PCI_TO_PCI_BRIDGE )
    {
        tableBar = (PBAR)&((PPCI_CONFIG_PCI_BRIDGE)cfg)->Bar[cap->TableOffsetBIR.bits.Table];
        if ( cap->TableOffsetBIR.bits.Table+1 < PCI_TYPE1_ADDRESSES )
            tableNextBar = (PBAR)&((PPCI_CONFIG_PCI_BRIDGE)cfg)->Bar[cap->TableOffsetBIR.bits.Table+1];
        pbaBar = (PBAR)&((PPCI_CONFIG_PCI_BRIDGE)cfg)->Bar[cap->PBAOffsetBIR.bits.PBA];
        if ( cap->PBAOffsetBIR.bits.PBA+1 < PCI_TYPE1_ADDRESSES )
            pbaNextBar = (PBAR)&((PPCI_CONFIG_PCI_BRIDGE)cfg)->Bar[cap->PBAOffsetBIR.bits.PBA+1];
    }

    UINT64 tableAddress = 0;
    if ( tableBar )
    {
        tableAddress = (UINT64)getBarBaseAddress(tableBar, tableNextBar);
        if ( tableAddress )
            tableAddress = tableAddress + ((UINT64)cap->TableOffsetBIR.bits.Offset<<3);
    }
    
    UINT64 pbaAddress = 0;
    if ( pbaBar )
    {
        pbaAddress = (UINT64)getBarBaseAddress(pbaBar, pbaNextBar);
        if ( pbaAddress )
            pbaAddress = pbaAddress + ((UINT64)cap->PBAOffsetBIR.bits.Offset<<3);
    }
    

    // security check if in bar range
    // ...


    //
    // allocate pool

    if ( !tableAddress && !pbaAddress)
        goto clean;

    UINT32 bufferSize = max(tableSize, pbaSize);
    buffer = ExAllocatePoolWithTag(PagedPool, bufferSize, POOL_TAG);
    if ( !buffer )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }

    if ( !tableAddress)
        goto readPBA;

    //
    // read table

    status = readPA(Device, tableAddress, CACHE_TYPE, buffer, bufferSize, READ_PA_MODE_DEF);
    if ( status != 0 )
    {
        EPrint("Reading tableAddress failed! (0x%x)\n", status);
        goto clean;
    }

    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                    "\r\n"
                    "  Table: %p\r\n",
                    (PVOID)tableAddress);

    PMSIX_TABLE_STRUCTURE tableEntry = (PMSIX_TABLE_STRUCTURE)buffer;
    for ( UINT32 i = 0; i < nrOfEntries; i++ )
    {
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                        "  [%u]\r\n"
                        "    MessageAddress: 0x%08x\r\n"
                        "      Zero [1:0]: 0x%x\r\n"
                        "      MA [31:2]: 0x%x (0x%x)\r\n"
                        "    MessageUpperAddress: 0x%08x\r\n"
                        "      MUA [31:0]: 0x%x\r\n"
                        "    MessageData: 0x%08x\r\n"
                        "      MD [31:0]: 0x%x\r\n"
                        "    VectorControl: 0x%08x\r\n"
                        "      Masked [0]: 0x%x\r\n"
                        "      Res_31_01 [31:1]: 0x%x\r\n",
                        i,
                        tableEntry->MessageAddress.AsUINT32,
                            tableEntry->MessageAddress.bits.Zero,
                            tableEntry->MessageAddress.bits.MA, (tableEntry->MessageAddress.bits.MA<<2),
                        tableEntry->MessageUpperAddress.AsUINT32,
                            tableEntry->MessageUpperAddress.bits.MUA,
                        tableEntry->MessageData.AsUINT32,
                            tableEntry->MessageData.bits.MD,
                        tableEntry->VectorControl.AsUINT32,
                            tableEntry->VectorControl.bits.Masked,
                            tableEntry->VectorControl.bits.Res_31_01);
        ++tableEntry;

        if ( logBufferRestSize < 0x100 )
            FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }
    
readPBA:
    if ( !pbaAddress)
        goto clean;

    //
    // read pba entries

    status = readPA(Device, pbaAddress, CACHE_TYPE, buffer, bufferSize, READ_PA_MODE_DEF);
    if ( status != 0 )
    {
        EPrint("Reading pbaAddress failed! (0x%x)\n", status);
        goto clean;
    }
    
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                    "\r\n"
                    "  PBA: %p\r\n",
                    (PVOID)pbaAddress);

    PMSIX_PBA_STRUCTURE pbaEntry = (PMSIX_PBA_STRUCTURE)buffer;
    CHAR bin64Str[BIN_64_STR_BUFFER_SIZE];
    for ( UINT32 i = 0; i < nrOfEntries; i++ )
    {
        uint64ToBin(pbaEntry->PendingBits, bin64Str);

        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                        "  [%u]\r\n"
                        "    Pending Bits: 0x%016llx\r\n"
                        "      Bits [63:0]: %s\r\n",
                        i,
                        pbaEntry->PendingBits,
                        bin64Str);
        ++pbaEntry;

        if ( logBufferRestSize < 0x100 )
            FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }

clean:
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                    "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    if ( buffer )
        ExFreePool(buffer);
    
    FLeave();
    return status;
}

NTSTATUS readCapability(
    _In_ HANDLE Device,
    _In_ PVOID CfgBuffer,
    _In_ ULONG CfgBufferSize,
    _In_ UINT8 Pointer,
    _In_ PLOG Log
)
{
    FEnter();

    NTSTATUS status = 0;

    PCHAR logBufferPtr = Log->Buffer;
    SIZE_T logBufferRestSize = Log->BufferSize;
    
    PPCI_CAPABILITIES_HEADER capHeader = NULL;

    SIZE_T counter = 0;
    SIZE_T maxCaps = CfgBufferSize / sizeof(PCI_CAPABILITIES_HEADER);

    //PVOID buffer = NULL;
    //ULONG bufferSize = 0x100;

    if ( CfgBufferSize < Pointer + sizeof(PCI_CAPABILITIES_HEADER) )
        return STATUS_UNSUCCESSFUL;
    
    //DPrint("bufferSize: 0x%x\n", bufferSize);
    //buffer = ExAllocatePoolWithTag(PagedPool, bufferSize, POOL_TAG);
    //if ( !buffer )
    //{
    //    status = STATUS_NO_MEMORY;
    //    goto clean;
    //}

    capHeader = (PPCI_CAPABILITIES_HEADER)(((PUINT8)CfgBuffer)+Pointer);
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "Capabilities:\r\n");

    while ( capHeader )
    {
        DPrint("CapabilityID: 0x%x\n", capHeader->CapabilityID);
        DPrint("Next: 0x%x\n", capHeader->Next);
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "[%zu]\r\n"
                                "CapabilityID [%02zx]: 0x%x (%s)\r\n"
                                "Next: 0x%x\r\n",
                                counter,
                                (SIZE_T)capHeader-(SIZE_T)CfgBuffer, capHeader->CapabilityID, capabilityIdStr(capHeader->CapabilityID),
                                capHeader->Next);

        switch ( capHeader->CapabilityID )
        {
            case PCI_CAPABILITY_ID_POWER_MANAGEMENT:
            {
                FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

                logPowerManagementCapability(capHeader, CfgBuffer, CfgBufferSize, Log);

                break;
            }
            case PCI_CAPABILITY_ID_MSI:
            {
                FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

                logMSICapability(capHeader, CfgBuffer, CfgBufferSize, Log);

                break;
            }
            case PCI_CAPABILITY_ID_PCIX:
            {
                FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

                logPciXCapability(Device, capHeader, CfgBuffer, CfgBufferSize, Log);

                break;
            }
            case PCI_CAPABILITY_ID_MSIX:
            {
                FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

                logMSIXCapability(Device, capHeader, CfgBuffer, CfgBufferSize, Log);

                break;
            }
            default:
                ;
                ULONG capSize = 0;
                if ( capHeader->Next )
                {
                    // sometimes they are unordered
                    if ( (SIZE_T)CfgBuffer + capHeader->Next > (SIZE_T)capHeader )
                        capSize = (ULONG)( (SIZE_T)CfgBuffer + capHeader->Next - (SIZE_T)capHeader );
                    else
                        capSize = 0x8;
                }
                else
                {
                    capSize = (ULONG)( (SIZE_T)CfgBuffer + PCI_CONFIG_SIZE - (SIZE_T)capHeader);
                }
                DPrint("capSize: 0x%x (may be wrong: next - current)\n", capSize);
                RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                    "(Size of printed bytes may be incorrect)\r\n");
                LogBytes("", "", capHeader, capSize, 0, TRUE, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
                
                RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                    "\r\n");
                break;
        }

        if ( logBufferRestSize < 0x100 )
            FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
        
        counter++;
        UINT8 next = capHeader->Next & ~0x3;

        if ( next && next < CfgBufferSize && counter < maxCaps )
            capHeader = (PPCI_CAPABILITIES_HEADER)(((SIZE_T)CfgBuffer) + next);
        else
            break;

        if ( logBufferRestSize < 0x100 )
            FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }

//clean:
    //if ( buffer )
    //    ExFreePool(buffer);

    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    
    FLeave();
    return status;
}

NTSTATUS logCapabilities(
    _In_ HANDLE Device,
    _In_ PVOID CfgBuffer,
    _In_ ULONG CfgBufferSize,
    _In_ UINT32 Flags,
    _In_ PLOG Log
)
{
    NTSTATUS status = 0;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;
    
    (Flags);

    if ( CfgBufferSize < 0x40 )
        return STATUS_UNSUCCESSFUL;
    
    FEnter();

    PPCI_CONFIG pciConfig = (PPCI_CONFIG)CfgBuffer;

    if ( ! pciConfig->Status.CapabilitiesList )
    {
        status = STATUS_SUCCESS;
        goto clean;
    }


    if ( pciConfig->HeaderType.Id == PCI_HEADER_GENERAL_DEVICE )
    {
        PPCI_CONFIG_GENERAL pciCfgGeneral = (PPCI_CONFIG_GENERAL)CfgBuffer;
        if ( pciCfgGeneral->CapabilitiesPointer )
        {
            DPrint("  CapabilitiesPointer: 0x%x\n", pciCfgGeneral->CapabilitiesPointer);
            status = readCapability(Device, CfgBuffer, CfgBufferSize, pciCfgGeneral->CapabilitiesPointer, Log);
        }
    }
    else if ( pciConfig->HeaderType.Id == PCI_HEADER_PCI_TO_PCI_BRIDGE )
    {
        PPCI_CONFIG_PCI_BRIDGE pciCfgPciBridge = (PPCI_CONFIG_PCI_BRIDGE)CfgBuffer;
        if ( pciCfgPciBridge->CapabilitiesPointer )
        {
            DPrint("  CapabilitiesPointer: 0x%x\n", pciCfgPciBridge->CapabilitiesPointer);
            status = readCapability(Device, CfgBuffer, CfgBufferSize, pciCfgPciBridge->CapabilitiesPointer, Log);
        }
    }
    else if ( pciConfig->HeaderType.Id == PCI_HEADER_PCI_TO_CARDBUS_BRIDGE
              && CfgBufferSize >= 0x48 )
    {
        PPCI_CONFIG_PCI_CB_BRIDGE pciCfgPciCbBridge = (PPCI_CONFIG_PCI_CB_BRIDGE)CfgBuffer;
        if ( pciCfgPciCbBridge->OffsetOfCapabilitiesList )
        {
            DPrint("  CapabilitiesPointer: 0x%x\n", pciCfgPciCbBridge->OffsetOfCapabilitiesList);
            status = readCapability(Device, CfgBuffer, CfgBufferSize, pciCfgPciCbBridge->OffsetOfCapabilitiesList, Log);
        }
    }

    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                                "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    
clean:
    FLeave();
    return status;
}
