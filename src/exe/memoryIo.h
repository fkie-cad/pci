#pragma once

//
// Basic functions to read and write kernel memory and ports
// The currently used rw driver is memio.sys
// If another rw driver is used this is the main file that has to be adjusted to the new driver needs.
// There may be some other parts located in other files, but actually should not and if found or reported are tried to be removed.
// 
// Mandatory functions to implement are
// - readPA
// - readPAWtf
// - writePA
// - in(B)(8|16|32)
// - out(8|16|32)
// 
// Mandatory but not necessarily subject to change
// - outIn(8|16|32)
// - outOut32
//
// In here readPa, readPAWtf and writePa are wrapper for more specific functions.
// It's not necessary to do so and the whole physical reading/writing logic can be placed directly in those three functions.
// The modes have to stay defined but just would be meaningless in that case.
//

//
// MemIo.sys specific header
// Some driver specific data like 
// - device name
// - ioctl codes
// - ioctl structs
//
#include "../MemIoShared.h"

//
// if memory mapping to user mode is supported, define this, 
// else comment it out
// if supported, 
// implement 
// - mapMemory
// - unmapMemory
// found right at the bottom of this file
//
#define RW_PRIM_MEM_MAPPING_SUPPORTED

//
// mode definitions
// have to stay defined, 
// even if not used by another's rw primitive implementation
//
#define READ_PA_MODE_DEF 1
#define READ_PA_MODE_4 2
#define READ_PA_MODE_MAPPED 3
#define READ_PA_MODE_CHUNKED_4 4

#define READ_PA_WTF_MODE_CHUNKED_4 1

#define WRITE_PA_MODE_DEF 1
#define WRITE_PA_MODE_4 2


//
// MemIo.sys
// internal specialized functions using the abilities of MemIo.sys
// not really necessary
// may use abilities that are not present in other rw drivers
//
static
NTSTATUS
__readPADef(HANDLE Device, UINT64 PA, UINT32 PageFlags, PVOID Buffer, UINT32 BufferSize);
static
NTSTATUS
__readPA4(HANDLE Device, UINT64 PA, UINT32 PageFlags, PUINT32 Buffer);
static
NTSTATUS
__readPAMmIoMapped(HANDLE Device, UINT64 PA, UINT32 PageFlags, PVOID Buffer, UINT32 BufferSize);
static
NTSTATUS
__readPAMmIoMappedChunked4(HANDLE Device, UINT64 PA, UINT32 PageFlags, PVOID Buffer, UINT32 BufferSize);



//
// Default read physical method called from within the usermode components.
// The logic may be implemented right in here.
// 
// The Mode param provides the possibility to call some other more specific functions in here,
// like actually done in here:
// - plain physical reading by the driver, 
// - plain physical reading just 4 bytes by the driver, 
// - user mode mapped reading
// - user mode mapped 4 byte chunks reading
// - ...
// 
NTSTATUS
readPA(HANDLE Device, UINT64 PA, UINT32 PageFlags, PVOID Buffer, UINT32 BufferSize, UINT32 Mode)
{
    NTSTATUS status = 0;

    switch ( Mode )
    {
        case READ_PA_MODE_DEF:
            status = __readPADef(Device, PA, PageFlags, Buffer, BufferSize);
            break;
        case READ_PA_MODE_4:
            status = __readPA4(Device, PA, PageFlags, Buffer);
            break;
        case READ_PA_MODE_MAPPED:
            status = __readPAMmIoMapped(Device, PA, PageFlags, Buffer, BufferSize);
            break;
        case READ_PA_MODE_CHUNKED_4:
            status = __readPAMmIoMappedChunked4(Device, PA, PageFlags, Buffer, BufferSize);
            break;
        default:
            status = STATUS_INVALID_PARAMETER;
            break;
    }

    return status;
}



// internal specialized functions using the abilities of MemIo.sys
// not really necessary
static
NTSTATUS
__readPAMmIoMapped4Wtf(HANDLE Device, UINT64 PA, UINT32 Size, UINT32 PageFlags, HANDLE File);

//
// Default read physical method called from within the usermode components.
// Writes directly to file, not to buffer.
// Just called in the spi project.
// 
// 
// The logic may be implemented right in here.
// The Mode param provides a possibility to call some other more specific functions in here,
// like done in here:
// - user mode mapped reading 4 byte chunks reading
//
NTSTATUS
readPAWtf(HANDLE Device, UINT64 PA, UINT32 Size, UINT32 PageFlags, HANDLE File, UINT32 Mode)
{
    NTSTATUS status = 0;

    switch ( Mode )
    {
        case READ_PA_WTF_MODE_CHUNKED_4:
            status = __readPAMmIoMapped4Wtf(Device, PA, Size, PageFlags, File);
            break;
        default:
            status = STATUS_INVALID_PARAMETER;
            break;
    }

    return status;
}



// internal specialized functions using the abilities of MemIo.sys
// not really necessary
static
NTSTATUS
__writePADef(HANDLE Device, UINT64 PA, UINT32 PageFlags, PVOID Buffer, UINT32 BufferSize);
static
NTSTATUS
__writePA4(HANDLE Device, UINT64 PA, UINT32 PageFlags, UINT32 Value);

//
// Default write physical method called from within the usermode components.
// The logic may be implemented right in here.
// The Flags param provides a possibility to call some other more specific functions in here,
// like done in here:
// - plain physical reading by the driver, 
// - user mode mapped reading
// - 4 bytes chunks reading, 
// - ...
//
NTSTATUS
writePA(HANDLE Device, UINT64 PA, UINT32 PageFlags, PVOID Buffer, UINT32 BufferSize, UINT32 Mode)
{
    NTSTATUS status = 0;

    switch ( Mode )
    {
        case WRITE_PA_MODE_DEF:
            status = __writePADef(Device, PA, PageFlags, Buffer, BufferSize);
            break;
        case WRITE_PA_MODE_4:
            status = __writePA4(Device, PA, PageFlags, *(PUINT32)Buffer);
            break;
        default:
            status = STATUS_INVALID_PARAMETER;
            break;
    }

    return status;
}

