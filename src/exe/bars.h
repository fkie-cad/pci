#pragma once

//
// some bar dependend function
// like reading them 
// or calculate them sizes
// 
// bar size calculation:
// 
// To determine the amount of address space needed by a PCI device, 
// 1) save the original value of the BAR, 
// 2a) check decode in Command register ??
// 2b) disable decode in Command register
// 3) write a value of all 1's (0xFFFFFFFF) to the register, 
// 4) then read it back. 
//   The value won't be 0xFFFFFFFF, because not all bits are writeable.
// 5) mask the information bits, I/O BAR: bit 0, Memory BAR: bit 0,1,2,3
// 6) performing a bitwise NOT ('~' in C), 
// 7) and incrementing the value by 1. 
// 8) The original value of the BAR should then be restored. 
// 9) reenable decode in Command register
//
// linux source code : https://elixir.bootlin.com/linux/v5.18-rc4/source/drivers/pci/probe.c#L198
//


NTSTATUS readBar(
    _In_ HANDLE Device,
    _In_ UINT64 PciExBarAddress,
    _In_ PBDF BDF,
    _In_ UINT32 BarsStartOffset,
    _In_ UINT32 BarId,
    _In_ PBAR Bar,
    _In_opt_ PBAR NextBar,
    _Inout_ PVOID *BarBuffer, 
    _Inout_ UINT64 *BarBufferSize,
    _Out_ UINT64 *BarSize
);

NTSTATUS readMemoryBar(
    _In_ HANDLE Device, 
    _In_ UINT64 Address,
    _Inout_ PVOID Buffer,
    _In_ ULONG Size
);

NTSTATUS readPortIoBar(
    _In_ HANDLE Device, 
    _In_ UINT16 BarPort,
    _Inout_ PVOID Buffer,
    _In_ ULONG Size
);


#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
NTSTATUS calculateBarSizeMMioMapped(
    _In_ HANDLE Device,
    _In_ UINT64 PciExBarAddress,
    _In_ PBDF BDF,
    _In_ UINT32 Bar0Offset,
    _Out_ PUINT64 BarSize
);
#else
NTSTATUS calculateBarSizePA(
    _In_ HANDLE Device,
    _In_ UINT64 PciExBarAddress,
    _In_ PBDF BDF,
    _In_ UINT32 BarOffset,
    _Out_ PUINT64 BarSize
);
#endif

NTSTATUS calculateBarSizePortIo(
    _In_ HANDLE Device,
    _In_ PBDF BDF,
    _In_ UINT32 BarOffset,
    _Out_ PUINT64 BarSize
);



/**
 * Realloc bar buffer, if too small.
 */
FORCEINLINE
NTSTATUS getBarBuffer(
    _In_ UINT64 BarSize, 
    _Inout_ PVOID *Buffer, 
    _Inout_ UINT64 *BufferSize
)
{
    FEnter();

    NTSTATUS status = 0;

    if ( !Buffer || !BufferSize )
        return STATUS_INVALID_PARAMETER;

    // realloc if too small
    if ( (*Buffer) && BarSize > (*BufferSize) )
    {
        NtFreeVirtualMemory(
                (HANDLE)-1,
                Buffer,
                BufferSize,
                MEM_RELEASE
            );
// warning C6001: Using uninitialized memory 'Buffer'
DISABLE_WARNING(6001)
        *Buffer = NULL;
DEFAULT_WARNING(6001)
    }
    if ( !(*Buffer) )
    {
        *BufferSize = BarSize;

        status = NtAllocateVirtualMemory(
                (HANDLE)-1,
                Buffer,
                0,
                BufferSize,
                MEM_COMMIT | MEM_RESERVE,
                PAGE_READWRITE
            );
    }
    
    FLeave();
    return status;
}

