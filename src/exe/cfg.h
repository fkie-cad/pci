#pragma once


//
// Get PciExBarAddress via device 00:00:00.
// Port io needed.
//
// PCIEXBAR is at offset 0x60 of device 00:00:00.
// 
// struct {
//    UINT64 PCIEXBAREN:1; // 0 0h RW PCIEXBAREN: 
//                //0: The PCIEXBAR register is disabled. Memory read and write transactions proceed s if there were no PCIEXBAR register. PCIEXBAR bits 38:26 are R/W with no functionality behind them.
//                //1: The PCIEXBAR register is enabled. Memory read and write transactions whose address bits 38:26 match PCIEXBAR will be translated to configuration reads and writes within the Uncore. These Translated cycles are routed as shown in the above table.
//    UINT64 LENGTH:2; // 2:1 0h RW LENGTH: This field describes the length of this region.
//                //00: 256MB (buses 0-255). Bits 38:28 are decoded in the PCI Express Base Address Field. 
//                //01: 128MB (buses 0-127). Bits 38:27 are decoded in the PCI Express Base Address Field. 
//                //10: 64MB (buses 0-63). Bits 38:26 are decoded in the PCI Express Base Address Field.
//                //11: Reserved.
//                //This register is locked by Intel TXT. 
//    UINT64 Reserved1:23; // 25:3 0h RO Reserved (RSVD): Reserved. 
//    UINT64 ADMSK64:1; // 26 0h RW_V ADMSK64: This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register. 
//    UINT64 ADMSK128:1; // 27 0h RW_V ADMSK128: This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register. 
//    UINT64 PCIEXBAR:11; // 38:28 0h RW PCIEXBAR: This field corresponds to bits 38 to 28 of the base address for PCI Express enhanced configuration space. 
//                        // BIOS will program this register resulting in a base address for a contiguous memory address space. 
//                        // The size of the range is defined by bits [2:1] of this register.
//                        // This Base address shall be assigned on a boundary consistent with the number of buses (defined by the Length field in this register) above TOLUD and still within the 39-bit addressable memory space. 
//                        // The address bits decoded depend on the length of the region defined by this register.
//                        // This register is locked by Intel TXT.
//                        // The address used to access the PCI Express configuration space for a specific device can be determined as follows: 
//                        //   PCI Express Base Address + Bus Number * 1MB + Device Number * 32KB + Function Number * 4KB. 
//                        // This address is the beginning of the 4KB space that contains both the PCI compatible configuration space and the PCI Express extended configuration space. 
//    UINT64 RSVD:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
//} PCIEXBAR; // 60h PCI Express Register Range Base Address (PCIEXBAR). This is the base address for the PCI Express configuration space. This window of addresses contains the 4KB of configuration space for each PCI Express device that can potentially be part of the PCI Express Hierarchy associated with the Uncore. There is no actual physical memory within this window of up to 256MB that can be addressed. The actual size of this range is determined by a field in this register 
//
NTSTATUS getPciExBarValuePortIo(
    _In_ HANDLE Device,
    _Out_ PUINT64 PciExBarAddress,
    _Out_ PUINT64 Size
)
{
    NTSTATUS status = 0;
    
    LARGE_INTEGER value64 = {0};
    BDF bdf = { 0, 0, 0 };
    UINT8 offset = 0x60;

    FEnter();

    *PciExBarAddress = 0;
    *Size = 0;
    
    UINT32 cfgAddress = PCI_CFG_PORT_IO_ADDR(bdf.Bus, bdf.Device, bdf.Function, offset);
    status = outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, cfgAddress, (PUINT32)&value64.LowPart);
    if ( status != 0 )
    {
        EPrint("outIn32 0x%x:0x%x failed! (0x%x)\n", PCI_CONFIG_COMMAND_PORT, cfgAddress, status);
        goto clean;
    }

    
    cfgAddress = PCI_CFG_PORT_IO_ADDR(bdf.Bus, bdf.Device, bdf.Function, offset+4);
    status = outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, cfgAddress, (PUINT32)&value64.HighPart);
    if ( status != 0 )
    {
        EPrint("outIn32 0x%x:0x%x failed! (0x%x)\n", PCI_CONFIG_COMMAND_PORT, cfgAddress, status);
        goto clean;
    }


    PPCI_EX_BAR xbar = (PPCI_EX_BAR)&value64.QuadPart;
    if ( xbar->PCIEXBAREN )
    {
        switch ( xbar->LENGTH )
        {
            case 0: 
                *PciExBarAddress = value64.QuadPart & 0x3fff0000000; 
                *Size = 256*0x10'0000; // 256 mb
                break;
            case 1: 
                *PciExBarAddress = value64.QuadPart & 0x3fff8000000;
                *Size = 128*0x10'0000; // 128 mb
                break;
            case 2: 
                *PciExBarAddress = value64.QuadPart & 0x3fffc000000;
                *Size = 64*0x10'0000; // 64 mb
                break;
            case 3: 
                *PciExBarAddress = value64.QuadPart & 0x3ffe0000000;
                *Size = 512*0x10'0000; // 512 mb
                break;
            case 4: 
                *PciExBarAddress = value64.QuadPart & 0x3ffc0000000;
                *Size = 1024*0x10'0000; // 1024 mb
                break;
            case 5: 
                *PciExBarAddress = value64.QuadPart & 0x3ff80000000;
                *Size = 2048ull*0x10'0000; // 2048 mb
                break;
            case 6: 
                *PciExBarAddress = value64.QuadPart & 0x3ff00000000;
                *Size = 4096ull*0x10'0000; // 4096 mb
                break;
            default:
                status = STATUS_UNSUCCESSFUL;
                break;
        }
    }