//
// Uses read pa of the driver.
// In this case it can read arbitrary sizes of up to one page 
// So there is just an iteration over the pages.
// If the driver just provides restricted size readings, the loops have to be adjusted.
//
NTSTATUS
__readPADef(HANDLE Device, UINT64 PA, UINT32 PageFlags, PVOID Buffer, UINT32 BufferSize)
{
    FEnter();

    NTSTATUS status = 0;
    SIZE_T bytesRead = 0;
    IO_STATUS_BLOCK iosb = { 0 };

    READ_PA_IN in = { 0 };

    UINT32 ioctl = IOCTL_READ_PA;
    
    SIZE_T startAddress = (SIZE_T)PA;
    SIZE_T endAddress = startAddress + BufferSize;
    SIZE_T nextPage = (SIZE_T)ALIGN_UP_TO_NEXT_BY(startAddress, PAGE_SIZE);
    UINT32 toRead = BufferSize;
    
    if ( endAddress > nextPage )
    {
        ULONG offset = 0;
        PUINT8 buffer = (PUINT8)Buffer;
        ULONG readSize = (ULONG)(nextPage - startAddress);
        
        //
        // first read from start address to end of page

        RtlZeroMemory(&in, sizeof(in));
        in.Address = (PVOID)startAddress;
        in.PageFlags = PageFlags;
        in.Size = readSize;
        
        RtlZeroMemory(&iosb, sizeof(iosb));
        status = generateIoRequest(Device, ioctl, &iosb, &in, READ_PA_IN_SIZE, &buffer[offset], readSize);
        if ( status != 0 )
        {
            EPrint("DeviceIO 0x%x request failed! (0x%x)\n", ioctl, status);
            goto clean;
        }
        bytesRead += (ULONG)iosb.Information;

        // next read with page aligned start addresses
        offset += readSize;
        toRead -= readSize;
        ULONG parts = toRead / PAGE_SIZE;
        ULONG rest = toRead % PAGE_SIZE;

        startAddress = nextPage;
        readSize = PAGE_SIZE;
        for ( ULONG i = 0; i < parts; i++ )
        {
            //DPrint("StartAddress: %llx.\n", startAddress);
            //DPrint("readSize: %x\n", readSize);

            RtlZeroMemory(&in, sizeof(in));
            in.Address = (PVOID)startAddress;
            in.PageFlags = PageFlags;
            in.Size = readSize;
        
            RtlZeroMemory(&iosb, sizeof(iosb));
            status = generateIoRequest(Device, ioctl, &iosb, &in, READ_PA_IN_SIZE, &buffer[offset], readSize);
            if ( status != 0 )
            {
                EPrint("DeviceIO 0x%x request failed! (0x%x)\n", ioctl, status);
                goto clean;
            }
            bytesRead += (ULONG)iosb.Information;

            startAddress += PAGE_SIZE;
            offset += PAGE_SIZE;
        }
        if ( rest > 0 )
        {
            readSize = rest;

            RtlZeroMemory(&in, sizeof(in));
            in.Address = (PVOID)startAddress;
            in.PageFlags = PageFlags;
            in.Size = readSize;
            
            RtlZeroMemory(&iosb, sizeof(iosb));
            status = generateIoRequest(Device, ioctl, &iosb, &in, READ_PA_IN_SIZE, &buffer[offset], readSize);
            if ( status != 0 )
            {
                EPrint("DeviceIO 0x%x request failed! (0x%x)\n", ioctl, status);
                goto clean;
            }
            bytesRead += (ULONG)iosb.Information;
        }
    }
    else
    {
        RtlZeroMemory(&in, sizeof(in));
        in.Address = (PVOID)startAddress;
        in.PageFlags = PageFlags;
        in.Size = toRead;

        DPrint("in\n");
        DPrint("  Address: %p\n", in.Address);
        DPrint("  PageFlags: 0x%x\n", in.PageFlags);
        DPrint("  Size: 0x%x\n", in.Size);
        
        RtlZeroMemory(&iosb, sizeof(iosb));
        status = generateIoRequest(Device, ioctl, &iosb, &in, READ_PA_IN_SIZE, Buffer, toRead);
        if ( status != 0 )
        {
            EPrint("DeviceIO 0x%x request failed! (0x%x)\n", ioctl, status);
            goto clean;
        }
        bytesRead += (ULONG)iosb.Information;
    }

    //DPrint("bytesRead: 0x%zx\n", bytesRead);

clean:

    FLeave();
    return status;
}

//
// user memory mapped physical address reading
// uses arbitrary, optimized chunk sizes of memcpy
// may lead to false data
//
// requirement
// the driver provides memory mapping to user space
//
NTSTATUS
__readPAMmIoMapped(HANDLE Device, UINT64 PA, UINT32 PageFlags, PVOID Buffer, UINT32 BufferSize)
{
    //FEnter();
    (PageFlags);

    NTSTATUS status = 0;
    SIZE_T bytesRead = 0;
    IO_STATUS_BLOCK iosb = { 0 };

    MAP_PA_IN mmioMapIn = { 0 };
    MAP_PA_OUT mmioMapOut = { 0 };
    UNMAP_PA_IN mmioUnmapIn = { 0 };
    
    UINT32 mapIoctl = IOCTL_MMIO_MAP;
    UINT32 unmapIoctl = IOCTL_MMIO_UNMAP;
    
    SIZE_T startAddress = (SIZE_T)PA;
    SIZE_T endAddress = startAddress + BufferSize;
    SIZE_T nextPage = (SIZE_T)ALIGN_UP_TO_NEXT_BY(startAddress, PAGE_SIZE);
    UINT32 toRead = BufferSize;
    
    if ( endAddress > nextPage )
    {
        ULONG offset = 0;
        PUINT8 buffer = (PUINT8)Buffer;
        ULONG readSize = (ULONG)(nextPage - startAddress);
        
        //
        // first read from start address to end of page

        RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
        RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
        mmioMapIn.Address = (PVOID)startAddress;
        mmioMapIn.Size = readSize;
        
        RtlZeroMemory(&iosb, sizeof(iosb));
        status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
        if ( status != 0 )
        {
            EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
            goto clean;
        }
        RtlCopyMemory(&buffer[offset], mmioMapOut.LockedAddress, readSize);
        bytesRead += readSize;
        
        mmioUnmapIn = mmioMapOut;
        status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
        if ( status != 0 )
        {
            EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
            goto clean;
        }

        // next read with page aligned start addresses
        offset += readSize;
        toRead -= readSize;
        ULONG parts = toRead / PAGE_SIZE;
        ULONG rest = toRead % PAGE_SIZE;

        startAddress = nextPage;
        readSize = PAGE_SIZE;
        for ( ULONG i = 0; i < parts; i++ )
        {
            //DPrint("StartAddress: %llx.\n", startAddress);
            //DPrint("readSize: %x\n", readSize);
            
            RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
            RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
            mmioMapIn.Address = (PVOID)startAddress;
            mmioMapIn.Size = readSize;
        
            RtlZeroMemory(&iosb, sizeof(iosb));
            status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
            if ( status != 0 )
            {
                EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
                goto clean;
            }
            RtlCopyMemory(&buffer[offset], mmioMapOut.LockedAddress, readSize);
            bytesRead += readSize;
            
            mmioUnmapIn = mmioMapOut;
            status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
            if ( status != 0 )
            {
                EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
                goto clean;
            }

            startAddress += PAGE_SIZE;
            offset += PAGE_SIZE;
        }
        if ( rest > 0 )
        {
            readSize = rest;
            
            RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
            RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
            mmioMapIn.Address = (PVOID)startAddress;
            mmioMapIn.Size = readSize;
            
            RtlZeroMemory(&iosb, sizeof(iosb));
            status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
            if ( status != 0 )
            {
                EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
                goto clean;
            }
            RtlCopyMemory(&buffer[offset], mmioMapOut.LockedAddress, readSize);
            bytesRead += readSize;
            
            mmioUnmapIn = mmioMapOut;
            status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
            if ( status != 0 )
            {
                EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
                goto clean;
            }
        }
    }
    else
    {
        RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
        RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
        mmioMapIn.Address = (PVOID)startAddress;
        mmioMapIn.Size = toRead;
        
        RtlZeroMemory(&iosb, sizeof(iosb));
        status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
        if ( status != 0 )
        {
            EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
            goto clean;
        }
        RtlCopyMemory(Buffer, mmioMapOut.LockedAddress, toRead);
        bytesRead += toRead;
        
        mmioUnmapIn = mmioMapOut;
        status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
        if ( status != 0 )
        {
            EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
            goto clean;
        }
    }

    //DPrint("bytesRead: 0x%zx\n", bytesRead);

clean:

    //FLeave();
    return status;
}