NTSTATUS readBar(
    _In_ HANDLE Device,
    _In_ UINT64 PciExBarAddress,
    _In_ PBDF BDF,
    _In_ UINT32 BarsStartOffset,
    _In_ UINT32 BarId,
    _In_ PBAR Bar,
    _In_opt_ PBAR NextBar,
    _Inout_ PVOID *BarBuffer, 
    _Inout_ UINT64 *BarBufferSize,
    _Out_ UINT64 *BarSize
)
{
    FEnter();

    NTSTATUS status = 0;

    UINT64 baseAddress = 0;
    UINT64 barSize = 0;

    *BarSize = 0;

    if ( !Bar->Value )
    {
        DPrint("No bar here!\n");
        goto clean;
    }

    if ( PciExBarAddress )
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
        status = calculateBarSizeMMioMapped(Device, PciExBarAddress, BDF, BarsStartOffset+BarId*4, &barSize);
#else
        status = calculateBarSizePA(Device, PciExBarAddress, BDF, BarsStartOffset+BarId*4, &barSize);
#endif
    else
        status = calculateBarSizePortIo(Device, BDF, BarsStartOffset+BarId*4, &barSize);
    if ( status != 0 )
    {
        EPrint("calculateBarSize() failed! (0x%x)", status);
        goto clean;
    }

    DPrint(" type: %s\n", BAR_TYPE_STR(Bar));
    DPrint(" address: %s\n", BAR_ADDRESS_WIDTH(Bar));
    DPrint(" barSize: 0x%llx\n", barSize);
    
    baseAddress = (UINT64)getBarBaseAddress(Bar, NextBar);

    printf("Bar[%u] (%s, %s, 0x%llx, 0x%llx)\r\n", 
            BarId,
            BAR_TYPE_STR(Bar),
            BAR_ADDRESS_WIDTH(Bar),
            baseAddress,
            barSize);

    if ( !baseAddress || !barSize )
        goto clean;
                
    status = getBarBuffer(barSize, BarBuffer, BarBufferSize);
    if ( status != 0)
    {
        EPrint("getBarBuffer failed! (0x%x)\n", status);
        goto clean;
    }

    if ( Bar->BarType==BAR_TYPE_MEMORY_SPACE )
    {
        status = readMemoryBar(Device, baseAddress, *BarBuffer, (UINT32)barSize);
        if ( status != 0)
        {
            EPrint("Reading bar %u failed! (0x%x)\n", BarId, status);
            goto clean;
        }
    }
    else
    {
        status = readPortIoBar(Device, *(PUINT16)&baseAddress, *BarBuffer, (UINT32)barSize);
        if ( status != 0)
        {
            EPrint("Reading bar %u failed! (0x%x)\n", BarId, status);
            goto clean;
        }
    }

    *BarSize = barSize;

clean:

    FLeave();
    return status;
}

NTSTATUS readMemoryBar(
    _In_ HANDLE Device, 
    _In_ UINT64 BarAddress,
    _Inout_ PVOID Buffer,
    _In_ ULONG BarSize
)
{
    FEnter();

    NTSTATUS status = 0;

    // at best it's 4 byte aligned
    status = readPA(Device, BarAddress, CACHE_TYPE, Buffer, BarSize, READ_PA_MODE_CHUNKED_4);
    //status = readPAMmIoMapped4(Device, BarAddress, CACHE_TYPE, Buffer, BarSize);
    if ( status != 0 )
    {
        EPrint("DeviceIO request failed! (0x%x)\n", status);
        goto clean;
    }
#ifdef DEBUG_PRINT_MEM
    DPrint("bar data:\n");
    DPrintMemCol8(Buffer, BarSize, BarAddress);
#endif

clean:
    
    FLeave();
    return status;
}

NTSTATUS readPortIoBar(
    _In_ HANDLE Device, 
    _In_ UINT16 BarPort,
    _Inout_ PVOID Buffer,
    _In_ ULONG BarSize
)
{
    FEnter();

    NTSTATUS status = 0;

    if ( BarSize % 4 == 0 )
    {
        status = in32B(Device, BarPort, Buffer, BarSize);
    }
    else if ( BarSize % 2 == 0 )
    {
        status = in16B(Device, BarPort, Buffer, BarSize);
    }
    else
    {
        status = in8B(Device, BarPort, Buffer, BarSize);
    }

#ifdef DEBUG_PRINT_MEM
    DPrint("bar data:\n");
    DPrintMemCol8(Buffer, BarSize, BarPort);
#endif

//clean:

    FLeave();
    return status;
}


#ifndef RW_PRIM_MEM_MAPPING_SUPPORTED