clean:

    FLeave();
    return status;
}

//
// Get PciExBarAddress via ACPI MCFG table.
// No port io needed.
// Using NtQuerySystemInformation.
//
NTSTATUS getPciExBarValueMCFG(
    _In_ HANDLE Device,
    _Out_ PUINT64 PciExBarAddress,
    _Out_ PUINT64 Size
)
{
    FEnter();
    (Device);
    NTSTATUS status = 0;

    ULONG bytesReturned = 0;
    PSYSTEM_FIRMWARE_TABLE_INFORMATION info = NULL;
    UINT32 infoSize = 0x100;

    *PciExBarAddress = 0;
    *Size = 0;

    info = (PSYSTEM_FIRMWARE_TABLE_INFORMATION)malloc(infoSize);
    if ( !info )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }
    RtlZeroMemory(info, infoSize);

    info->ProviderSignature = 'ACPI'; // big endian
    info->TableID = 'GFCM'; // little endian
    info->Action = SystemFirmwareTable_Get;
    info->TableBufferLength = infoSize - FIELD_OFFSET(SYSTEM_FIRMWARE_TABLE_INFORMATION, TableBuffer);

    status = NtQuerySystemInformation(SystemFirmwareTableInformation, info, infoSize, &bytesReturned);
    if ( status != 0 )
    {
        EPrint("NtQuerySystemInformation failed! (0x%x)\n", status);
        goto clean;
    }

    DPrintMemCol8(info, bytesReturned, 0);
    // systemfirmware table should contain a whole MCFG in its TableBuffer
    if ( bytesReturned < FIELD_OFFSET(SYSTEM_FIRMWARE_TABLE_INFORMATION, TableBuffer) + sizeof(MCFG) )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("NtQuerySystemInformation returned less then expected data! (0x%x)\n", status);
        goto clean;
    }
    
    PMCFG dt = (PMCFG)info->TableBuffer;
    // dt->Header.Length should be at least of size of MCFG + one CSBAA
    if ( dt->Header.Length < sizeof(DESCRIPTION_HEADER) + sizeof(dt->Reserved) + sizeof(CSBAA) )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("MCFG length incorrect! (0x%x)\n", status);
        goto clean;
    }
    ULONG nrOfEntries = (dt->Header.Length - sizeof(DESCRIPTION_HEADER) - sizeof(dt->Reserved)) / sizeof(CSBAA);

    if ( !nrOfEntries )
    {
        status = STATUS_UNSUCCESSFUL;
        goto clean;
    }
    
    DPrint("BaseAddress: %p\n", dt->csbaa[0].BaseAddress);
    DPrint("SegmentGroupNr: 0x%x\n", dt->csbaa[0].SegmentGroupNr);
    DPrint("StartBusNr: 0x%x\n", dt->csbaa[0].StartBusNr);
    DPrint("EndBusNr: 0x%x\n", dt->csbaa[0].EndBusNr);
    DPrint("Reserved: 0x%x\n", dt->csbaa[0].Reserved);

    *PciExBarAddress = (UINT64)dt->csbaa[0].BaseAddress;
    *Size = (dt->csbaa[0].EndBusNr+1) * 0x10'0000;

clean:
    if ( info )
        free(info);

    FLeave();
    return status;
}