//
// User memory mapped pa reading of 4 byte chunks
//
// requirement
// the driver provides memory mapping to user space
//
NTSTATUS
__readPAMmIoMappedChunked4(HANDLE Device, UINT64 PA, UINT32 PageFlags, PVOID Buffer, UINT32 BufferSize)
{
    FEnter();
    (PageFlags);

    NTSTATUS status = 0;
    SIZE_T bytesRead = 0;
    IO_STATUS_BLOCK iosb = { 0 };

    MAP_PA_IN mmioMapIn = { 0 };
    MAP_PA_OUT mmioMapOut = { 0 };
    UNMAP_PA_IN mmioUnmapIn = { 0 };
    
    UINT32 mapIoctl = IOCTL_MMIO_MAP;
    UINT32 unmapIoctl = IOCTL_MMIO_UNMAP;
    
    SIZE_T startAddress = (SIZE_T)PA;
    SIZE_T endAddress = startAddress + BufferSize;
    SIZE_T nextPage = (SIZE_T)ALIGN_UP_TO_NEXT_BY(startAddress, PAGE_SIZE);
    UINT32 toRead = BufferSize;
    
    PUINT32 bufferPtr32 = NULL;
    volatile PUINT32 vaPtr32 = NULL;

    UINT32 ci = 0;

    DPrint("Device: %p\n", Device);
    DPrint("PA: 0x%llx\n", PA);
    DPrint("PageFlags: 0x%x\n", PageFlags);
    DPrint("Buffer: %p\n", Buffer);
    DPrint("BufferSize: 0x%x\n", BufferSize);

    if ( (UINT64)startAddress%4 != 0 || toRead % 4 != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Not 4 Byte aligned! (0x%x)\n", status);
        return status;
    }
    
    if ( endAddress > nextPage )
    {
        ULONG offset = 0;
        PUINT8 buffer = (PUINT8)Buffer;
        ULONG readSize = (ULONG)(nextPage - startAddress);
        
        //
        // first read from start address to end of page

        RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
        RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
        mmioMapIn.Address = (PVOID)startAddress;
        mmioMapIn.Size = readSize;
        
        RtlZeroMemory(&iosb, sizeof(iosb));
        status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
        if ( status != 0 )
        {
            EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
            goto clean;
        }
        
        bufferPtr32 = (PUINT32)&buffer[offset];
        vaPtr32 = (PUINT32)mmioMapOut.LockedAddress;

        for ( ci = 0; ci < readSize; ci += 4 )
        {
            *bufferPtr32 = *vaPtr32;
            ++bufferPtr32;
            ++vaPtr32;
        }

        bytesRead += readSize;
        
        mmioUnmapIn = mmioMapOut;
        status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
        if ( status != 0 )
        {
            EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
            goto clean;
        }

        // next read with page aligned start addresses
        offset += readSize;
        toRead -= readSize;
        ULONG parts = toRead / PAGE_SIZE;
        ULONG rest = toRead % PAGE_SIZE;

        startAddress = nextPage;
        readSize = PAGE_SIZE;
        for ( ULONG pi = 0; pi < parts; pi++ )
        {
            //DPrint("StartAddress: %llx.\n", startAddress);
            //DPrint("readSize: %x\n", readSize);
            
            RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
            RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
            mmioMapIn.Address = (PVOID)startAddress;
            mmioMapIn.Size = readSize;
        
            RtlZeroMemory(&iosb, sizeof(iosb));
            status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
            if ( status != 0 )
            {
                EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
                goto clean;
            }
            
            bufferPtr32 = (PUINT32)&buffer[offset];
            vaPtr32 = (PUINT32)mmioMapOut.LockedAddress;

            for ( ci = 0; ci < readSize; ci += 4 )
            {
                *bufferPtr32 = *vaPtr32;
                ++bufferPtr32;
                ++vaPtr32;
            }

            bytesRead += readSize;
            
            mmioUnmapIn = mmioMapOut;
            status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
            if ( status != 0 )
            {
                EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
                goto clean;
            }

            startAddress += PAGE_SIZE;
            offset += PAGE_SIZE;
        }
        if ( rest > 0 )
        {
            readSize = rest;
            
            RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
            RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
            mmioMapIn.Address = (PVOID)startAddress;
            mmioMapIn.Size = readSize;
            
            RtlZeroMemory(&iosb, sizeof(iosb));
            status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
            if ( status != 0 )
            {
                EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
                goto clean;
            }
            
            bufferPtr32 = (PUINT32)&buffer[offset];
            vaPtr32 = (PUINT32)mmioMapOut.LockedAddress;

            for ( ci = 0; ci < readSize; ci += 4 )
            {
                *bufferPtr32 = *vaPtr32;
                ++bufferPtr32;
                ++vaPtr32;
            }

            bytesRead += readSize;
            
            mmioUnmapIn = mmioMapOut;
            status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
            if ( status != 0 )
            {
                EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
                goto clean;
            }
        }
    }
    else
    {
        RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
        RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
        mmioMapIn.Address = (PVOID)startAddress;
        mmioMapIn.Size = toRead;
        
        RtlZeroMemory(&iosb, sizeof(iosb));
        status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
        if ( status != 0 )
        {
            EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
            goto clean;
        }
        
        bufferPtr32 = (PUINT32)Buffer;
        vaPtr32 = (PUINT32)mmioMapOut.LockedAddress;

        for ( ci = 0; ci < toRead; ci += 4 )
        {
            *bufferPtr32 = *vaPtr32;
            ++bufferPtr32;
            ++vaPtr32;
        }

        bytesRead += toRead;
        
        mmioUnmapIn = mmioMapOut;
        status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
        if ( status != 0 )
        {
            EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
            goto clean;
        }
    }

    //DPrint("bytesRead: 0x%zx\n", bytesRead);

clean:

    FLeave();
    return status;
}