static NTSTATUS probeBarSizePa(
    HANDLE Device, 
    UINT64 BarIndex, 
    UINT32 ReadPageFlags, 
    UINT32 WritePageFlags,
    UINT32 *OriginalBarValue, 
    UINT32 *BarSize)
{
    UINT32 initValue = 0xFFFFFFFF;
    NTSTATUS status;

    *BarSize = 0xFFFFFFFF;
    status = readPA(Device, BarIndex, ReadPageFlags, OriginalBarValue, 4, READ_PA_MODE_4);
    if ( status || *OriginalBarValue == 0 )
        return status;

    status = writePA(Device, BarIndex, WritePageFlags, &initValue, 4, WRITE_PA_MODE_4);
    if ( status )
        return status;

    status = readPA(Device, BarIndex, ReadPageFlags, BarSize, 4, READ_PA_MODE_4);

    NTSTATUS rstatus = writePA(Device, BarIndex, WritePageFlags, OriginalBarValue, 4, WRITE_PA_MODE_4);
    return status ? status : rstatus;
}

//
// Calculate bar size using ioctl 4 bytes reads and writes
// 
// See header of this file to see in an abstract level how the calculation done.
// 
NTSTATUS calculateBarSizePA(
    _In_ HANDLE Device,
    _In_ UINT64 PciExBarAddress,
    _In_ PBDF BDF,
    _In_ UINT32 Bar0Offset,
    _Out_ PUINT64 BarSize
)
{
    FEnter();

    NTSTATUS status = 0;
    UINT64 barSize = 0; // overall calculated (if 64-bit combined) bar size
    UINT32 bar0Size = 0xFFFFFFFF;
    UINT64 cmdIndex = 0;
    UINT64 bar0Index = 0;
    UINT64 bar1Index = 0;
    UINT16 cmdOffset = (UINT16)offsetof(PCI_CONFIG, Command);
    UINT32 bar0OriginalValue = 0;
    UINT32 bar1OriginalValue = 0;
    //BOOLEAN bar0NeedsRestore = FALSE;
    //BOOLEAN bar1NeedsRestore = FALSE;
    UINT32 command = 0;
    UINT32 origCommand = 0;
    UINT32 mask = 0;
    UINT32 waitCount = 0;
    UINT32 maxWait = 0x20;
    //UINT32 initValue = 0xFFFF'FFFF;

    UINT32 readPageFlags = CACHE_TYPE;
    UINT32 writePageFlags = CACHE_TYPE;
    
    BOOLEAN decodeDisabled = 0;

    *BarSize = 0;

    // (2a/b)
    cmdIndex = PCI_CFG_MM_ADDR(PciExBarAddress, BDF->Bus, BDF->Device, BDF->Function, cmdOffset);
    //if (!dev->mmio_always_on) {
        //pio_Read_32_32(PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, cmdIndex, &command);
        status = readPA(Device, cmdIndex, readPageFlags, &command, 4, READ_PA_MODE_4);
        if ( status != 0 )
        {
            EPrint("readPA failed! (0x%x)\n", status);
            goto clean;
        }
        while ( !(command & PCI_COMMAND_DECODE_ENABLE) )
        {
            waitCount++;
            if ( waitCount >= maxWait )
                break;
                //return STATUS_UNSUCCESSFUL;
            Sleep(1);
            //pio_Read_32_32(PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, cmdIndex, &command);
            status = readPA(Device, cmdIndex, readPageFlags, &command, 4, READ_PA_MODE_4);
            if ( status != 0 )
            {
                EPrint("readPA failed! (0x%x)\n", status);
                goto clean;
            }
        }
        if ( command & PCI_COMMAND_DECODE_ENABLE )
        {
            origCommand = command;
            //pio_Write_32_32(PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, cmdIndex, (command & ~PCI_COMMAND_DECODE_ENABLE));
            command &= ~PCI_COMMAND_DECODE_ENABLE;
            status = writePA(Device, cmdIndex, writePageFlags, &command, 4, WRITE_PA_MODE_4);
            if ( status != 0 )
            {
                EPrint("writePA failed! (0x%x)\n", status);
                goto clean;
            }
            decodeDisabled = 1;
        }
    //}

    // (1)
    bar0Index = PCI_CFG_MM_ADDR(PciExBarAddress, BDF->Bus, BDF->Device, BDF->Function, Bar0Offset);
    status = probeBarSizePa(Device, bar0Index, readPageFlags, writePageFlags, &bar0OriginalValue, &bar0Size);
    if ( status != 0 )
    {
        EPrint("Reading bar 0 failed! (0x%x)\n", status);
        goto clean;
    }

    mask = (((PBAR)&bar0OriginalValue)->BarType==BAR_TYPE_MEMORY_SPACE)
            ? ~0b1111
            : ~0b11;
    if ( bar0Size == 0xFFFF'FFFF )
        goto clean;

    bar0Size = bar0Size & mask; // (5)
    bar0Size = ~bar0Size; // (6)
    bar0Size = bar0Size + 1; // (7)
    if (((PBAR)&bar0OriginalValue)->BarType==BAR_TYPE_IO_SPACE)
        bar0Size = bar0Size & IO_SPACE_LIMIT; // (5)
    DPrint("barSize1: 0x%x\n", bar0Size);

    barSize = bar0Size;

    if ( IS_64BIT_BAR(&bar0OriginalValue) )
    {
        UINT32 bar1Size = 0xFFFF'FFFF;
        UINT32 bar1Offset = Bar0Offset+4;
        
        bar1Index = PCI_CFG_MM_ADDR(PciExBarAddress, BDF->Bus, BDF->Device, BDF->Function, bar1Offset);
        status = probeBarSizePa(Device, bar1Index, readPageFlags, writePageFlags, &bar1OriginalValue, &bar1Size);
        if ( status != 0 )
        {
            EPrint("Reading bar 1 failed! (0x%x)\n", status);
            goto clean;
        }
        
        if ( bar1Size != 0xFFFF'FFFF )
        {
            bar1Size = bar1Size & 0xFFFF'FFFF; // (5) => & FFFF'FFF0
            bar1Size = ~bar1Size; // (6)
            bar1Size = bar1Size + 1; // (7)
            barSize |= ((UINT64)bar1Size << 0x20);
        //    sz64 |= ((u64)sz << 32);
        //    mask64 |= ((u64)~0 << 32);
        }
        DPrint("bar1Size: 0x%x\n", bar1Size);
    }

    *BarSize = barSize;

clean:
    // (9)
    //if ( !dev->mmio_always_on )
    {
        if ( decodeDisabled )
        {
            //pio_Write_32_32(PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, cmdIndex, command);
            NTSTATUS status2 = writePA(Device, cmdIndex, writePageFlags, &origCommand, 4, WRITE_PA_MODE_4);
            if ( status2 != 0 )
            {
                EPrint("writePA origCommand failed! (0x%x)\n", status2);
            }
        }
    }
    
    FLeave();
    return status;
}
#endif

#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
//
// Calculate bar size using memory mapped to user space
// 
// Volatile is needed for anti optimization, i.e. real reads and writes, no caching
// Since the "init" value is written to the bar and then read again, 
//   the compiler might optimize this and just don't read the value again,
//   because it thinks it has to be the init value, which is wrong in this case.
// 
// See header of this file to see in an abstract level how the calculation done.
// 
NTSTATUS calculateBarSizeMMioMapped(
    _In_ HANDLE Device,
    _In_ UINT64 PciExBarAddress,
    _In_ PBDF BDF,
    _In_ UINT32 Bar0Offset,
    _Out_ PUINT64 BarSize
)
{
    FEnter();

    NTSTATUS status = 0;
    UINT64 barSize = 0; // overall calculated (if 64-bit combined) bar size
    UINT32 bar0Size = 0xFFFFFFFF;
    
    UINT16 cmdOffset = (UINT16)offsetof(PCI_CONFIG, Command);
    UINT32 bar0Offset = Bar0Offset;
    UINT32 bar1Offset = Bar0Offset + 4;
    
    BOOLEAN isMapped = FALSE;
    UINT64 mappedCfgAddr = 0;

    volatile PUINT32 cmdReg = 0;
    volatile PUINT32 bar0Reg = 0;
    volatile PUINT32 bar1Reg = 0;

    UINT32 bar0OriginalValue = 0;
    
    volatile UINT32 command = 0;
    UINT32 origCommand = 0;
    BOOLEAN decodeDisabled = 0;
    
    UINT32 mask = 0;
    UINT32 waitCount = 0;
    UINT32 maxWait = 0x20;
    
    UINT32 initValue = 0xFFFF'FFFF;

    PVOID cfgBase = (PVOID)PCI_CFG_MM_ADDR(PciExBarAddress, BDF->Bus, BDF->Device, BDF->Function, 0);;
    
    *BarSize = 0;
    
    //
    // map config
    UINT8 mapOutParams[MEM_MAP_OUT_SIZE] = { 0 };
    status = mapMemory(Device, cfgBase, 0x100, (PVOID*)&mappedCfgAddr, mapOutParams, MEM_MAP_OUT_SIZE);
    if ( status != 0 )
    {
        EPrint("mapMemory request failed! (0x%x)\n", status);
        goto clean;
    }
    isMapped = TRUE;

    cmdReg = (PUINT32)(mappedCfgAddr + cmdOffset);
    bar0Reg = (PUINT32)(mappedCfgAddr + bar0Offset);
    bar1Reg = (PUINT32)(mappedCfgAddr + bar1Offset);

    //
    // (2a/b)
    // 
    //if (!dev->mmio_always_on) {
        command = *cmdReg;
        DPrint("command: 0x%x\n", command);
        while ( !(command & PCI_COMMAND_DECODE_ENABLE) )
        {
            waitCount++;
            if ( waitCount >= maxWait )
                break;
                //return STATUS_UNSUCCESSFUL;
            Sleep(1);
            command = *cmdReg;
        }
        if ( command & PCI_COMMAND_DECODE_ENABLE )
        {
            origCommand = command;
            command &= ~PCI_COMMAND_DECODE_ENABLE;
            *cmdReg = command;

            decodeDisabled = 1;
        }
    //}

    // (1)
    bar0OriginalValue = *bar0Reg;
    DPrint("bar0OriginalValue: 0x%x\n", bar0OriginalValue);

    if ( !bar0OriginalValue )
        goto clean;

    // (3)
    *bar0Reg = initValue;
    
    // (4)
    bar0Size = *bar0Reg;
    DPrint("bar0Size: 0x%x\n", bar0Size);

    // (8)
    *bar0Reg = bar0OriginalValue;

    mask = (((PBAR)&bar0OriginalValue)->BarType==BAR_TYPE_MEMORY_SPACE)
            ? ~0b1111
            : ~0b11;
    if ( bar0Size == initValue )
        goto clean;

    bar0Size = bar0Size & mask; // (5)
    bar0Size = ~bar0Size; // (6)
    bar0Size = bar0Size + 1; // (7)
    if (((PBAR)&bar0OriginalValue)->BarType==BAR_TYPE_IO_SPACE)
        bar0Size = bar0Size & IO_SPACE_LIMIT; // (5)
    DPrint("bar0Size: 0x%x\n", bar0Size);

    barSize = bar0Size;

    if ( IS_64BIT_BAR(&bar0OriginalValue) )
    {
        UINT32 bar1Size = 0xFFFF'FFFF;
        UINT32 bar1OriginalValue = 0;
        
        bar1OriginalValue = *bar1Reg;
        DPrint("bar1OriginalValue: 0x%x\n", bar1OriginalValue);
        // write -1
        *bar1Reg = initValue;
        // read
        bar1Size = *bar1Reg;
        // restore
        *bar1Reg = bar1OriginalValue;
        
        if ( bar1Size != initValue )
        {
            bar1Size = bar1Size & 0xFFFF'FFFF; // (5) => & FFFF'FFF0
            bar1Size = ~bar1Size; // (6)
            bar1Size = bar1Size + 1; // (7)
            barSize |= ((UINT64)bar1Size << 0x20);
        //    sz64 |= ((u64)sz << 32);
        //    mask64 |= ((u64)~0 << 32);
        }
        DPrint("bar1Size: 0x%x\n", bar1Size);
    }

    *BarSize = barSize;

clean:
    // (9)
    //if ( !dev->mmio_always_on )
    {
        if ( decodeDisabled )
        {
            *cmdReg = origCommand;
        }
    }
    
    if ( isMapped )
    {
        unmapMemory(Device, (PVOID)mappedCfgAddr, mapOutParams, MEM_MAP_OUT_SIZE);
    }
    
    FLeave();
    return status;
}
#endif

//
// Calculate bar size using port io.
// 
// See header of this file to see in an abstract level how the calculation done.
// 
NTSTATUS calculateBarSizePortIo(
    _In_ HANDLE Device,
    _In_ PBDF BDF,
    _In_ UINT32 BarOffset,
    _Out_ PUINT64 BarSize
)
{
    FEnter();

    NTSTATUS status = 0;
    UINT64 barSize = 0;
    UINT32 barSize1 = 0xFFFFFFFF;
    UINT32 cmdIndex = 0;
    UINT32 barIndex = 0;
    UINT16 cmdOffset = (UINT16)offsetof(PCI_CONFIG, Command);
    UINT32 originalValue = 0;
    UINT32 command = 0;
    BOOLEAN decodeDisabled = 0;
    UINT32 mask = 0;
    UINT32 waitCount = 0;
    UINT32 maxWait = 0x100;

    *BarSize = 0;

    // (2a/b)
    cmdIndex = PCI_CFG_PORT_IO_ADDR(BDF->Bus, BDF->Device, BDF->Function, cmdOffset);
    //if (!dev->mmio_always_on) {
        outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, cmdIndex, &command);
        while ( !(command & PCI_COMMAND_DECODE_ENABLE) )
        {
            waitCount++;
            if ( waitCount >= maxWait )
            {
                EPrint("breaking wait count!\n");
                break;
            }
            //return STATUS_UNSUCCESSFUL;
            Sleep(1);
            outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, cmdIndex, &command);
        }
        if ( command & PCI_COMMAND_DECODE_ENABLE )
        {
            decodeDisabled = 1;
            outOut32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, cmdIndex, (command & ~PCI_COMMAND_DECODE_ENABLE));
        }
    //}

    // (1)
    barIndex = PCI_CFG_PORT_IO_ADDR(BDF->Bus, BDF->Device, BDF->Function, BarOffset);
    outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, barIndex, &originalValue);
    DPrint("OriginalValue: 0x%x\n", originalValue);

    if ( !originalValue )
        goto clean;

    // (3)
    //barIndex = PCI_CFG_PORT_IO_ADDR(BDF->Bus, BDF->Device, BDF->Function, BarOffset);
    status = outOut32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, barIndex, 0xFFFF'FFFF);
    if ( status != 0 ) goto clean;
    
    // (4)
    status = outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, barIndex, &barSize1);
    if ( status != 0 ) goto clean;

    // (8)
    status = outOut32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, barIndex, originalValue);
    if ( status != 0 ) goto clean;
    
    mask = (((PBAR)&originalValue)->BarType==BAR_TYPE_MEMORY_SPACE)
            ? ~0b1111
            : ~0b11;
    if ( barSize1 == 0xFFFF'FFFF )
        goto clean;

    barSize1 = barSize1 & mask; // (5)
    barSize1 = ~barSize1; // (6)
    barSize1 = barSize1 + 1; // (7)
    if (((PBAR)&originalValue)->BarType==BAR_TYPE_IO_SPACE)
        barSize1 = barSize1 & IO_SPACE_LIMIT; // (5)
    DPrint("barSize1: 0x%x\n", barSize1);

    barSize = barSize1;

    if ( IS_64BIT_BAR(&originalValue) )
    {
        UINT32 barSize2 = 0xFFFF'FFFF;
        UINT32 originalBar1Value = 0;
        UINT32 barOffset2 = BarOffset+4;

        barIndex = PCI_CFG_PORT_IO_ADDR(BDF->Bus, BDF->Device, BDF->Function, barOffset2);
        // read
        outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, barIndex, &originalBar1Value);
        DPrint("originalBar1Value: 0x%x\n", originalBar1Value);

        // write -1
        outOut32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, barIndex, 0xFFFFFFFF);
        // read
        outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, barIndex, &barSize2);
        // restore
        outOut32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, barIndex, originalBar1Value);
        
        if ( barSize2 != 0xFFFF'FFFF )
        {
            barSize2 = barSize2 & 0xFFFF'FFFF; // (5) => & FFFF'FFF0
            barSize2 = ~barSize2; // (6)
            barSize2 = barSize2 + 1; // (7)
            barSize |= ((UINT64)barSize2 << 0x20);
        //    sz64 |= ((u64)sz << 32);
        //    mask64 |= ((u64)~0 << 32);
        }
        DPrint("barSize2: 0x%x\n", barSize2);
    }

    *BarSize = barSize;

clean:
    // (9)
    //if ( !dev->mmio_always_on )
    {
        if ( decodeDisabled && command & PCI_COMMAND_DECODE_ENABLE )
        {
            outOut32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, cmdIndex, command);
        }
    }
    
    FLeave();
    return status;
}