//
// User memory mapped pa reading of 4 byte chunks
// Write it directly to file
//
// requirement
// the driver provides memory mapping to user space
//
NTSTATUS
__readPAMmIoMapped4Wtf(HANDLE Device, UINT64 PA, UINT32 Size, UINT32 PageFlags, HANDLE File)
{
    FEnter();
    (PageFlags);

    NTSTATUS status = 0;
    SIZE_T bytesRead = 0;
    IO_STATUS_BLOCK iosb = { 0 };

    MAP_PA_IN mmioMapIn = { 0 };
    MAP_PA_OUT mmioMapOut = { 0 };
    UNMAP_PA_IN mmioUnmapIn = { 0 };
    
    UINT32 mapIoctl = IOCTL_MMIO_MAP;
    UINT32 unmapIoctl = IOCTL_MMIO_UNMAP;
    
    SIZE_T startAddress = (SIZE_T)PA;
    SIZE_T endAddress = startAddress + Size;
    SIZE_T nextPage = (SIZE_T)ALIGN_UP_TO_NEXT_BY(startAddress, PAGE_SIZE);
    UINT32 toRead = Size;
    
    PUINT8 buffer = NULL;
    PUINT32 bufferPtr32 = NULL;
    volatile PUINT32 vaPtr32 = NULL;
    
    ULONG bytesWritten = 0;

    UINT32 ci = 0;

    if ( (UINT64)startAddress%4 != 0 || toRead % 4 != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Not 4 Byte aligned! (0x%x)\n", status);
        return status;
    }
    
    // alloc intermediate page buffer 
    buffer = (PUINT8)malloc(PAGE_SIZE);
    if ( !buffer )
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean;
    }

    if ( endAddress > nextPage )
    {
        ULONG offset = 0;
        ULONG readSize = (ULONG)(nextPage - startAddress);
        
        DPrint("startAddress: 0x%zx\n", startAddress);
        DPrint("readSize: 0x%x\n", readSize);
        //
        // first read from start address to end of page

        RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
        RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
        mmioMapIn.Address = (PVOID)startAddress;
        mmioMapIn.Size = readSize;
        
        RtlZeroMemory(&iosb, sizeof(iosb));
        status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
        if ( status != 0 )
        {
            EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
            goto clean;
        }
        
        bufferPtr32 = (PUINT32)buffer;
        vaPtr32 = (PUINT32)mmioMapOut.LockedAddress;

        for ( ci = 0; ci < readSize; ci += 4 )
        {
            *bufferPtr32 = *vaPtr32;
            ++bufferPtr32;
            ++vaPtr32;
        }

        status = kWriteFile(File, buffer, readSize, (PULONG)&bytesWritten);
        DPrint("kWriteFile status: 0x%x\n", status);
        DPrint("0x%x bytes written to dump file!\n", bytesWritten);
        if ( status != 0 )
        {
            EPrint("Writing failed! (0x%x)\n", status);
            goto clean;
        }

        bytesRead += readSize;
        
        mmioUnmapIn = mmioMapOut;
        status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
        if ( status != 0 )
        {
            EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
            goto clean;
        }

        // next read with page aligned start addresses
        offset += readSize;
        toRead -= readSize;
        ULONG parts = toRead / PAGE_SIZE;
        ULONG rest = toRead % PAGE_SIZE;

        startAddress = nextPage;
        readSize = PAGE_SIZE;
        for ( ULONG pi = 0; pi < parts; pi++ )
        {
            //DPrint("StartAddress: %llx.\n", startAddress);
            //DPrint("readSize: %x\n", readSize);
            
            DPrint("startAddress: 0x%zx\n", startAddress);
            DPrint("readSize: 0x%x\n", readSize);

            RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
            RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
            mmioMapIn.Address = (PVOID)startAddress;
            mmioMapIn.Size = readSize;
        
            RtlZeroMemory(&iosb, sizeof(iosb));
            status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
            if ( status != 0 )
            {
                EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
                goto clean;
            }
            
            bufferPtr32 = (PUINT32)buffer;
            vaPtr32 = (PUINT32)mmioMapOut.LockedAddress;

            for ( ci = 0; ci < readSize; ci += 4 )
            {
                *bufferPtr32 = *vaPtr32;
                ++bufferPtr32;
                ++vaPtr32;
            }

            status = kWriteFile(File, buffer, readSize, (PULONG)&bytesWritten);
            DPrint("kWriteFile status: 0x%x\n", status);
            DPrint("0x%x bytes written to dump file!\n", bytesWritten);
            if ( status != 0 )
            {
                EPrint("Writing failed! (0x%x)\n", status);
                goto clean;
            }

            bytesRead += readSize;
            
            mmioUnmapIn = mmioMapOut;
            status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
            if ( status != 0 )
            {
                EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
                goto clean;
            }

            startAddress += PAGE_SIZE;
            offset += PAGE_SIZE;
        }
        if ( rest > 0 )
        {
            readSize = rest;
            DPrint("startAddress: 0x%zx\n", startAddress);
            DPrint("readSize: 0x%x\n", readSize);
            
            RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
            RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
            mmioMapIn.Address = (PVOID)startAddress;
            mmioMapIn.Size = readSize;
            
            RtlZeroMemory(&iosb, sizeof(iosb));
            status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
            if ( status != 0 )
            {
                EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
                goto clean;
            }
            
            bufferPtr32 = (PUINT32)buffer;
            vaPtr32 = (PUINT32)mmioMapOut.LockedAddress;

            for ( ci = 0; ci < readSize; ci += 4 )
            {
                *bufferPtr32 = *vaPtr32;
                ++bufferPtr32;
                ++vaPtr32;
            }
            
            status = kWriteFile(File, buffer, readSize, (PULONG)&bytesWritten);
            DPrint("kWriteFile status: 0x%x\n", status);
            DPrint("0x%x bytes written to dump file!\n", bytesWritten);
            if ( status != 0 )
            {
                EPrint("Writing failed! (0x%x)\n", status);
                goto clean;
            }

            bytesRead += readSize;
            
            mmioUnmapIn = mmioMapOut;
            status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
            if ( status != 0 )
            {
                EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
                goto clean;
            }
        }
    }
    else
    {
        RtlZeroMemory(&mmioMapOut, sizeof(mmioMapOut));
        RtlZeroMemory(&mmioMapIn, sizeof(mmioMapIn));
        mmioMapIn.Address = (PVOID)startAddress;
        mmioMapIn.Size = toRead;
        DPrint("startAddress: 0x%zx\n", startAddress);
        DPrint("toRead: 0x%x\n", toRead);
        
        RtlZeroMemory(&iosb, sizeof(iosb));
        status = generateIoRequest(Device, mapIoctl, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
        if ( status != 0 )
        {
            EPrint("DeviceIO 0x%x request failed! (0x%x)\n", mapIoctl, status);
            goto clean;
        }
        
        bufferPtr32 = (PUINT32)buffer;
        vaPtr32 = (PUINT32)mmioMapOut.LockedAddress;

        for ( ci = 0; ci < toRead; ci += 4 )
        {
            *bufferPtr32 = *vaPtr32;
            ++bufferPtr32;
            ++vaPtr32;
        }
        
        status = kWriteFile(File, buffer, toRead, (PULONG)&bytesWritten);
        DPrint("kWriteFile status: 0x%x\n", status);
        DPrint("0x%x bytes written to dump file!\n", bytesWritten);
        if ( status != 0 )
        {
            EPrint("Writing failed! (0x%x)\n", status);
            goto clean;
        }

        bytesRead += toRead;
        
        mmioUnmapIn = mmioMapOut;
        status = generateIoRequest(Device, unmapIoctl, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
        if ( status != 0 )
        {
            EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
            goto clean;
        }
    }

    //DPrint("bytesRead: 0x%zx\n", bytesRead);

clean:
    if ( buffer )
        free(buffer);

    FLeave();
    return status;
}

//
// Convenience function to read just 4 bytes.
// Might be the only possibility provided by some drivers.
//
NTSTATUS
__readPA4(HANDLE Device, UINT64 PA, UINT32 PageFlags, PUINT32 Buffer)
{
    //FEnter();

    NTSTATUS status = 0;
    IO_STATUS_BLOCK iosb = { 0 };

    READ_PA_IN in = { 0 };

    UINT32 ioctl = IOCTL_READ_PA;
    
    SIZE_T startAddress = (SIZE_T)PA;
    UINT32 toRead = 4;
    
    if ( startAddress % 4 != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        goto clean;
    }
    
    in.Address = (PVOID)startAddress;
    in.PageFlags = PageFlags;
    in.Size = toRead;
    
    RtlZeroMemory(&iosb, sizeof(iosb));
    status = generateIoRequest(Device, ioctl, &iosb, &in, READ_PA_IN_SIZE, Buffer, toRead);
    if ( status != 0 )
    {
        EPrint("DeviceIO 0x%x request failed! (0x%x)\n", ioctl, status);
        goto clean;
    }

    //DPrint("bytesRead: 0x%zx\n", bytesRead);

clean:

    //FLeave();
    return status;
}

//
// Uses write pa of the driver.
// In this case it can write arbitrary sizes of up to one page 
// So there is just an iteration over the pages.
// If the driver just provides restricted size writing, the loops have to be adjusted.
//
NTSTATUS
__writePADef(HANDLE Device, UINT64 PA, UINT32 PageFlags, PVOID Buffer, UINT32 BufferSize)
{
    //FEnter();

    NTSTATUS status = 0;
    //ULONG bytesReturned;
    IO_STATUS_BLOCK iosb = { 0 };
    UINT32 ioctl = IOCTL_WRITE_PA;

    PWRITE_PA_IN in = NULL;
    UINT32 inSize = WRITE_PA_IN_DEF_SIZE + PAGE_SIZE;
    
    //if ( PA % 4 != 0 || BufferSize % 4 != 0 )
    //{
    //    status = STATUS_INVALID_PARAMETER;
    //    goto clean;
    //}

    in = (PWRITE_PA_IN)malloc(inSize);
    if ( !in )
    {
        status = STATUS_NO_MEMORY;
        EPrint("No memory! (0x%x)\n", status);
        goto clean;
    }


    SIZE_T startAddress = (SIZE_T)PA;
    SIZE_T endAddress = startAddress + BufferSize;
    SIZE_T nextPage = (SIZE_T)ALIGN_UP_TO_NEXT_BY(startAddress, PAGE_SIZE);
    UINT32 toWrite = BufferSize;
    
    if ( endAddress > nextPage )
    {
        ULONG offset = 0;
        PUINT8 buffer = (PUINT8)Buffer;
        ULONG writeSize = (ULONG)(nextPage - startAddress);
        
        //
        // first write from start address to end of page

        RtlZeroMemory(in, inSize);
        in->Address = (PVOID)startAddress;
        in->PageFlags = PageFlags;
        in->Size = writeSize;
        RtlCopyMemory(in->Data, &buffer[offset], writeSize);
        
        RtlZeroMemory(&iosb, sizeof(iosb));
        status = generateIoRequest(Device, ioctl, &iosb, in, inSize, NULL, 0);
        if ( status != 0 )
        {
            EPrint("DeviceIO 0x%x request failed! (0x%x)\n", ioctl, status);
            goto clean;
        }

        // next write with page aligned start addresses
        offset += writeSize;
        toWrite -= writeSize;
        ULONG parts = toWrite / PAGE_SIZE;
        ULONG rest = toWrite % PAGE_SIZE;

        startAddress = nextPage;
        writeSize = PAGE_SIZE;
        for ( ULONG i = 0; i < parts; i++ )
        {
            //DPrint("StartAddress: %llx.\n", startAddress);
            //DPrint("readSize: %x\n", writeSize);
            
            RtlZeroMemory(in, inSize);
            in->Address = (PVOID)startAddress;
            in->PageFlags = PageFlags;
            in->Size = writeSize;
            RtlCopyMemory(in->Data, &buffer[offset], writeSize);
        
            RtlZeroMemory(&iosb, sizeof(iosb));
            status = generateIoRequest(Device, ioctl, &iosb, in, inSize, NULL, 0);
            if ( status != 0 )
            {
                EPrint("DeviceIO 0x%x request failed! (0x%x)\n", ioctl, status);
                goto clean;
            }

            startAddress += PAGE_SIZE;
            offset += PAGE_SIZE;
        }
        if ( rest > 0 )
        {
            writeSize = rest;
            
            RtlZeroMemory(in, inSize);
            in->Address = (PVOID)startAddress;
            in->PageFlags = PageFlags;
            in->Size = writeSize;
            RtlCopyMemory(in->Data, &buffer[offset], writeSize);
            
            RtlZeroMemory(&iosb, sizeof(iosb));
            status = generateIoRequest(Device, ioctl, &iosb, in, inSize, NULL, 0);
            if ( status != 0 )
            {
                EPrint("DeviceIO 0x%x request failed! (0x%x)\n", ioctl, status);
                goto clean;
            }
        }
    }
    else
    {
        RtlZeroMemory(in, inSize);
        in->Address = (PVOID)startAddress;
        in->PageFlags = PageFlags;
        in->Size = toWrite;
        RtlCopyMemory(in->Data, Buffer, toWrite);
        
        RtlZeroMemory(&iosb, sizeof(iosb));
        status = generateIoRequest(Device, ioctl, &iosb, in, inSize, Buffer, toWrite);
        if ( status != 0 )
        {
            EPrint("DeviceIO 0x%x request failed! (0x%x)\n", ioctl, status);
            goto clean;
        }
    }


clean:
    if ( in )
        free(in);
    
    //FLeave();
    return status;
}

//
// Convenience function to write just 4 bytes.
// Might be the only possibility provided by some drivers.
//
NTSTATUS
__writePA4(HANDLE Device, UINT64 PA, UINT32 PageFlags, UINT32 Value)
{
    //FEnter();

    NTSTATUS status = 0;
    IO_STATUS_BLOCK iosb = { 0 };
    UINT32 ioctl = IOCTL_WRITE_PA;

    PWRITE_PA_IN in = NULL;
    UINT32 inSize = 0x14;
    UINT8 inBuffer[0x14];

    in = (PWRITE_PA_IN)&inBuffer;

    SIZE_T startAddress = (SIZE_T)PA;
    UINT32 toWrite = 4;
    
    if ( startAddress % 4 != 0 )
    {
        EPrint("Unaligned start address!\n");
        status = STATUS_INVALID_PARAMETER;
        goto clean;
    }

    RtlZeroMemory(in, inSize);
    in->Address = (PVOID)startAddress;
    in->PageFlags = PageFlags;
    in->Size = toWrite;
    *(PUINT32)in->Data = Value;
        
    status = generateIoRequest(Device, ioctl, &iosb, in, inSize, NULL, 0);
    if ( status != 0 )
    {
        EPrint("DeviceIO 0x%x request failed! (0x%x)\n", ioctl, status);
        goto clean;
    }

clean:
    
    //FLeave();
    return status;
}



//
// portio reading
// 
// in(B)(8|16|32)
// out(8|16|32)
// have to be adjusted to the driver needs
// 
// outIn(8|16|32)
// outOut32
// are just wrapper calling the former
//

//
// indword
//
NTSTATUS
in32(HANDLE Device, UINT16 Port, PUINT32 Value)
{
    //FEnter();

    NTSTATUS status = 0;
    ULONG bytesReturned;
    IO_STATUS_BLOCK iosb = { 0 };
    
    PORT_IN_IN in = { 0};
    PORT_IN_OUT out = { 0};

    in.Port = Port;
    in.Size = 4;

    *Value = 0;

    UINT32 ioctl = IOCTL_PORT_IN;

    status = generateIoRequest(Device, ioctl, &iosb, &in, sizeof(in), &out, sizeof(out));
    if ( status != 0 )
    {
        EPrint("DeviceIO request failed! (0x%x)\n", status);
        goto clean;
    }
    
    //DPrint("Success!\n");
    //DPrint("Port 0x%x read!\n", Port);
    bytesReturned = (ULONG)iosb.Information;
    *Value = out.Value;
    //DPrint("bytesReturned: 0x%x\n", bytesReturned);

clean:
    //FLeave();
    return status;
}

//
// inword
//
NTSTATUS
in16(HANDLE Device, UINT16 Port, PUINT16 Value)
{
    //FEnter();

    NTSTATUS status = 0;
    ULONG bytesReturned;
    IO_STATUS_BLOCK iosb = { 0 };
    
    PORT_IN_IN in = { 0};
    PORT_IN_OUT out = { 0};

    //in.AD.AddressPort = Port;
    in.Port = Port;
    in.Size = 2;

    *Value = 0;


    UINT32 ioctl = IOCTL_PORT_IN;

    status = generateIoRequest(Device, ioctl, &iosb, &in, sizeof(in), &out, sizeof(out));
    if ( status != 0 )
    {
        EPrint("DeviceIO request failed! (0x%x)\n", status);
        goto clean;
    }
    
    //DPrint("Success!\n");
    //DPrint("Port 0x%x read!\n", Port);
    bytesReturned = (ULONG)iosb.Information;
    *Value = (UINT16)out.Value;
    //DPrint("bytesReturned: 0x%x\n", bytesReturned);

clean:
    //FLeave();
    return status;
}

//
// inbyte
//
NTSTATUS
in8(HANDLE Device, UINT16 Port, PUINT8 Value)
{
    //FEnter();

    NTSTATUS status = 0;
    ULONG bytesReturned;
    IO_STATUS_BLOCK iosb = { 0 };
    
    PORT_IN_IN in = { 0};
    PORT_IN_OUT out = { 0};

    //in.AD.AddressPort = Port;
    in.Port = Port;
    in.Size = 1;

    *Value = 0;


    UINT32 ioctl = IOCTL_PORT_IN;

    status = generateIoRequest(Device, ioctl, &iosb, &in, sizeof(in), &out, sizeof(out));
    if ( status != 0 )
    {
        EPrint("DeviceIO request failed! (0x%x)\n", status);
        goto clean;
    }
    
    //DPrint("Success!\n");
    //DPrint("Port 0x%x read!\n", Port);
    bytesReturned = (ULONG)iosb.Information;
    *Value = (UINT8)out.Value;
    //DPrint("bytesReturned: 0x%x\n", bytesReturned);

clean:
    //FLeave();
    return status;
}

//
// port in into buffer with 32 bit chunks
//
NTSTATUS
in32B(HANDLE Device, UINT16 Port, PVOID Buffer, UINT32 BufferSize)
{
    //FEnter();

    NTSTATUS status = 0;
    ULONG bytesReturned;
    IO_STATUS_BLOCK iosb = { 0 };
    
    PORT_IN_IN in = { 0};
    //PORT_IN_OUT out = { 0};

    //in.AD.AddressPort = Port;
    in.Port = Port;
    in.Size = 4;

    UINT32 ioctl = IOCTL_PORT_IN;

    if ( BufferSize % 4 != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("BufferSize not 4 byte aligned! (0x%x)\n", status);
        goto clean;
    }

    UINT32 parts = BufferSize / 4;
    UINT16 offset = Port;
    PUINT32 buffer32Ptr = (PUINT32)Buffer;

    for ( UINT32 i = 0; i < parts; i++ )
    {
        RtlZeroMemory(&iosb, sizeof(iosb));

        in.Port = offset;
        
        status = generateIoRequest(Device, ioctl, &iosb, &in, sizeof(in), buffer32Ptr, 4);
        if ( status != 0 )
        {
            EPrint("DeviceIO request failed! (0x%x)\n", status);
            goto clean;
        }
    
        //DPrint("Success!\n");
        //DPrint("Port 0x%x read!\n", Port);
        bytesReturned = (ULONG)iosb.Information;
        //DPrint("bytesReturned: 0x%x\n", bytesReturned);

        ++buffer32Ptr;
        offset += 4;
    }

clean:
    //FLeave();
    return status;
}

//
// port in into buffer with 16 bit chunks
//
NTSTATUS
in16B(HANDLE Device, UINT16 Port, PVOID Buffer, UINT32 BufferSize)
{
    //FEnter();

    NTSTATUS status = 0;
    ULONG bytesReturned;
    IO_STATUS_BLOCK iosb = { 0 };
    
    PORT_IN_IN in = { 0};

    //in.AD.AddressPort = Port;
    in.Port = Port;
    in.Size = 2;

    UINT32 ioctl = IOCTL_PORT_IN;

    if ( BufferSize % 2 != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("BufferSize not 2 byte aligned! (0x%x)\n", status);
        goto clean;
    }

    UINT32 parts = BufferSize / 2;
    UINT16 offset = Port;
    PUINT16 buffer16Ptr = (PUINT16)Buffer;

    for ( UINT32 i = 0; i < parts; i++ )
    {
        RtlZeroMemory(&iosb, sizeof(iosb));

        in.Port = offset;
        
        status = generateIoRequest(Device, ioctl, &iosb, &in, sizeof(in), buffer16Ptr, 2);
        if ( status != 0 )
        {
            EPrint("DeviceIO request failed! (0x%x)\n", status);
            goto clean;
        }
    
        //DPrint("Success!\n");
        //DPrint("Port 0x%x read!\n", Port);
        bytesReturned = (ULONG)iosb.Information;
        //DPrint("bytesReturned: 0x%x\n", bytesReturned);

        ++buffer16Ptr;
        offset += 2;
    }

clean:
    //FLeave();
    return status;
}

//
// port in into buffer with 8 bit chunks
//
NTSTATUS
in8B(HANDLE Device, UINT16 Port, PVOID Buffer, UINT32 BufferSize)
{
    //FEnter();
    
    NTSTATUS status = 0;
    ULONG bytesReturned;
    IO_STATUS_BLOCK iosb = { 0 };
    
    PORT_IN_IN in = { 0};
    
    //in.AD.AddressPort = Port;
    in.Port = Port;
    in.Size = 1;
    
    UINT32 ioctl = IOCTL_PORT_IN;
    
    UINT16 offset = Port;
    PUINT8 buffer8Ptr = (PUINT8)Buffer;

    for ( UINT32 i = 0; i < BufferSize; i++ )
    {
        RtlZeroMemory(&iosb, sizeof(iosb));
        
        in.Port = offset;
        
        status = generateIoRequest(Device, ioctl, &iosb, &in, sizeof(in), buffer8Ptr, 1);
        if ( status != 0 )
        {
            EPrint("DeviceIO request failed! (0x%x)\n", status);
            goto clean;
        }
        
        //DPrint("Success!\n");
        //DPrint("Port 0x%x read!\n", Port);
        bytesReturned = (ULONG)iosb.Information;
        //DPrint("bytesReturned: 0x%x\n", bytesReturned);

        ++buffer8Ptr;
        ++offset;
    }

clean:
    //FLeave();
    return status;
}


//
// portio write
//

//
// outbyte
//
NTSTATUS
out8(_In_ HANDLE Device, _In_ UINT16 Port, _In_ UINT8 Value)
{
    //FEnter();

    NTSTATUS status = 0;
    ULONG bytesReturned;
    IO_STATUS_BLOCK iosb = { 0 };
    
    PORT_OUT in = { 0};

    in.Port = Port;
    in.Size = 1;
    in.Value = Value;

    UINT32 ioctl = IOCTL_PORT_OUT;

    status = generateIoRequest(Device, ioctl, &iosb, &in, sizeof(in), NULL, 0);
    if ( status != 0 )
    {
        EPrint("DeviceIO request failed! (0x%x)\n", status);
        goto clean;
    }
    
    //DPrint("Success!\n");
    //DPrint("Port 0x%x written!\n", Port);
    bytesReturned = (ULONG)iosb.Information;
    //DPrint("bytesReturned: 0x%x\n", bytesReturned);

clean:
    
    //FLeave();
    return status;
}

//
// outword
//
NTSTATUS
out16(_In_ HANDLE Device, _In_ UINT16 Port, _In_ UINT16 Value)
{
    //FEnter();

    NTSTATUS status = 0;
    ULONG bytesReturned;
    IO_STATUS_BLOCK iosb = { 0 };
    
    PORT_OUT in = { 0};

    in.Port = Port;
    in.Size = 2;
    in.Value = Value;

    UINT32 ioctl = IOCTL_PORT_OUT;

    status = generateIoRequest(Device, ioctl, &iosb, &in, sizeof(in), NULL, 0);
    if ( status != 0 )
    {
        EPrint("DeviceIO request failed! (0x%x)\n", status);
        goto clean;
    }
    
    //DPrint("Success!\n");
    //DPrint("Port 0x%x written!\n", Port);
    bytesReturned = (ULONG)iosb.Information;
    //DPrint("bytesReturned: 0x%x\n", bytesReturned);

clean:
    
    //FLeave();
    return status;
}

//
// outdword
//
NTSTATUS
out32(_In_ HANDLE Device, _In_ UINT16 Port, _In_ UINT32 Value)
{
    //FEnter();

    NTSTATUS status = 0;
    ULONG bytesReturned;
    IO_STATUS_BLOCK iosb = { 0 };
    
    PORT_OUT in = { 0 };

    in.Port = Port;
    in.Size = 4;
    in.Value = Value;

    UINT32 ioctl = IOCTL_PORT_OUT;

    status = generateIoRequest(Device, ioctl, &iosb, &in, sizeof(in), NULL, 0);
    if ( status != 0 )
    {
        EPrint("DeviceIO request failed! (0x%x)\n", status);
        goto clean;
    }
    
    //DPrint("Success!\n");
    //DPrint("Port 0x%x written!\n", Port);
    bytesReturned = (ULONG)iosb.Information;
    //DPrint("bytesReturned: 0x%x\n", bytesReturned);

clean:
    
    //FLeave();
    return status;
}


// 
// outIn(8|16|32)
// outOut32
// are just wrapper calling the former port io functions
//

//
// portio write (command) read 8 bit (status/value)
//
NTSTATUS
outIn8(
    _In_ HANDLE Device, 
    _In_ UINT16 CmdPort, 
    _In_ UINT16 StatusPort, 
    _In_ UINT32 Index, 
    _In_ PUINT8 Value
)
{
    //FEnter();

    NTSTATUS status = 0;

    status = out32(Device, CmdPort, Index);
    if ( status != 0 )
    {
        EPrint("out32 0x%x:0x%x failed! (0x%x)\n", CmdPort, Index, status);
        goto clean;
    }
    status = in8(Device, StatusPort, Value);
    if ( status != 0 )
    {
        EPrint("in8 0x%x failed! (0x%x)\n", StatusPort, status);
        goto clean;
    }

clean:
    
    //FLeave();
    return status;
}

//
// portio write (command) read 16 bit (status/value)
//
NTSTATUS
outIn16(
    _In_ HANDLE Device, 
    _In_ UINT16 CmdPort, 
    _In_ UINT16 StatusPort, 
    _In_ UINT32 Index, 
    _In_ PUINT16 Value
)
{
    //FEnter();

    NTSTATUS status = 0;

    status = out32(Device, CmdPort, Index);
    if ( status != 0 )
    {
        EPrint("out32 0x%x:0x%x failed! (0x%x)\n", CmdPort, Index, status);
        goto clean;
    }
    status = in16(Device, StatusPort, Value);
    if ( status != 0 )
    {
        EPrint("in16 0x%x failed! (0x%x)\n", StatusPort, status);
        goto clean;
    }

clean:
    
    //FLeave();
    return status;
}

//
// portio write (command) read 32 bit (status/value)
//
NTSTATUS
outIn32(
    _In_ HANDLE Device, 
    _In_ UINT16 CmdPort, 
    _In_ UINT16 StatusPort, 
    _In_ UINT32 Index, 
    _In_ PUINT32 Value
)
{
    //FEnter();

    NTSTATUS status = 0;

    status = out32(Device, CmdPort, Index);
    if ( status != 0 )
    {
        EPrint("out32 0x%x:0x%x failed! (0x%x)\n", CmdPort, Index, status);
        goto clean;
    }
    status = in32(Device, StatusPort, Value);
    if ( status != 0 )
    {
        EPrint("in32 0x%x failed! (0x%x)\n", StatusPort, status);
        goto clean;
    }

clean:
    
    //FLeave();
    return status;
}


//
// portio write (cmd) write 32 bit (value)
//
NTSTATUS
outOut32(
    _In_ HANDLE Device, 
    _In_ UINT16 CmdPort, 
    _In_ UINT16 StatusPort, 
    _In_ UINT32 Index, 
    _In_ UINT32 Value
)
{
    //FEnter();

    NTSTATUS status = 0;

    status = out32(Device, CmdPort, Index);
    if ( status != 0 )
    {
        EPrint("out32 0x%x:0x%x failed! (0x%x)\n", CmdPort, Index, status);
        goto clean;
    }
    status = out32(Device, StatusPort, Value);
    if ( status != 0 )
    {
        EPrint("out32 0x%x:0x%x failed! (0x%x)\n", StatusPort, Value, status);
        goto clean;
    }

clean:
    
    //FLeave();
    return status;
}


#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED

#define MEM_MAP_OUT_SIZE (sizeof(MAP_PA_OUT))

INT mapMemory(
    _In_ HANDLE Device,
    _In_ PVOID Address,
    _In_ UINT32 Size,
    _Out_ PVOID *MappedAddr,
    _Inout_ PVOID OutParams,
    _In_ UINT32 OutParamsSize
)
{
    FEnter();

    NTSTATUS status = 0;

    IO_STATUS_BLOCK iosb = { 0 };
    MAP_PA_IN mmioMapIn = { 0 };
    MAP_PA_OUT mmioMapOut = { 0 };
    
    mmioMapIn.Address = Address;
    mmioMapIn.Size = Size;
    DPrint("Address: %p\n", mmioMapIn.Address);
    DPrint("Size: 0x%x\n", mmioMapIn.Size);

    *MappedAddr = NULL;
    RtlZeroMemory(OutParams, OutParamsSize);
    if ( OutParamsSize < sizeof(mmioMapOut) )
        return STATUS_INVALID_PARAMETER;

    status = generateIoRequest(Device, IOCTL_MMIO_MAP, &iosb, &mmioMapIn, sizeof(mmioMapIn), &mmioMapOut, sizeof(mmioMapOut));
    if ( status != 0 )
    {
        EPrint("DeviceIO request IOCTL_MMIO_MAP failed! (0x%x)\n", status);
        goto clean;
    }
    *MappedAddr = mmioMapOut.LockedAddress;
    RtlCopyMemory(OutParams, &mmioMapOut, sizeof(mmioMapOut));

clean:
    FLeave();
    return status;
}

INT unmapMemory(
    _In_ HANDLE Device,
    _In_ PVOID MappedAddr,
    _In_ PVOID OutParams,
    _In_ UINT32 OutParamsSize
)
{
    FEnter();

    NTSTATUS status = 0;
    (MappedAddr);

    IO_STATUS_BLOCK iosb = { 0 };
    UNMAP_PA_IN mmioUnmapIn = { 0 };
    
    if ( OutParamsSize < sizeof(mmioUnmapIn) )
        return STATUS_INVALID_PARAMETER;

    RtlCopyMemory(&mmioUnmapIn, OutParams, sizeof(mmioUnmapIn));
    status = generateIoRequest(Device, IOCTL_MMIO_UNMAP, &iosb, &mmioUnmapIn, sizeof(mmioUnmapIn), NULL, 0);
    if ( status != 0 )
    {
        EPrint("DeviceIO request IOCTL_MMIO_UNMAP failed! (0x%x)\n", status);
        //goto clean;
    }
    FLeave();

    return status;
}
#endif
