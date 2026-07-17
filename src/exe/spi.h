#pragma once

NTSTATUS
commandSpiFromRCBA(
    _In_ HANDLE Device,
    _In_ PCOMMAND_SPI_PARAMS CommandSpiParams,
    _In_ UINT16 VendorId,
    _In_ UINT16 DeviceId,
    _In_ RCBA RCBA,
    _In_ PLOG Log
);

NTSTATUS
commandSpiFromDevice(
    _In_ HANDLE Device,
    _In_ UINT64 PciExBarAddress,
    _In_ PCOMMAND_SPI_PARAMS CommandSpiParams,
    _In_ PLOG Log
);

NTSTATUS
readSpiFromRCBA(
    _In_ HANDLE Device,
    _In_ PREAD_SPI_PARAMS ReadSpiParams,
    _In_ UINT16 VendorId,
    _In_ UINT16 DeviceId,
    _In_ RCBA RCBA,
    _In_ PLOG Log
);

NTSTATUS
readSpiFromDevice(
    _In_ HANDLE Device,
    _In_ UINT64 PciExBarAddress,
    _In_ PREAD_SPI_PARAMS ReadSpiParams,
    _In_ PLOG Log
);

NTSTATUS readSpiRegister(
    _In_ HANDLE Device,
    _In_ PVOID Address,
    _In_ UINT64 Size,
    _In_ PVOID Buffer,
    _In_ UINT32 BufferSize,
    _In_ PLOG Log
);

NTSTATUS dumpSpiFlash(
    _In_ HANDLE Device,
    _In_ PVOID SpiBarBase,
    _In_ PVOID SpiRegister,
    _In_ UINT32 SpiRegisterSize,
    _In_ PREAD_SPI_PARAMS ReadSpiParams,
    _In_ PLOG Log
);

#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
NTSTATUS readSpiFlashMemoryMm(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ UINT32 LinearAddress, // Offset to start
    _In_ UINT32 Size, // Size to read
    _Out_ PVOID OutputBuffer, // paged result buffer
    _In_ UINT32 OutputBufferSize, // paged result buffer size
    _Out_ PULONG BytesWritten,
    _In_ PVOID MappedSection
);

NTSTATUS readSpiFlashMemoryMmWtf(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ UINT32 LinearAddress, // Offset to start
    _In_ UINT32 Size, // Size to read
    _Out_ HANDLE OutFile, // file to write result
    _Out_ PULONG BytesWritten,
    _In_ PVOID MappedSection
);
#endif

#ifndef RW_PRIM_MEM_MAPPING_SUPPORTED
NTSTATUS readSpiFlashMemoryPA(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ UINT32 LinearAddress, // Offset to start
    _In_ UINT32 Size, // Size to read
    _Out_ PVOID OutputBuffer, // paged result buffer
    _In_ UINT32 OutputBufferSize, // paged result buffer size
    _Out_ PULONG BytesWritten
);

NTSTATUS readSpiFlashMemoryPaWtf(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ UINT32 LinearAddress, // Offset to start
    _In_ UINT32 Size, // Size to read
    _In_ HANDLE OutFile, // file to write result
    _Out_ PULONG BytesWritten
);
#endif

NTSTATUS issueSpiCommandLoop(
    _In_ HANDLE Device,
    _In_ PCOMMAND_SPI_PARAMS CommandSpiParams,
    _In_ PVOID RegisterBuffer,
    _In_ PVOID SpiBarBase,
    _In_opt_ PVOID MappedSection
);

#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
NTSTATUS issueSpiCommandMm(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ PCOMMAND_SPI_PARAMS CommandSpiParams,
    _In_ PVOID MappedSection,
    _In_opt_ PVOID Input,
    _In_ UINT32 InputSize,
    _Inout_opt_ PVOID Output,
    _In_ UINT32 OutputSize,
    _Out_opt_ PUINT32 OutputSizeWritten
);
#else
NTSTATUS issueSpiCommandPA(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ PCOMMAND_SPI_PARAMS CommandSpiParams,
    _In_opt_ PVOID Input,
    _In_ UINT32 InputSize,
    _Inout_opt_ PVOID Output,
    _In_ UINT32 OutputSize,
    _Out_opt_ PUINT32 OutputWritten
);
#endif



int dumpSpi(
    _In_ HANDLE Device,
    _In_ PCMD_PARAMS Params 
)
{
    FEnter();

    NTSTATUS status = 0;

    SIZE_T logBufferRestSize = 0;
    PCHAR logBufferPtr = NULL;

    PWCHAR filePath = NULL;
    UINT32 filePathCch = 0;
    
    LOG log = { 0 };
    log.Dir = Params->LogDir;
    log.BufferSize = PAGE_SIZE;

    //PVOID cfgBuffer = NULL;
    //UINT32 cfgBufferSize = PCI_CONFIG_SIZE;
    
    //PVOID barBuffer = NULL;
    //UINT32 barBufferSize = PAGE_SIZE;
    
    UINT64 pciExBarAddress = 0;
    UINT64 pciExBarSize = 0;

    BDF pchBdf = { 0 };
    
    READ_SPI_PARAMS readSpiParams = { 0 };
    
    SYSTEMTIME lt = { 0 };

    printf("Dumping SPI flash\n");
    
    GetLocalTime(&lt);
    printf("[Started %02u:%02u:%02u]\n\n", lt.wHour, lt.wMinute, lt.wSecond);

    //
    // init log buffer

    if ( Params->Flags.WriteToFile )
    {
        log.Buffer = malloc(log.BufferSize);
        if ( !log.Buffer )
            goto clean;

        logBufferRestSize = log.BufferSize;
        logBufferPtr = log.Buffer;

        PWCHAR fileBaseName = L"spi.log";
        filePathCch = (Params->LogDirCb/2) + (UINT32)wcslen(fileBaseName) + 1; // + separator (1) 
        filePath = malloc(filePathCch*2 + 2); // + \0\0 (2)
        if ( !filePath )
            goto clean;

        RtlStringCchPrintfW(filePath, filePathCch+1, L"%s\\%s", log.Dir, fileBaseName);

        status = kOpenFile(filePath, filePathCch, &log.File, OPEN_FOR_WRITE_ONLY, FILE_OVERWRITE_IF, 0);
        if ( status != 0 )
        {
            EPrint("Opening file \"%ws\" failed! (0x%x)\n", filePath, status);
            goto clean;
        }
    }
    
    //
    // init cfg buffer
    // not needed if only vendorId, deviceId and rcba are relevant

    //cfgBuffer = malloc(cfgBufferSize);
    //if ( !cfgBuffer )
    //{
    //    status = STATUS_NO_MEMORY;
    //    goto clean;
    //}


    //
    // init params

    readSpiParams.BDF = Params->PciIo.BDF;
    readSpiParams.BarId = Params->PciIo.BarId;
    readSpiParams.Rcbo = Params->PciIo.Rcbo;
    readSpiParams.Flags.ReadMode = Params->PciIo.Flags.ReadMode;
    readSpiParams.Flags.DumpBiosMode = Params->Flags.DumpBiosMode;
    readSpiParams.Flags.Regions = Params->Flags.Regions;
    readSpiParams.Base = Params->SpiIo.Base;
    readSpiParams.Limit = Params->SpiIo.Limit;
    
    
    //
    // get pciExBarAddress
    // to us mmio cfg read if possible
    // and no desired read mode is forced
    
    if ( readSpiParams.Flags.ReadMode == 0 
      || readSpiParams.Flags.ReadMode == READ_CFG_MODE_MM )
    {
        status = getPciExBarValueMCFG(Device, &pciExBarAddress, &pciExBarSize);
        if ( status != 0 )
        {
            EPrint("getPciExBarValueMCFG failed! (0x%x)\n", status);
            status = getPciExBarValuePortIo(Device, &pciExBarAddress, &pciExBarSize);
            
            if ( status != 0 )
            {
                EPrint("getPciExBarValuePortIo failed! (0x%x)\n", status);
                if ( readSpiParams.Flags.ReadMode == READ_CFG_MODE_MM )
                    goto clean;
                else
                    readSpiParams.Flags.ReadMode = READ_CFG_MODE_PORT;
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
        
            //pciIoParams->PciExBarAddress = pciExBarAddress;
            //pciIoParams->PciExBarSize = pciExBarSize;
            readSpiParams.Flags.ReadMode = READ_CFG_MODE_MM;
        }
    }
    
    //
    // read cfg values of 00:1f:00 (PCH/LPC)

    pchBdf.Bus = 0;
    pchBdf.Device = 0x1f;
    pchBdf.Function = 0;

    PCHAR lpcStr = NULL;

    UINT32 venDevId = (UINT32)-1;
    UINT16 vendorId = (UINT16)-1;
    UINT16 deviceId = (UINT16)-1;
    RCBA rcba = { 0 };
    
    //
    // read vendor device id value at once

    UINT32 offset = 0;
    if ( readSpiParams.Flags.ReadMode == READ_CFG_MODE_PORT )
    {
        UINT32 index = PCI_CFG_PORT_IO_ADDR(pchBdf.Bus, pchBdf.Device, pchBdf.Function, offset);
        status = outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, index, &venDevId);
        if ( status != 0 )
        {
            EPrint("outIn32 0x%x:0x%x => 0x%x failed! (0x%x)\n", PCI_CONFIG_COMMAND_PORT, index, PCI_CONFIG_STATUS_PORT, status);
            goto clean;
        }
    }
    else
    {
        UINT64 index = PCI_CFG_MM_ADDR(pciExBarAddress, pchBdf.Bus, pchBdf.Device, pchBdf.Function, offset);
        status = readPA(Device, index, CACHE_TYPE, &venDevId, 4, READ_PA_MODE_4);
        if ( status != 0 )
        {
            EPrint("readPA4 0x%llx failed! (0x%x)\n", index, status);
            goto clean;
        }
    }
    DPrint("venDevId: 0x%x\n", venDevId);

    // split into vendor and device
    vendorId = venDevId & 0xFFFF;
    deviceId = (venDevId>>0x10) & 0xFFFF;
    DPrint("vendorId: 0x%x\n", vendorId);
    DPrint("deviceId: 0x%x\n", deviceId);

    //
    // read rcba value
    // could check chip series to know, if rcba is to be expected to prevent false positives

    offset = 0xF0;
    if ( readSpiParams.Flags.ReadMode == READ_CFG_MODE_PORT )
    {
        UINT32 index = PCI_CFG_PORT_IO_ADDR(pchBdf.Bus, pchBdf.Device, pchBdf.Function, offset);
        status = outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, index, (PUINT32)&rcba);
        if ( status != 0 )
        {
            EPrint("outIn32 0x%x:0x%x => 0x%x failed! (0x%x)\n", PCI_CONFIG_COMMAND_PORT, index, PCI_CONFIG_STATUS_PORT, status);
            goto clean;
        }
    }
    else
    {
        UINT64 index = PCI_CFG_MM_ADDR(pciExBarAddress, pchBdf.Bus, pchBdf.Device, pchBdf.Function, offset);
        status = readPA(Device, index, CACHE_TYPE, (PUINT32)&rcba, 4, READ_PA_MODE_4);
        if ( status != 0 )
        {
            EPrint("readPA4 0x%llx failed! (0x%x)\n", index, status);
            goto clean;
        }
    }
    DPrint("rcba: 0x%x\n", *(PUINT32)&rcba);
    DPrint("  Enable: 0x%x\n", rcba.Enable);
    DPrint("  Reserved1: 0x%x\n", rcba.Reserved1);
    DPrint("  BA: 0x%x\n", rcba.BA);


    //
    // check vendor
    // currently just intel is supported

    if ( vendorId != PCI_VENDORID_INTEL )
    {
        status = STATUS_NOT_SUPPORTED;
        EPrint("Only Intel supported yet! (0x%x)\n", status);

        goto clean;
    }
    
    
    // get spi interface from manual given bdf bar values
    if ( Params->Flags.SpiLocation == SPI_LOC_BDF )
    {
        status = readSpiFromDevice(Device, pciExBarAddress, &readSpiParams, &log);
    }
    // get spi interface from manual given rcba offset
    else if ( Params->Flags.SpiLocation == SPI_LOC_RCBA )
    {
        status = readSpiFromRCBA(Device, &readSpiParams, vendorId, deviceId, rcba, &log);
    }
    else
    {
        readSpiParams.Rcbo = RCBA_SPI_OFFSET; // use default offset

        //
        // Switch over known device ids to get the method that locates the spi register.
        // It's either rcba+offset or device 00:1f:05 bar[0]
        // If deviceId is not known
        //   try autodetection by
        //   checking rcba.Enable 
        //   and choose rcba or device method depending on the result.

        switch ( deviceId )
        {
            // knwon rcba cases
            case 0x1c4f: // 5 Series QM67 Express Chipset LPC Controller
            case 0x3b09: // HM55 Chipset LPC Interface Controller (6/7 series)
            case 0x9c43: // 8 Series LPC Controller
            case 0x9cc3: // 8 Series Wildcat Point-LP LPC Controller
                DPrint("Read SPI bar from RCBA + offset!\n")
                status = readSpiFromRCBA(Device, &readSpiParams, vendorId, deviceId, rcba, &log);
                break;

            // known bdf 00:1f:05::bar[0] cases
            case 0xa154: // CM238 Chipset LPC/eSPI Controller
            case 0x9d48: // Sunrise Point-LP LPC Controller
            case 0x9d4e: // Sunrise Point LPC/eSPI Controller
            case 0xa2c6: // pch 200 series
            case 0xa308: // 300 Series Chipset Family LPC Controller
                DPrint("Read SPI bar from 00:1f:05!\n")
                switch ( deviceId )
                {
                    case 0xa154: lpcStr = "100 Series: CM238 Chipset LPC"; break;
                    case 0x9d48: lpcStr = "200 Series: Sunrise Point-LP LPC Controller"; break;
                    case 0x9d4e: lpcStr = "200 Series: Sunrise Point LPC/eSPI Controller"; break;
                    case 0xa2c6: lpcStr = "200 Series"; break;
                    case 0xa308: lpcStr = "300 Series: Chipset Family LPC Controller"; break;
                    default: break; // should not happen
                }
                DPrint("  %s\n", lpcStr);

                readSpiParams.BDF.Bus = 0;
                readSpiParams.BDF.Device = 0x1f;
                readSpiParams.BDF.Function = 5;
                readSpiParams.BarId = 0;
                status = readSpiFromDevice(Device, pciExBarAddress, &readSpiParams, &log);

                break;

            // auto select method
            default:
                //
                // not known device,
                // choose rcba if rcba.enable
                // else choose device 00:1f:05
                // 

                DPrint("Not known DeviceId: 0x%x. (0x%x)\n", deviceId, status);
                DPrint("Trying auto detection\n");
            
                if ( rcba.Enable )
                {
                    // just VendorId [0:1], DeviceId [2:3] and RCBA [f0:f7] are needed of cfg
                    status = readSpiFromRCBA(Device, &readSpiParams, vendorId, deviceId, rcba, &log);
                }
                else
                {
                    readSpiParams.BDF.Bus = 0;
                    readSpiParams.BDF.Device = 0x1f;
                    readSpiParams.BDF.Function = 5;

                    status = readSpiFromDevice(Device, pciExBarAddress, &readSpiParams, &log);
                }
                break;
        }
    }
    
    GetLocalTime(&lt);
    printf("\n[Finished %02u:%02u:%02u]\n", lt.wHour, lt.wMinute, lt.wSecond);

clean:
    //if (cfgBuffer)
        //free(cfgBuffer);
    if ( log.Buffer )
        free(log.Buffer);
    if ( log.File )
        ZwClose(log.File);
    if ( filePath )
        free(filePath);
    //if ( barBuffer )
        //free(barBuffer);
    
    FLeave();
    return status;
}

NTSTATUS
readSpiFromRCBA(
    _In_ HANDLE Device,
    _In_ PREAD_SPI_PARAMS ReadSpiParams,
    _In_ UINT16 VendorId,
    _In_ UINT16 DeviceId,
    _In_ RCBA RCBA,
    _In_ PLOG Log
)
{
    FEnter();

    NTSTATUS status = 0;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;

    PVOID rcba_ba = NULL;
    UINT32 spiOffset = ReadSpiParams->Rcbo;
    PVOID spiAddress = NULL;
    UINT64 spiSize = RCBA_SPI_SIZE;
    
    PVOID spiRegisterBuffer = NULL;
    ULONG spiRegisterBufferSize = PAGE_SIZE;
    
    //PVOID rcba = NULL;

    ULONG seriesNr = 0;

    if ( VendorId != PCI_VENDORID_INTEL )
    {
        status = STATUS_NOT_SUPPORTED;
        EPrint("Not supported vendor id 0x%x! (0x%x)\n", VendorId, status);
        goto clean;
    }

    switch ( DeviceId )
    {
        case 0x1c4f: // 5 Series QM67 Express Chipset LPC Controller
        case 0x3b09: // 7 Series LPC Controller
        case 0x9c43: // 8 Series LPC Controller
        case 0x9cc3: // 8 Series Wildcat Point-LP LPC Controller
        
            switch ( DeviceId ) {
                case 0x1c4f: 
                    seriesNr = 5; break;
                case 0x3b09: 
                    seriesNr = 6; break;
                case 0x9c43: 
                case 0x9cc3: 
                    seriesNr = 8; break;
                default:
                    goto clean;
            }
        
            DPrint("%u Series LPC Controller\n", seriesNr);
            
            if ( !RCBA.Enable )
            {
                status = STATUS_UNSUCCESSFUL;
                EPrint("RCBA not enabled! (0x%x)\n", status);
                goto clean;
            }

            rcba_ba = (PVOID)( (*(PUINT32)&RCBA) & (RCBA_BA_MASK) );
            // spiOffset = RCBA_SPI_OFFSET; // could be overwritten here

            break;
        
        default:
            // not known but trying to dump anyway from the hardcoded offset
            
            //status = STATUS_UNSUCCESSFUL;
            EPrint("Unknown device 0x%x! (0x%x)\n", DeviceId, status);
            
            // using it as default

            if ( !RCBA.Enable )
            {
                status = STATUS_UNSUCCESSFUL;
                EPrint("RCBA not enabled! (0x%x)\n", status);
                goto clean;
            }

            rcba_ba = (PVOID)( (*(PUINT32)&RCBA) & (RCBA_BA_MASK) );


            //goto clean;
            break;
    }

    spiAddress = (PVOID)((SIZE_T)rcba_ba + spiOffset);

    DPRINT_PTR(rcba_ba, "");
    DPRINT_INT_H(spiOffset, "");
    DPRINT_PTR(spiAddress, "");
    DPRINT_INT_H(spiSize, "");

    if ( Log->File )
    {
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "  rcba_ba: %p\r\n"
                            "  spiOffset: 0x%x\r\n"
                            "  spiAddress: %p\r\n"
                            "  spiSize: 0x%llx\r\n",
                            rcba_ba,
                            spiOffset,
                            spiAddress,
                            spiSize);
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }


    //
    // allocate register buffer

    spiRegisterBufferSize = (UINT32)spiSize;
    spiRegisterBuffer = malloc(spiRegisterBufferSize);
    if ( !spiRegisterBuffer )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }
    
    //
    // read spi register

    status = readSpiRegister(Device, spiAddress, spiSize, spiRegisterBuffer, spiRegisterBufferSize, Log);
    if ( status != 0 )
    {
        EPrint("readSpiRegister failed! (0x%x)\n", status);
        goto clean;
    }
    

    //
    // dump spi 

    PVOID spiBarBase = getBarBaseAddress((PBAR)&spiAddress, NULL);
    if ( !spiBarBase )
    {
        status = STATUS_UNSUCCESSFUL; 
        EPrint("spiBarBase is NULL! (0x%x)\n", status);
        goto clean;
    }

    status = dumpSpiFlash(Device, spiBarBase, spiRegisterBuffer, spiRegisterBufferSize, ReadSpiParams, Log);
    if ( status != 0 )
    {
        EPrint("dumpSpiFlash failed! (0x%x)\n", status);
        goto clean;
    }

clean:
    if ( Log->File )
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    if ( spiRegisterBuffer )
        free(spiRegisterBuffer);

    FLeave();
    return status;
}

NTSTATUS
readSpiFromDevice(
    _In_ HANDLE Device,
    _In_ UINT64 PciExBarAddress,
    _In_ PREAD_SPI_PARAMS ReadSpiParams,
    _In_ PLOG Log
)
{
    FEnter();

    NTSTATUS status = 0;
    ULONG i;

    UINT32 offset;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;
    
    PVOID cfgBuffer = NULL; // nonpaged 
    ULONG cfgBufferSize = PCI_CONFIG_SIZE;
    PVOID registerBuffer = NULL; // paged
    ULONG registerBufferSize = 0;
    
    PBDF bdf = &ReadSpiParams->BDF;
    UINT16 barId = ReadSpiParams->BarId;
    
    //
    // allocate registerBuffer

    DPrint("cfgBufferSize: 0x%x\n", cfgBufferSize);
    cfgBuffer = malloc(cfgBufferSize);
    if ( !cfgBuffer )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }


    //
    // read pciExBarAddress
    // use mmio cfg read if possible
    // should be set already
        
    //if ( PciExBarAddress )
    //    ReadSpiParams->Flags.ReadMode = READ_CFG_MODE_MM;
    //else
    //    ReadSpiParams->Flags.ReadMode = READ_CFG_MODE_PORT;



    //
    // read entire cfg for log info,
    // just bar[x] is actually needed 

    if ( ReadSpiParams->Flags.ReadMode == READ_CFG_MODE_MM )
    {
        UINT64 pa = PCI_CFG_MM_ADDR(PciExBarAddress, bdf->Bus, bdf->Device, bdf->Function, 0);
        status = readPA(Device, pa, CACHE_TYPE, cfgBuffer, cfgBufferSize, READ_PA_MODE_CHUNKED_4);
        //status = readPAMmIoMapped4(Device, pa, CACHE_TYPE, cfgBuffer, cfgBufferSize);
        if ( status != 0 )
        {
            EPrint("readPAMmIoMapped4 config %02x:%02x:%02x failed! (0x%x)\n",bdf->Bus, bdf->Device, bdf->Function,  status);
            goto clean;
        }
    }
    else
    {
        ULONG parts = cfgBufferSize / 4;
        ULONG rest = cfgBufferSize % 4;

        PUINT32 bufferPtr = (PUINT32)cfgBuffer;
        UINT32 index = 0;
        offset = 0;
        UINT32 value32 = 0;

        if ( rest != 0 )
        {
            status = STATUS_INVALID_PARAMETER_3;
            EPrint("Length has to be 4-Byte aligned! (0x%x)\n", status);
            goto clean;
        }
    
        //RtlZeroMemory(cfgBuffer, cfgBufferSize);
        
        // read in 32 bit chunks
        for ( i = 0; i < parts; i++ )
        {
            index = PCI_CFG_PORT_IO_ADDR(bdf->Bus, bdf->Device, bdf->Function, offset);
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
#ifdef DEBUG_PRINT_MEM
    DPrintMemCol8(cfgBuffer, cfgBufferSize, 0);
#endif
    if ( Log->File )
    {
        status = logCfg(bdf, cfgBuffer, cfgBufferSize, Log);
        if ( status != 0 )
        {
            EPrint("Writing config to file failed! (0x%x)\n", status);
            //goto clean;
        }
    }

    if ( isReadDenied(cfgBuffer, 0x40) )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("Spi Cfg is read protected!\n");
        goto clean;
    }


    //
    // get spi bar address

    UINT32 spiBarAddress = 0;
    offset = (UINT32)offsetof(PCI_CONFIG_GENERAL, Bar[barId]);

    spiBarAddress = ((PPCI_CONFIG_GENERAL)cfgBuffer)->Bar[barId];
    
    DPrint("bar offset: 0x%x\n", offset);
    DPrint("bar[%u]: %p\n", barId, (PVOID)spiBarAddress);
    
    if ( ! BAR_VALUE_VALID(spiBarAddress) )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("No valid bar value found! (0x%x)\n", status);
        goto clean;
    }

    printf("spiBarAddress: %p\n", (PVOID)spiBarAddress);
    

    
    //
    // calculate size of bar memory
    // [considering spi bar is not used now]
    // usually it's a page size

    UINT64 spiBarSize = (UINT64)-1;
    if ( ReadSpiParams->Flags.ReadMode == READ_CFG_MODE_MM )
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
        status = calculateBarSizeMMioMapped(Device, PciExBarAddress, bdf, offset, &spiBarSize);
#else
        status = calculateBarSizePA(Device, PciExBarAddress, bdf, offset, &spiBarSize);
#endif
    else if ( ReadSpiParams->Flags.ReadMode == READ_CFG_MODE_PORT )
        status = calculateBarSizePortIo(Device, bdf, offset, &spiBarSize);
    printf("Spi bar size: 0x%llx\n", spiBarSize);
    if ( status != 0 || spiBarSize == 0 )
        spiBarSize = PAGE_SIZE;

    //
    // alloc spi register buffer

    registerBufferSize = (UINT32)spiBarSize;
    registerBuffer = malloc(registerBufferSize);
    if ( !registerBuffer )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }
    
    //
    // read register

    PBAR bar = (PBAR)&spiBarAddress;
    PVOID spiBarBase = getBarBaseAddress(bar, NULL);
    if ( !spiBarBase )
    {
        status = STATUS_UNSUCCESSFUL; 
        EPrint("spiBarBase is NULL! (0x%x)\n", status);
        goto clean;
    }

    if ( Log->File )
    {
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "SpiBar (%s, %s)\r\n", 
                            BAR_TYPE_STR(bar),
                            BAR_ADDRESS_WIDTH(bar));
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }

    status = readSpiRegister(Device, spiBarBase, spiBarSize, registerBuffer, registerBufferSize, Log);
    if ( status != 0 )
    {
        EPrint("readSpiRegister failed! (0x%x)\n", status);
        goto clean;
    }


    //
    // dump spi 
    
    //PVOID spiBarBase = getBarBaseAddress((PBAR)&spiBarAddress, NULL);
    status = dumpSpiFlash(Device, spiBarBase, registerBuffer, registerBufferSize, ReadSpiParams, Log);
    if ( status != 0 )
    {
        EPrint("dumpSpiFlash failed! (0x%x)\n", status);
        goto clean;
    }

clean:
    if ( Log->File )
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    if ( cfgBuffer )
        free(cfgBuffer);
    if ( registerBuffer )
        free(registerBuffer);

    FLeave();
    return status;
}

NTSTATUS readSpiRegister(
    _In_ HANDLE Device,
    _In_ PVOID Address,
    _In_ UINT64 Size,
    _In_ PVOID Buffer,
    _In_ UINT32 BufferSize,
    _In_ PLOG Log
)
{
    FEnter();

    NTSTATUS status = 0;
    NTSTATUS logStatus = 0;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;

    if ( Size > BufferSize )
    {
        status = STATUS_BUFFER_TOO_SMALL;
        EPrint("Register buffer too small! (0x%x)", status);
        goto clean;
    }
    
    if ( Log->File )
    {
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "SpiRegister\r\n");
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }
    status = readMemoryBar(Device, (UINT64)Address, Buffer, (UINT32)Size);
    if ( status != 0 )
    {
        EPrint("readMemoryBar failed! (0x%x)\n", status);
        goto clean;
    }
    if ( Log->File )
        logStatus = logBarBuffer(Address, Buffer, (UINT32)Size, Log);
    

    //
    // test for read protection

    if ( isReadDenied(Buffer, 0x10) )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("Spi Bar not readable! (0x%x)\n", status);
        goto clean;
    }



    //
    // log formatted

    if ( Log->File )
    {
        status = logSpiRegister(Buffer, (UINT32)Size, Log);
        if ( status != 0 )
        {
            EPrint("logSpiRegister failed! (0x%x)\n", status);
            goto clean;
        }
    }

clean:
    if ( Log->File )
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    FLeave();
    return status;
}

// 
// read bios from pa - mmio method
//
// Could be read directly to file
//
FORCEINLINE
NTSTATUS dumpPhysicalMMIo(
    _In_ HANDLE Device, 
    _In_ PHYSICAL_ADDRESS Address, 
    _Out_writes_(Size) PVOID Buffer,
    _In_ UINT32 Size,
    _In_ PWCHAR LogDir
)
{
    FEnter();

    NTSTATUS status = 0;

    WCHAR filename[0x100];
    HANDLE file = NULL;
    ULONG bytesWritten;
    
    status = readPA(Device, Address.QuadPart, CACHE_TYPE, Buffer, Size, READ_PA_MODE_CHUNKED_4);
    //status = readPAMmIoMapped4(Device, Address.QuadPart, CACHE_TYPE, Buffer, Size);
    //status = readPAMmIoMapped4Wtf(Device, Address.QuadPart, CACHE_TYPE, file);
    if ( status != 0 )
    {
        EPrint("Reading Bios pa mmio method failed! (0x%x)\n", status);
        goto clean;
    }
    DPrint("  mmio read finished\n");
    
    RtlZeroMemory(filename, 0x100);
    RtlStringCchPrintfW(filename, 0x100, L"%s\\spi-bios-mmio.dmp", LogDir);
    DPrint("  opening file %ws\n", filename);
    status = kOpenFile(filename, (ULONG)wcslen(filename), &file, OPEN_FOR_WRITE_ONLY, FILE_OVERWRITE_IF, 0);
    if ( !status )
    {
        DPrint("  writing\n");
        kWriteFile(file, Buffer, Size, &bytesWritten);
        DPrint("  written\n");
        ZwClose(file);
    }

clean:
    FLeave();
    return status;
}

//
// Calculate spi size depending on highest limit
// because BIOS is not always last, i.e. dell 5540.
// More precisely BIOS is last on chip 1 but there is chip 2 with ME on it.
//
UINT32 getSpiSize(
    _In_ PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP SpiRegisterData,
    _In_ UINT32 NrRegions
)
{
    UINT32 i = 0;
    UINT32 highestLimit = 0;
    UINT32 size = 0;

    for ( i = 0; i < NrRegions; i++ )
    {
        // check validity of values
        // check is not sufficient because some vendors don't initialize their unused values
        if ( SpiRegisterData->FREG[i].RB == FD_INVALID_REGION_BASE 
          && SpiRegisterData->FREG[i].RL == FD_INVALID_REGION_LIMIT )
        {
            DPrint("Unfilled region!\n");
            continue;
        }
        if ( *(PUINT32)&SpiRegisterData->FREG[i] == (UINT32)-1 )
        {
            EPrint("Invalid region!\n");
            break;
        }

        UINT32 regBase = REGION_BASE_ADDRESS(SpiRegisterData->FREG[i]);
        UINT32 regLimit = REGION_LIMIT(SpiRegisterData->FREG[i]);
        
        // some more validity check
        if ( regBase > regLimit )
        {
            DPrint("Invalid region!\n");
            continue;
        }
        
        if ( highestLimit < regLimit )
            highestLimit = regLimit;
    }

    if ( highestLimit )
    {
        size = highestLimit+1;
    }

    return size;
}

// warning C6387: 'mmioMapOut.LockedAddress' could be '0'
DISABLE_WARNING( 6387 )
NTSTATUS dumpSpiFlash(
    _In_ HANDLE Device,
    _In_ PVOID SpiBarBase,
    _In_ PVOID SpiRegister,
    _In_ UINT32 SpiRegisterSize,
    _In_ PREAD_SPI_PARAMS ReadSpiParams,
    _In_ PLOG Log
)
{
    FEnter();

    NTSTATUS status = 0;

    //UINT32 index = 0;
    //UINT32 offset;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;
    
    PVOID fdrBuffer = NULL;
    ULONG fdrBufferSize = FLASH_DESRIPTOR_MIN_SIZE;
    
    PVOID flashRegionBuffer = NULL;
    SIZE_T flashRegionBufferSize = 0;
    
    WCHAR filename[0x100];
    HANDLE dumpFile = NULL;
    ULONG bytesWritten;
    
    ULONG flashRegionBytesWritten = 0;

#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    PVOID lockedSpiBar = NULL;
    UINT8 mapOutParams[MEM_MAP_OUT_SIZE] = { 0 };
#endif
    
    UINT32 regId;
    UINT32 dumpRegionStartId = FREG_BIOS_ID;
    

    (SpiRegisterSize);

    if ( !logBufferPtr || !logBufferRestSize )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("No log buffer! (0x%x)\n", status);
        goto clean;
    }

    //
    // use SPIBAR memory to do the read Flash Memory control thing

    // allocate fdrBuffer

    fdrBuffer = malloc(fdrBufferSize);
    if ( !fdrBuffer )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }
    
    //
    // map spi bar (mmio)
    
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    status = mapMemory(Device, SpiBarBase, 0x1000, &lockedSpiBar, &mapOutParams, MEM_MAP_OUT_SIZE);
    if ( status != 0 )
    {
        EPrint("mapMemory request failed! (0x%x)\n", status);
        goto clean;
    }
#endif

    //
    // read fdr

    printf("Reading Flash Descriptor [%x:%x]\n", 0, FLASH_DESRIPTOR_MIN_SIZE);
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    status = readSpiFlashMemoryMm(Device, SpiRegister, SpiBarBase, 0, FLASH_DESRIPTOR_MIN_SIZE, fdrBuffer, fdrBufferSize, &flashRegionBytesWritten, lockedSpiBar);
#else
    status = readSpiFlashMemoryPA(Device, SpiRegister, SpiBarBase, 0, FLASH_DESRIPTOR_MIN_SIZE, fdrBuffer, fdrBufferSize, &flashRegionBytesWritten);
#endif
    if ( status != 0 )
    {
        EPrint("Reading Flash Descriptor failed! (0x%x)\n", status);
        goto clean;
    }
    printf("Done\n");


    // 
    // write plain and formatted flash descriptor to log 
    // and if possible to user buffer
    // and in binary to file
    
    UINT32 nrComponents = (UINT32)-1;
    UINT32 nrRegions = (UINT32)-1;

    PFLASH_DESCRIPTOR fd = (PFLASH_DESCRIPTOR)((SIZE_T)fdrBuffer + ICH_FLASH_DESCRIPTOR_SIG_OFFSET);
    if ( fd->Signature != FLASH_DESRIPTOR_MODE_SIG )
    {
        fd = (PFLASH_DESCRIPTOR)((SIZE_T)fdrBuffer + PCH_FLASH_DESCRIPTOR_SIG_OFFSET);
        if ( fd->Signature != FLASH_DESRIPTOR_MODE_SIG )
        {
            status = STATUS_UNSUCCESSFUL;
            EPrint("Wrong signature 0x%x! (0x%x)\n", fd->Signature, status);
            goto clean;
        }
    }
    nrComponents = fd->FLMAP0.NC+1;
    nrRegions = (fd->FLMAP0.NR) ? fd->FLMAP0.NR+1 : 0;

    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                    "0x%x Flash Descriptor Data bytes\r\n", 
                    fdrBufferSize);
    LogBytes(NULL, NULL, fdrBuffer, fdrBufferSize, (PVOID)0, TRUE, Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "\r\n");
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    logFlashDescriptor(fdrBuffer, fdrBufferSize, Log);

    // write to file, cause already spi cycled
    if ( ReadSpiParams->Flags.Regions & 1 )
    {
        RtlZeroMemory(filename, 0x100);
        RtlStringCchPrintfW(filename, 0x100, L"%s\\spi-fd.dmp", Log->Dir);
        status = kOpenFile(filename, (ULONG)wcslen(filename), &dumpFile, OPEN_FOR_WRITE_ONLY, FILE_OVERWRITE_IF, 0);
        if ( !status )
        {
            kWriteFile(dumpFile, fdrBuffer, fdrBufferSize, &bytesWritten);
            ZwClose(dumpFile);
        }
    }


    //
    // read regions with spi commands

    // check signature
    if ( !isValidFlashDescriptor(fdrBuffer) )
    {
        EPrint("Not a valid flash descriptor\n");
        goto clean;
    }
    
    
    //
    // take values from spiRegister.BFPREG
    // FlashDescriptor.FREG[0] could also be used

    PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP spiRegisterData = (PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP)SpiRegister;

    if ( spiRegisterData->BFPREG.RB == FD_INVALID_REGION_BASE
      || spiRegisterData->BFPREG.RL == 0
      || spiRegisterData->BFPREG.RB > spiRegisterData->BFPREG.RL
      || *(PUINT32)&spiRegisterData->BFPREG == (UINT32)-1 )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("Invalid BFPREG 0x%x! (0x%x)\n", *(PUINT32)&spiRegisterData->BFPREG, status);
        goto logOtherRegions;
    }
    DpVar(UINT32, biosSize, (REGION_SIZE(spiRegisterData->BFPREG)));
    DPRINT_INT_H(biosSize, "");
    UINT32 biosBase = REGION_BASE_ADDRESS(spiRegisterData->BFPREG);
    DpVar(UINT32, biosLimit, (REGION_LIMIT(spiRegisterData->BFPREG)));
    // iterate regs and get highest limit or sum Component1Density + Component2Density
    UINT32 spiSize = getSpiSize(spiRegisterData, SPI_RAM_MAX_NR_FLASH_REGIONS);

    DPRINT_INT_H(biosBase, "");
    DPRINT_INT_H(biosLimit, "");
    DPRINT_INT_H(spiSize, "");


    //
    // dump range of spi into one blob, if wanted (/ft <base> <to>)
    
    if ( ReadSpiParams->Base != FD_INVALID_REGION_BASE 
      && ReadSpiParams->Limit != FD_INVALID_REGION_LIMIT 
      && ReadSpiParams->Limit > ReadSpiParams->Base )
    {
        UINT32 base = ReadSpiParams->Base;
        UINT32 limit = ReadSpiParams->Limit;
        if ( limit > spiSize )
        {
            if ( spiSize <= base )
            {
                status = STATUS_INVALID_PARAMETER;
                EPrint("Invalid base\n");
                goto clean;
            }
            limit = spiSize;
        }

        UINT32 reqSize = limit - base;
        DPRINT_INT_H(reqSize, "");

        //if ( reqSize > flashRegionBufferSize )
        //{
        //    status = reAllocVirtMem(&flashRegionBuffer, &flashRegionBufferSize, reqSize);
        //    if ( status != 0 )
        //    {
        //        goto clean;
        //    }
        //}
        
        printf("Reading spi rom from 0x%x to 0x%x\n", base, limit);
        DPRINT_PTR(flashRegionBuffer, "");
        DPRINT_INT_H(flashRegionBufferSize, "");

        // open file
        RtlZeroMemory(filename, 0x100);
        RtlStringCchPrintfW(filename, 0x100, L"%s\\spi-%x-%x.dmp", Log->Dir, base, limit);
        DPrint("filename: %ws\n", filename);
        status = kOpenFile(filename, (ULONG)wcslen(filename), &dumpFile, OPEN_FOR_WRITE_ONLY, FILE_OVERWRITE_IF, 0);
        DPrint("kOpenFile status: 0x%x\n", status);
        if ( status != 0 )
        {
            EPrint("Opening dump file \"%ws\" to write failed! (0x%x)\n", filename, status);
            goto clean;
        }
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
        //status = readSpiFlashMemoryMm(Device, SpiRegister, SpiBarBase, base, reqSize, flashRegionBuffer, (UINT32)flashRegionBufferSize, &flashRegionBytesWritten, ReadSpiParams, lockedSpiBar);
        status = readSpiFlashMemoryMmWtf(Device, SpiRegister, SpiBarBase, base, reqSize, dumpFile, &flashRegionBytesWritten, lockedSpiBar);
#else
        status = readSpiFlashMemoryPaWtf(Device, SpiRegister, SpiBarBase, base, reqSize, dumpFile, &flashRegionBytesWritten);
#endif
        if ( status != 0 )
        {
            EPrint("Reading SPI chip failed! (0x%x)\n", status);
            goto clean;
        }
        ZwClose(dumpFile);
        dumpFile = NULL;
        DPrint("Success!\n");
        printf("Done!\n");
        DPrint("Read 0x%x bytes of spi!\n", flashRegionBytesWritten);
        
        //*(PUINT32)&ReadSpiParams->StatusInfo |= MAX_REGION_MASK;
        
        // set StatusInfo.Reg[i]Read
        //if ( i < SPI_RAM_MAX_NR_FLASH_REGIONS )
        //{
        //    *(PUINT32)&ReadSpiParams->StatusInfo |= (1 << i);
        //}
        
        // 
        // write plain spi bytes 
    
        //if ( flashRegionBytesWritten )
        //{
        //    RtlZeroMemory(filename, 0x100);
        //    RtlStringCchPrintfW(filename, 0x100, L"%s\\spi-spi-%x-%x.dmp", Log->Dir, base, limit);
        //    DPrint("filename: %ws\n", filename);
        //    status = kOpenFile(filename, (ULONG)wcslen(filename), &dumpFile, OPEN_FOR_WRITE_ONLY, FILE_OVERWRITE_IF, 0);
        //    DPrint("kOpenFile status: 0x%x\n", status);
        //    if ( status != 0 )
        //    {
        //        EPrint("Opening dump file \"%ws\" to write failed! (0x%x)\n", filename, status);
        //        goto clean;
        //    }
        //
        //    NTSTATUS writeStatus = kWriteFile(dumpFile, flashRegionBuffer, flashRegionBytesWritten, &bytesWritten);
        //    DPrint("kWriteFile status: 0x%x\n", writeStatus);
        //    DPrint("0x%x bytes written to dump file!\n", bytesWritten);
        //    ZwClose(dumpFile);
        //}

        // skip region dumping
        goto clean;
    };


logOtherRegions:
    ;

    //
    // read all valid flash regions
    // check readability over Master[i] record ??
    // 
    // skip Flash Descriptor and maybe BIOS (if manually mmio dumped before)
    
    PWCHAR regLogName[SPI_RAM_MAX_NR_FLASH_REGIONS] = {
        L"fd",
        L"bios",
        L"me",
        L"gbe",
        L"pd",
        L"reg[5]",
        L"reg[6]",
        L"reg[7]",
        L"ec",
        L"reg[9]",
        L"reg[10]",
        L"reg[11]",
    };
    
    // nrRegions = max(nrRegions, SPI_RAM_MAX_NR_FLASH_REGIONS);
    // NR is not filled with valid values either
    // FD (0) and Bios (1) already dumped, => start at me (2)
    //nrRegions = (nrRegions) ? nrRegions : SPI_RAM_MAX_NR_FLASH_REGIONS;
    nrRegions = SPI_RAM_MAX_NR_FLASH_REGIONS;
    printf("Reading regions\n");
    DPrint("nrRegions: 0x%x\n", nrRegions);
    DPrint("region mask: 0x%x\n", ReadSpiParams->Flags.Regions);
    for ( regId = dumpRegionStartId; regId < nrRegions; regId++ )
    {
        printf("%u / %u\n", (regId+1), nrRegions);

        // check validity of values
        // check is not sufficient because some vendors don't initialize their unused values
        if ( spiRegisterData->FREG[regId].RB == FD_INVALID_REGION_BASE 
          && spiRegisterData->FREG[regId].RL == FD_INVALID_REGION_LIMIT )
        {
            EPrint("Unfilled region!\n");
            continue;
        }
        if ( *(PUINT32)&spiRegisterData->FREG[regId] == (UINT32)-1 )
        {
            EPrint("Invalid region!\n");
            break;
        }

        UINT32 regBase = REGION_BASE_ADDRESS(spiRegisterData->FREG[regId]);
        UINT32 regLimit = REGION_LIMIT(spiRegisterData->FREG[regId]);
        UINT32 regSize = REGION_SIZE(spiRegisterData->FREG[regId]);
        
        // some more validity check
        if ( regBase > regLimit )
        {
            EPrint("Invalid region!\n");
            continue;
        }
        
        // check if user wants to dump this region
        if ( ! (ReadSpiParams->Flags.Regions & (1 << regId) ) )
        {
            printf("Skipping unwanted valid region (id: 0x%x, base: 0x%x, limit: 0x%x, size: 0x%x)!\n", regId, regBase, regLimit, regSize);
            continue;
        }
        
        //ReadSpiParams->StatusInfo.ActReg = regId;
    
        //UINT32 regBase = REGION_BASE_ADDRESS(spiRegisterData->FREG[regId]);
        //DpVar(UINT32, regLimit, REGION_LIMIT(spiRegisterData->FREG[regId]));
        //UINT32 regSize = REGION_SIZE(spiRegisterData->FREG[regId]);

        printf("Dumping region 0x%x (%s) [%x:%x]\n", regId, FD_REGION_NAME(regId), regBase, regLimit);
        DPRINT_INT_H(regBase, "  ");
        DPRINT_INT_H(regLimit, "  ");
        DPRINT_INT_H(regSize, "  ");
    
        //if ( regSize > flashRegionBufferSize )
        //{
        //    status = reAllocVirtMem(&flashRegionBuffer, &flashRegionBufferSize, regSize);
        //    if ( status != 0 )
        //    {
        //        goto clean;
        //    }
        //}

        RtlZeroMemory(filename, 0x100);
        RtlStringCchPrintfW(filename, 0x100, L"%s\\spi-%s.dmp", Log->Dir, regLogName[regId]);
        status = kOpenFile(filename, (ULONG)wcslen(filename), &dumpFile, OPEN_FOR_WRITE_ONLY, FILE_OVERWRITE_IF, 0);
        if ( status != 0 )
        {
            EPrint("Opening file \"%ws\" failed! (0x%x)", filename, status);
            continue;
        }

        // special case for bios and /mmio flag set
        if ( regId == FREG_BIOS_ID 
          && ReadSpiParams->Flags.DumpBiosMode == DUMP_BIOS_MODE_MMIO )
        {
            PHYSICAL_ADDRESS pa = { 0 };

            // bios is found at end of 4gb region - its size
            biosBase = 0xFFFF'FFFF - regSize + 1;
            pa.QuadPart = biosBase;

            DPrint("DUMP_BIOS_MODE_MMIO\n");
            DPrint("  biosBase: %p\n", (PVOID)biosBase);
            DPrint("  biosSize: 0x%x\n", regSize);
    
            //ReadSpiParams->StatusInfo.ActRegPc = 0;
            
            printf("Reading Bios from RAM [%x:%x]\n", biosBase, 0xFFFF'FFFF);
            
            status = readPAWtf(Device, pa.QuadPart, regSize, CACHE_TYPE, dumpFile, READ_PA_WTF_MODE_CHUNKED_4);
            if ( status != 0 )
            {
                EPrint("Reading Bios pa mmio method failed! (0x%x)\n", status);
                goto clean;
            }
        }
        else
        {
            // normal spi dumping
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
            //status = readSpiFlashMemoryMm(Device, SpiRegister, SpiBarBase, regBase, regSize, flashRegionBuffer, regSize, &flashRegionBytesWritten, lockedSpiBar);
            status = readSpiFlashMemoryMmWtf(Device, SpiRegister, SpiBarBase, regBase, regSize, dumpFile, &flashRegionBytesWritten, lockedSpiBar);
#else
            //status = readSpiFlashMemoryPA(Device, SpiRegister, SpiBarBase, regBase, regSize, flashRegionBuffer, regSize, &flashRegionBytesWritten);
            status = readSpiFlashMemoryPaWtf(Device, SpiRegister, SpiBarBase, regBase, regSize, dumpFile, &flashRegionBytesWritten);
#endif
            if ( status != 0 )
            {
                EPrint("Reading Region[0x%x] failed! (0x%x)\n", regId, status);
                //goto clean;
            }
        }

        ZwClose(dumpFile);
        dumpFile = NULL;
        printf("Done!\n"
               "\n");
        
        // 
        // write plain spi bytes to file
    
        //if ( flashRegionBytesWritten )
        //{
        //    RtlZeroMemory(filename, 0x100);
        //    RtlStringCchPrintfW(filename, 0x100, L"%s\\spi-%s.dmp", Log->Dir, regLogName[regId]);
        //    status = kOpenFile(filename, (ULONG)wcslen(filename), &dumpFile, OPEN_FOR_WRITE_ONLY, FILE_OVERWRITE_IF, 0);
        //    if ( status != 0 )
        //    {
        //        EPrint("Opening file \"%ws\" failed! (0x%x)", filename, status);
        //        continue;
        //    }
        //    kWriteFile(dumpFile, flashRegionBuffer, flashRegionBytesWritten, &bytesWritten);
        //    ZwClose(dumpFile);
        //}
    }
    
    
clean:
    FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
    if ( lockedSpiBar )
    {
        unmapMemory(Device, lockedSpiBar, mapOutParams, MEM_MAP_OUT_SIZE);
    }
#endif
    if ( dumpFile )
        ZwClose(dumpFile);
    if ( fdrBuffer )
        free(fdrBuffer);
    if ( flashRegionBuffer )
    {
        NtFreeVirtualMemory(
                (HANDLE)-1,
                &flashRegionBuffer,
                &flashRegionBufferSize,
                MEM_RELEASE
            );
    }
    
    FLeave();
    return status;
}
DEFAULT_WARNING ( 6387 )

//
// check HSFS.H_SCIP == 0
// 
// H_SCIP
// Hardware sets this bit when software sets the Flash Cycle Go (FGO) bit in the Hardware Sequencing Flash Control register. 
// This bit remains set until the cycle completes on the SPI interface. 
// Hardware automatically sets and clears this bit so that software can determine when read data is valid and/or when it is safe to begin programming the next command. 
// Software must only program the next command when this bit is 0.
//
FORCEINLINE
NTSTATUS checkCycleInProgressMm(PUINT8 va)
{
    FEnter();

    NTSTATUS status = 0;

    volatile PBIOS_HSFSTS_CTL hsfsts_ctl = (PBIOS_HSFSTS_CTL)va;

    LARGE_INTEGER freq, t0, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t0);
    //LONGLONG timeoutTicks = freq.QuadPart / 10; // 100 ms
    LONGLONG timeoutTicks = freq.QuadPart * 5;  // 5 s - covers erase

    while ( hsfsts_ctl->H_SCIP == 1 )
    {
        _mm_pause();
        QueryPerformanceCounter(&now);
        if ( now.QuadPart - t0.QuadPart > timeoutTicks )
        {
            status = STATUS_UNSUCCESSFUL;
            goto clean;
        }
    }

clean:

    FLeave();
    return status;
}

#ifndef RW_PRIM_MEM_MAPPING_SUPPORTED
// check HSFS.H_SCIP == 0, HSFS.FDONE == 1
FORCEINLINE
NTSTATUS checkCycleInProgressPA(
    _In_ HANDLE Device, 
    _In_ PUINT8 pa, 
    _Out_opt_ PUINT32 value
)
{
    FEnter();
    NTSTATUS status = 0;
    BIOS_HSFSTS_CTL hsfsts_ctl = { 0 };

    LARGE_INTEGER freq, t0, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t0);
    LONGLONG timeoutTicks = freq.QuadPart * 5;  // 5 s - covers erase

    do {
        status = readPA(Device, (UINT64)pa, CACHE_TYPE, &hsfsts_ctl, 4, READ_PA_MODE_4);
        if ( status )
        {
            EPrint("Reading hsfsts (%p) failed! (0x%x)\n", pa, status);
        }

        QueryPerformanceCounter(&now);
        if ( now.QuadPart - t0.QuadPart > timeoutTicks ) {
            status = STATUS_UNSUCCESSFUL;
            EPrint("Waited too long for H_SCIP to get 0!\n");
            goto clean;
        }
    } while ( hsfsts_ctl.H_SCIP == 1 );

    DPrint("hsfsts_ctl\n");
    DPrint("  H_SCIP: 0x%x\n", hsfsts_ctl.H_SCIP);
    DPrint("  H_AEL: 0x%x\n", hsfsts_ctl.H_AEL);
    DPrint("  FCERR: 0x%x\n", hsfsts_ctl.FCERR);

    if ( value )
        *value = *(PUINT32)&hsfsts_ctl;

clean:
    FLeave();
    return status;
}

//
// check for error
FORCEINLINE
NTSTATUS
checkErrorPA(
    _In_ HANDLE Device, 
    _In_ PUINT8 pa, 
    _Inout_ PUINT32 value
)
{
    FEnter();
    NTSTATUS status = 0;
    BIOS_HSFSTS_CTL hsfsts_ctl = { 0 };
    
    if ( !value )
    {
        status = readPA(Device, (UINT64)pa, CACHE_TYPE, (PUINT32)&hsfsts_ctl, 4, READ_PA_MODE_4);
        if ( status )
        {
            EPrint("Reading hsfsts (%p) failed! (0x%x)\n", pa, status);
            goto clean;
        }
    }
    else 
    {
        *(PUINT32)&hsfsts_ctl = *value;
    }
    
    DPrint("hsfsts_ctl\n");
    DPrint("  H_SCIP: 0x%x\n", hsfsts_ctl.H_SCIP);
    DPrint("  H_AEL: 0x%x\n", hsfsts_ctl.H_AEL);
    DPrint("  FCERR: 0x%x\n", hsfsts_ctl.FCERR);


    if ( hsfsts_ctl.FCERR 
        || hsfsts_ctl.H_AEL )
    {
        // Just DPrint because it's not a break condition for now.
        // Can a block not be readable but another one could be in the same region??
        DPrint("Error set => FCERR: 0x%x, H_AEL: 0x%x\n",
            hsfsts_ctl.FCERR, hsfsts_ctl.H_AEL);

        status = (hsfsts_ctl.H_AEL<<1) | hsfsts_ctl.FCERR;
        goto clean;
    }

clean:
    FLeave();
    return status;
}
#endif

//
// reading spi flash via commands
// spi bar is already mapped making virtual rw possible
// 
// 1. Write the size-1 of data bytes we’d like to read to HSFSTS_CTL.FDBC
// 2. Set HSFSTS_CTL.FCYCLE to 0b00 to indicate a flash read operation
// 3. Write the flash offset we’re interested in to the FADDR register
// 4. Trigger a flash cycle by setting the HSFSTS_CTL.FGO bit
// 5. Wait until data is ready by polling HSFSTS_CTL.H_SCIP
// 6. Read the returned data from the relevant FDATAn register
// 7. Jump to 3 and repeat as necessary
//
NTSTATUS readSpiFlashMemoryMm(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ UINT32 LinearAddress, // Offset to start
    _In_ UINT32 Size, // Size to read
    _Out_ PVOID OutputBuffer, // paged result buffer
    _In_ UINT32 OutputBufferSize, // paged result buffer size
    _Out_ PULONG BytesWritten,
    _In_ PVOID MappedSection
)
{
    FEnter();
    (Device);

    NTSTATUS status = 0;

    PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP spiRegisterData = (PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP)SpiRegisterDataBuffer;
    BIOS_HSFSTS_CTL hsfsts_ctl = spiRegisterData->HSFSTS_CTL;
    BIOS_FADDR faddr = spiRegisterData->FADDR;

    UINT32 HSFSTS_CTL_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, HSFSTS_CTL); // 04
    UINT32 FADDR_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FADDR); // 08
    UINT32 FDATA_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FDATA[0]); // 10 - 4F
    
    volatile PUINT8 hsfsts_ctl_va = NULL;
    volatile PUINT8 faddr_va = NULL;

    UINT32 fdata[FDATA_COUNT];
    UINT32 fdataSize = FDATA_COUNT * sizeof(UINT32);

    PUINT8 outputBufferPtr = OutputBuffer; 
    //ULONG outputBufferRestSize = OutputBufferSize;

    //LARGE_INTEGER interval;


    *BytesWritten = 0;

    // TODO: remove 0x40 alignment restriction
    //       but there won't be a case, where it's necessary
    if ( Size % fdataSize != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Size not aligned! (0x%x)\n", status);
        goto clean;
    }

    DPrint("SpiBaseAddress: %p\n", SpiBaseAddress);
    DPrint("LinearAddress: 0x%x\n", LinearAddress);
    DPrint("OutputBuffer: %p\n", OutputBuffer);
    DPrint("OutputBufferSize: 0x%x\n", OutputBufferSize);
    DPrint("MappedSection: %p\n", MappedSection);

    //if ( spiRegisterData->HSFSTS_CTL.FDV != 1 )
    //{
    //    status = STATUS_INVALID_PARAMETER;
    //    EPrint("FDV not valid! (0x%x)\n", status);
    //    goto clean;
    //}
    
    if ( !OutputBuffer || OutputBufferSize < Size )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Invalid output buffer! (0x%x)\n", status);
        goto clean;
    }

    ULONG offset = LinearAddress;
    ULONG parts = Size / fdataSize;
    ULONG i;

    DPRINT_INT_H(LinearAddress, "  ");
    DPRINT_INT_H(Size, "  ");
    DPRINT_INT_H(parts, "  ");
    

    //
    // save virtual addresses

    UINT32 faddrPageOffset = ((UINT64)SpiBaseAddress + FADDR_Offset)%PAGE_SIZE;
    faddr_va = ((PUINT8)MappedSection + faddrPageOffset);

    UINT32 hsfstsPageOffset = ((UINT64)SpiBaseAddress + HSFSTS_CTL_Offset)%PAGE_SIZE;
    hsfsts_ctl_va = ((PUINT8)MappedSection + hsfstsPageOffset);

    UINT32 fdataPageOffset = ((UINT64)SpiBaseAddress + FDATA_Offset) % PAGE_SIZE;
    PUINT8 fdata_va = (PUINT8)MappedSection + fdataPageOffset;
    PUINT32 fdata_va_ptr = NULL;


    //
    // check HSFS.H_SCIP first, if someone else currently dumps the memory
    
    //status = checkCycleDone((PUINT8)hsfsts_ctl);
    status = checkCycleInProgressMm(hsfsts_ctl_va);
    if ( status != 0 )
        goto clean;


    //
    // calculated update period
    
    UINT32 updatePeriod = (parts > 100) ? parts / 100 : 1;
    UINT8 pc = 0;
    DPRINT_INT_H(updatePeriod, "  ");

    //
    // do the cycle
    
    try
    {
        for ( i = 0; i < parts; i++ )
        {
#ifdef DEBUG_PRINT
            pc = (UINT8)((i+1)*100/parts);
            DPrint("part 0x%x / 0x%x (%u%%): 0x%x\n", i+1, parts, pc, offset);
            DPrint("CurrentProcessorNumber: 0x%x\r", NtGetCurrentProcessorNumber());
#endif
#ifndef DEBUG_PRINT
            if ( i % updatePeriod == 0 )
            {
                pc = (UINT8)((i+1)*100/parts);
            }
            printf("part 0x%x / 0x%x (%u%%): 0x%x\r", i+1, parts, pc, offset);
#endif
            //DPrint("  HSFSTS_CTL: 0x%x\n",*(PUINT32)&hsfsts_ctl);
            //DPrint("  FCERR: 0x%x\n",hsfsts_ctl.FCERR);
            //DPrint("  H_AEL: 0x%x\n",hsfsts_ctl.H_AEL);
            //DPrint("  H_SCIP: 0x%x\n",hsfsts_ctl.H_SCIP);
            //DPrint("  FGO: 0x%x\n",hsfsts_ctl.FGO);
            //DPrint("  FCYCLE: 0x%x\n",hsfsts_ctl.FCYCLE);
            //DPrint("  WET: 0x%x\n",hsfsts_ctl.WET);
            //DPrint("  FADDR.FLA: 0x%x\n",faddr.FLA);

            //
            // write address

            faddr.FLA = offset;

            DPrint("faddr: 0x%x\n", *(PUINT32)&faddr);
            *(PUINT32)faddr_va = *(PUINT32)&faddr;

            //
            // write command
            
            hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
            hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
            hsfsts_ctl.FGO = 1;
            hsfsts_ctl.FCYCLE = FCYCLE_READ;
            hsfsts_ctl.FDBC = fdataSize - 1;
            
            DPrint("hsfsts_ctl: 0x%x\n", *(PUINT32)&hsfsts_ctl);
            *(PUINT32)hsfsts_ctl_va =  *(PUINT32)&hsfsts_ctl;


            //
            // Wait for SPI read cycle completion (HSFS.H_SCIP == 0, HSFS.FDONE == 1)

            status = checkCycleInProgressMm(hsfsts_ctl_va);
            if ( status != 0 )
                goto clean;


            //
            // check for error
        
            if ( ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->FCERR 
              || ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->H_AEL )
            {
                //status = STATUS_UNSUCCESSFUL;
                //EPrint("Reading not successful! (0x%x)\n", status);
                DPrint("Reading FLA: 0x%x not successful! FCERR: 0x%x, H_AEL: 0x%x\n",
                    faddr.FLA,
                    ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->FCERR, ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->H_AEL);

                RtlFillMemory(fdata, fdataSize, 0xFF);

                goto fill_buffer;
            }


            //
            // read all FDATA[n] at once

            RtlZeroMemory(fdata, fdataSize);
            PUINT32 fdata_loc_ptr = (PUINT32)&fdata;

            //pa.QuadPart = (UINT64)SpiBaseAddress + FDATA_Offset;
            //pageOffset = pa.QuadPart % PAGE_SIZE;
            //va = (PUINT8)MappedSection + pageOffset;
            fdata_va_ptr = (PUINT32)fdata_va;
            //DPrint("read from %p\n", (PVOID)pa.QuadPart);
        
            DPrint("Reading FDATA\n");
            for ( UINT32 bi = 0; bi < FDATA_COUNT; bi++ )
            {
                DPrint("  fdata_va_ptr: %p\n", fdata_va_ptr);
                *fdata_loc_ptr = *fdata_va_ptr;
                
                ++fdata_loc_ptr;
                ++fdata_va_ptr;
            }

        
            //
            // write to buffer
            // TODO: can be written directly to output buffer

        fill_buffer:
            RtlCopyMemory(outputBufferPtr, fdata, fdataSize);
            //outputBufferRestSize -= fdataSize;
            outputBufferPtr += fdataSize;
            *BytesWritten += fdataSize;

        //skip:
            offset += fdataSize;
        }
    } 
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        EPrint("Spi cycle failed! (0x%x)", status);
        goto clean;
    }
#ifdef DEBUG_PRINT
    printf("part 0x%x / 0x%x (%u%%): 0x%x\n", parts, parts, 100, offset);
#else
    printf("part 0x%x / 0x%x (%u%%): 0x%x\r", parts, parts, 100, offset);
#endif
    printf("\n");

clean:
    // reset command status register
    //if ( hsfsts_ctl_va )
    //{
    //    *(PUINT32)&hsfsts_ctl = 0;
    //    hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
    //    hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
    //     *(PUINT32)hsfsts_ctl_va =  *(PUINT32)&hsfsts_ctl;
    //}
    //restorePfn(&rwPageData);

    return status;
}

//
// reading spi flash via commands
// spi bar is already mapped making virtual rw possible
// result is written directly to file
// 
// 1. Write the size-1 of data bytes we’d like to read to HSFSTS_CTL.FDBC
// 2. Set HSFSTS_CTL.FCYCLE to 0b00 to indicate a flash read operation
// 3. Write the flash offset we’re interested in to the FADDR register
// 4. Trigger a flash cycle by setting the HSFSTS_CTL.FGO bit
// 5. Wait until data is ready by polling HSFSTS_CTL.H_SCIP
// 6. Read the returned data from the relevant FDATAn register
// 7. Jump to 3 and repeat as necessary
//
NTSTATUS readSpiFlashMemoryMmWtf(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ UINT32 LinearAddress, // Offset to start
    _In_ UINT32 Size, // Size to read
    _Out_ HANDLE OutFile, // file to write result
    _Out_ PULONG BytesWritten,
    _In_ PVOID MappedSection
)
{
    FEnter();
    (Device);

    NTSTATUS status = 0;

    PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP spiRegisterData = (PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP)SpiRegisterDataBuffer;
    BIOS_HSFSTS_CTL hsfsts_ctl = spiRegisterData->HSFSTS_CTL;
    BIOS_FADDR faddr = spiRegisterData->FADDR;

    UINT32 HSFSTS_CTL_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, HSFSTS_CTL); // 04
    UINT32 FADDR_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FADDR); // 08
    UINT32 FDATA_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FDATA[0]); // 10 - 4F
    
    //LARGE_INTEGER pa;
    //CM_PAGE_DATA rwPageData = { 0 };
    volatile PUINT8 hsfsts_ctl_va = NULL;
    volatile PUINT8 faddr_va = NULL;

    UINT32 fdata[FDATA_COUNT];
    UINT32 fdataSize = FDATA_COUNT * sizeof(UINT32);
    
    UINT32 bytesWritten = 0;

    //PUINT8 outputBufferPtr = OutputBuffer; 
    //ULONG outputBufferRestSize = OutputBufferSize;

    //LARGE_INTEGER interval;


    *BytesWritten = 0;

    // TODO: remove 0x40 alignment restriction
    //       but there won't be a case, where it's necessary
    if ( Size % fdataSize != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Size not aligned! (0x%x)\n", status);
        goto clean;
    }

    DPrint("SpiBaseAddress: %p\n", SpiBaseAddress);
    DPrint("LinearAddress: 0x%x\n", LinearAddress);
    DPrint("OutFile: %p\n", OutFile);
    DPrint("MappedSection: %p\n", MappedSection);

    //if ( spiRegisterData->HSFSTS_CTL.FDV != 1 )
    //{
    //    status = STATUS_INVALID_PARAMETER;
    //    EPrint("FDV not valid! (0x%x)\n", status);
    //    goto clean;
    //}
    
    //if ( !OutputBuffer || OutputBufferSize < Size )
    //{
    //    status = STATUS_INVALID_PARAMETER;
    //    EPrint("Invalid output buffer! (0x%x)\n", status);
    //    goto clean;
    //}

    ULONG offset = LinearAddress;
    //ULONG readSize = fdataSize;
    ULONG parts = Size / fdataSize;
    //ULONG rest = Size % fdataSize;
    ULONG i;

    DPRINT_INT_H(LinearAddress, "  ");
    DPRINT_INT_H(Size, "  ");
    DPRINT_INT_H(parts, "  ");
    //DPRINT_INT_H(rest, "  ");
    
    
    //
    // save virtual addresses

    UINT32 faddrPageOffset = ((UINT64)SpiBaseAddress + FADDR_Offset)%PAGE_SIZE;
    faddr_va = ((PUINT8)MappedSection + faddrPageOffset);

    UINT32 hsfstsPageOffset = ((UINT64)SpiBaseAddress + HSFSTS_CTL_Offset)%PAGE_SIZE;
    hsfsts_ctl_va = ((PUINT8)MappedSection + hsfstsPageOffset);

    UINT32 fdataPageOffset = ((UINT64)SpiBaseAddress + FDATA_Offset) % PAGE_SIZE;
    PUINT8 fdata_va = (PUINT8)MappedSection + fdataPageOffset;
    PUINT32 fdata_va_ptr = NULL;


    //
    // check HSFS.H_SCIP first, if someone else currently dumps the memory
    
    //status = checkCycleDone((PUINT8)hsfsts_ctl);
    status = checkCycleInProgressMm(hsfsts_ctl_va);
    if ( status != 0 )
        goto clean;


    //
    // calculated update period
    
    UINT32 updatePeriod = (parts > 100) ? parts / 100 : 1;
    UINT8 pc = 0;
    DPRINT_INT_H(updatePeriod, "  ");

    //
    // do the cycle
    
    try
    {
        for ( i = 0; i < parts; i++ )
        {
#ifdef DEBUG_PRINT
            pc = (UINT8)((i+1)*100/parts);
            DPrint("part 0x%x / 0x%x (%u%%): 0x%x\n", i+1, parts, pc, offset);
            DPrint("CurrentProcessorNumber: 0x%x\r", NtGetCurrentProcessorNumber());
#endif

#ifndef DEBUG_PRINT
            if ( i % updatePeriod == 0 )
            {
                pc = (UINT8)((i+1)*100/parts);
            }
            printf("part 0x%x / 0x%x (%u%%): 0x%x\r", i+1, parts, pc, offset);
#endif

            //DPrint("  HSFSTS_CTL: 0x%x\n",*(PUINT32)&hsfsts_ctl);
            //DPrint("  FCERR: 0x%x\n",hsfsts_ctl.FCERR);
            //DPrint("  H_AEL: 0x%x\n",hsfsts_ctl.H_AEL);
            //DPrint("  H_SCIP: 0x%x\n",hsfsts_ctl.H_SCIP);
            //DPrint("  FGO: 0x%x\n",hsfsts_ctl.FGO);
            //DPrint("  FCYCLE: 0x%x\n",hsfsts_ctl.FCYCLE);
            //DPrint("  WET: 0x%x\n",hsfsts_ctl.WET);
            //DPrint("  FADDR.FLA: 0x%x\n",faddr.FLA);

            //
            // write address

            faddr.FLA = offset;

            DPrint("faddr: 0x%x\n", *(PUINT32)&faddr);
            *(PUINT32)faddr_va = *(PUINT32)&faddr;

            //
            // write command
            
            hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
            hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
            hsfsts_ctl.FGO = 1;
            hsfsts_ctl.FCYCLE = FCYCLE_READ;
            hsfsts_ctl.FDBC = fdataSize - 1;
            
            DPrint("hsfsts_ctl: 0x%x\n", *(PUINT32)&hsfsts_ctl);
            *(PUINT32)hsfsts_ctl_va =  *(PUINT32)&hsfsts_ctl;


            //
            // Wait for SPI read cycle completion (HSFS.H_SCIP == 0, HSFS.FDONE == 1)

            status = checkCycleInProgressMm(hsfsts_ctl_va);
            if ( status != 0 )
                goto clean;

            
            ////HSFSTS_CLEAR = (PCH_RCBA_SPI_HSFSTS_AEL | PCH_RCBA_SPI_HSFSTS_FCERR | PCH_RCBA_SPI_HSFSTS_FDONE)
            //hsfsts_ctl.FDONE = 1;
            //hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
            //hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
            //*(PUINT32)hsfsts_ctl_va =  *(PUINT32)&hsfsts_ctl;

            //
            // check for error
        
            if ( ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->FCERR 
              || ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->H_AEL )
            {
                //status = STATUS_UNSUCCESSFUL;
                //EPrint("Reading not successful! (0x%x)\n", status);
                DPrint("Reading FLA: 0x%x not successful! FCERR: 0x%x, H_AEL: 0x%x\n",
                    faddr.FLA,
                    ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->FCERR, ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->H_AEL);

                RtlFillMemory(fdata, fdataSize, 0xFF);

                goto write_file;
            }


            //
            // read all FDATA[n] at once

            RtlZeroMemory(fdata, fdataSize);
            PUINT32 fdata_loc_ptr = (PUINT32)&fdata;

            //pa.QuadPart = (UINT64)SpiBaseAddress + FDATA_Offset;
            //pageOffset = pa.QuadPart % PAGE_SIZE;
            //va = (PUINT8)MappedSection + pageOffset;
            fdata_va_ptr = (PUINT32)fdata_va;
            //DPrint("read from %p\n", (PVOID)pa.QuadPart);
        
            DPrint("Reading FDATA\n");
            for ( UINT32 bi = 0; bi < FDATA_COUNT; bi++ )
            {
                DPrint("  fdata_va_ptr: %p\n", fdata_va_ptr);
                *fdata_loc_ptr = *fdata_va_ptr;
                
                ++fdata_loc_ptr;
                ++fdata_va_ptr;
            }

        
             //
            // write to file

        write_file:
            ;
            status = kWriteFile(OutFile, fdata, fdataSize, (PULONG)&bytesWritten);
            DPrint("kWriteFile status: 0x%x\n", status);
            DPrint("0x%x bytes written to dump file!\n", bytesWritten);
            if ( status != 0 )
            {
                EPrint("Writing failed! (0x%x)", status);
                goto clean;
            }

            *BytesWritten += fdataSize;

        //skip:
            offset += fdataSize;
        }
    } 
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        EPrint("Spi cycle failed! (0x%x)", status);
        goto clean;
    }
#ifdef DEBUG_PRINT
    printf("part 0x%x / 0x%x (%u%%): 0x%x\n", parts, parts, 100, offset);
#else
    printf("part 0x%x / 0x%x (%u%%): 0x%x\r", parts, parts, 100, offset);
#endif
    printf("\n");

clean:
    // reset command status register
    //if ( hsfsts_ctl_va )
    //{
    //    *(PUINT32)&hsfsts_ctl = 0;
    //    hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
    //    hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
    //     *(PUINT32)hsfsts_ctl_va =  *(PUINT32)&hsfsts_ctl;
    //}
    //restorePfn(&rwPageData);

    return status;
}

#ifndef RW_PRIM_MEM_MAPPING_SUPPORTED
//
// Read spi with multiple physical reads
// 
// Slower than the mapping variant, 'cause each read and write a remapping is done
// If an rw primitive driver is used, this may be the only option to go
//   since mapping drivers are less likely to get, but some are available.
//
NTSTATUS readSpiFlashMemoryPA(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ UINT32 LinearAddress, // Offset to start
    _In_ UINT32 Size, // Size to read
    _Out_ PVOID OutputBuffer, // paged result buffer
    _In_ UINT32 OutputBufferSize, // paged result buffer size
    _Out_ PULONG BytesWritten
)
{
    FEnter();
    
    NTSTATUS status = 0;

    PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP spiRegisterData = (PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP)SpiRegisterDataBuffer;
    BIOS_HSFSTS_CTL hsfsts_ctl = spiRegisterData->HSFSTS_CTL;
    BIOS_FADDR faddr = spiRegisterData->FADDR;

    UINT32 HSFSTS_CTL_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, HSFSTS_CTL); // 04
    UINT32 FADDR_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FADDR); // 08
    UINT32 FDATA_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FDATA[0]); // 10 - 4F
    
    PUINT8 hsfsts_ctl_pa = NULL;
    PUINT8 faddr_pa = NULL;

    UINT32 fdata[FDATA_COUNT];
    UINT32 fdataSize = FDATA_COUNT * sizeof(UINT32);

    PUINT8 outputBufferPtr = OutputBuffer; 
    //ULONG outputBufferRestSize = OutputBufferSize;

    //LARGE_INTEGER interval;

    UINT32 readPageFlags = CACHE_TYPE;
    UINT32 writePageFlags = 0;


    *BytesWritten = 0;

    // TODO: remove 0x40 alignment restriction
    //       but there won't be a case, where it's necessary
    if ( Size % fdataSize != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Size not aligned! (0x%x)\n", status);
        goto clean;
    }

    DPrint("SpiBaseAddress: %p\n", SpiBaseAddress);
    DPrint("LinearAddress: 0x%x\n", LinearAddress);
    DPrint("OutputBuffer: %p\n", OutputBuffer);
    DPrint("OutputBufferSize: 0x%x\n", OutputBufferSize);

    //if ( spiRegisterData->HSFSTS_CTL.FDV != 1 )
    //{
    //    status = STATUS_INVALID_PARAMETER;
    //    EPrint("FDV not valid! (0x%x)\n", status);
    //    goto clean;
    //}
    
    if ( !OutputBuffer || OutputBufferSize < Size )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Invalid output buffer! (0x%x)\n", status);
        goto clean;
    }

    ULONG offset = LinearAddress;
    //ULONG readSize = fdataSize;
    ULONG parts = Size / fdataSize;
    //ULONG rest = Size % fdataSize;
    ULONG i;

    DPRINT_INT_H(LinearAddress, "  ");
    DPRINT_INT_H(Size, "  ");
    DPRINT_INT_H(parts, "  ");
    //DPRINT_INT_H(rest, "  ");


    faddr_pa = ((PUINT8)SpiBaseAddress + FADDR_Offset);

    hsfsts_ctl_pa = ((PUINT8)SpiBaseAddress + HSFSTS_CTL_Offset);

    PUINT8 fdata_pa = (PUINT8)SpiBaseAddress + FDATA_Offset;
    PUINT32 fdata_pa_ptr = NULL;


    //
    // check HSFS.H_SCIP first, if someone else currently dumps the memory
    
    status = checkCycleInProgressPA(Device, hsfsts_ctl_pa, (PUINT32)&hsfsts_ctl);
    if ( status != 0 )
        goto clean;


    //
    // do the cycle
    
    UINT32 updatePeriod = (parts > 100) ? parts / 100 : 1;
    UINT8 pc = 0;
    DPRINT_INT_H(updatePeriod, "  ");

    //
    // do the cycle
    
    try
    {
        for ( i = 0; i < parts; i++ )
        {
#ifdef DEBUG_PRINT
            pc = (UINT8)((i+1)*100/parts);
            DPrint("part 0x%x / 0x%x (%u%%): 0x%x\n", i+1, parts, pc, offset);
            DPrint("CurrentProcessorNumber: 0x%x\r", NtGetCurrentProcessorNumber());
#endif

#ifndef DEBUG_PRINT
            if ( i % updatePeriod == 0 )
            {
                pc = (UINT8)((i+1)*100/parts);
            }
            printf("part 0x%x / 0x%x (%u%%): 0x%x\r", i+1, parts, pc, offset);
#endif

            //DPrint("  HSFSTS_CTL: 0x%x\n",*(PUINT32)&hsfsts_ctl);
            //DPrint("  FCERR: 0x%x\n",hsfsts_ctl.FCERR);
            //DPrint("  H_AEL: 0x%x\n",hsfsts_ctl.H_AEL);
            //DPrint("  H_SCIP: 0x%x\n",hsfsts_ctl.H_SCIP);
            //DPrint("  FGO: 0x%x\n",hsfsts_ctl.FGO);
            //DPrint("  FCYCLE: 0x%x\n",hsfsts_ctl.FCYCLE);
            //DPrint("  WET: 0x%x\n",hsfsts_ctl.WET);
            //DPrint("  FADDR.FLA: 0x%x\n",faddr.FLA);

            //
            // write address

            faddr.FLA = offset;

            //DPrint("writing faddr\n");
            status = writePA(Device, (UINT64)faddr_pa, writePageFlags, (PUINT32)&faddr, 4, WRITE_PA_MODE_4);
            if ( status )
            {
                EPrint("Writing faddr (%p) failed! (0x%x)\n", faddr_pa, status);
                break;
            }


            //
            // write command
            
            hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
            hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
            hsfsts_ctl.FGO = 1;
            hsfsts_ctl.FCYCLE = FCYCLE_READ;
            hsfsts_ctl.FDBC = fdataSize - 1;
        
            //DPrint("writing hsfsts\n");
            status = writePA(Device, (UINT64)hsfsts_ctl_pa, writePageFlags, (PUINT32)&hsfsts_ctl, 4, WRITE_PA_MODE_4);
            if ( status )
            {
                EPrint("Writing hsfsts (%p) failed! (0x%x)\n", hsfsts_ctl_pa, status);
                break;
            }
        
        
            //
            // Wait for SPI read cycle completion (HSFS.H_SCIP == 0, HSFS.FDONE == 1)

            status = checkCycleInProgressPA(Device, hsfsts_ctl_pa, (PUINT32)&hsfsts_ctl);
            if ( status != 0 )
            {
                goto clean;
            }
            
            status = checkErrorPA(Device, hsfsts_ctl_pa, (PUINT32)&hsfsts_ctl);
            if ( status != 0 )
            {
                if ( status < 0 )
                {
                    goto clean;
                }
                else
                {
                    RtlFillMemory(fdata, fdataSize, 0xFF);
                    status = 0;
                    goto fill_buffer;
                }
            }


            //
            // read all FDATA[n] at once

            RtlZeroMemory(fdata, fdataSize);
            PUINT32 fdata_loc_ptr = (PUINT32)&fdata;

            //pa.QuadPart = (UINT64)SpiBaseAddress + FDATA_Offset;
            //pageOffset = pa.QuadPart % PAGE_SIZE;
            //va = (PUINT8)MappedSection + pageOffset;
            fdata_pa_ptr = (PUINT32)fdata_pa;
            //DPrint("read from %p\n", (PVOID)pa.QuadPart);
        
            DPrint("Reading FDATA\n");
            for ( UINT32 bi = 0; bi < FDATA_COUNT; bi++ )
            {
                DPrint("  fdata_pa_ptr: %p\n", fdata_pa_ptr);
                status = readPA(Device, (UINT64)fdata_pa_ptr, readPageFlags, fdata_loc_ptr, 4, READ_PA_MODE_4);
                if ( status )
                {
                    EPrint("Reading fdata (%p) failed! (0x%x)\n", fdata_pa_ptr, status);
                    break;
                }

                ++fdata_loc_ptr;
                //_disable();
                ++fdata_pa_ptr;
                //_enable();
            }

        
            //
            // write to paged output buffer until full

        fill_buffer:
            RtlCopyMemory(outputBufferPtr, fdata, fdataSize);
            //outputBufferRestSize -= fdataSize;
            outputBufferPtr += fdataSize;
            *BytesWritten += fdataSize;

        //skip:
            offset += fdataSize;
        }
    } 
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        EPrint("Unable to read 0x%x bytes from %p! (0x%x)", 4, fdata_pa_ptr, status);
        goto clean;
    }
#ifdef DEBUG_PRINT
    printf("part 0x%x / 0x%x (%u%%): 0x%x\n", parts, parts, 100, offset);
#else
    printf("part 0x%x / 0x%x (%u%%): 0x%x\r", parts, parts, 100, offset);
#endif
    printf("\n");

clean:
    //if ( hsfsts_ctl_pa )
    //{
    //    hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
    //    hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
    //    //hsfsts_ctl.FGO = 1;
    //    //hsfsts_ctl.FCYCLE = FCYCLE_READ;
    //    //hsfsts_ctl.FDBC = fdataSize - 1;
    //    
    //    NTSTATUS status2 = writePA(Device, (UINT64)hsfsts_ctl_pa, writePageFlags, (PUINT32)&hsfsts_ctl, 4, WRITE_PA_MODE_4);
    //    if ( status2 )
    //    {
    //        EPrint("Resetting hsfsts (%p) failed! (0x%x)\n", hsfsts_ctl_pa, status);
    //    }
    //}

    return status;
}

//
// Read spi with multiple physical reads
// Write result directly to file
// 
// Slower than the mapping variant, 'cause each read and write a remapping is done
// If an rw primitive driver is used, this may be the only option to go
//   since mapping drivers are less likely to get, but some are available.
//
NTSTATUS readSpiFlashMemoryPaWtf(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ UINT32 LinearAddress, // Offset to start
    _In_ UINT32 Size, // Size to read
    _In_ HANDLE OutFile, // file to write result
    _Out_ PULONG BytesWritten
)
{
    FEnter();

    NTSTATUS status = 0;
    PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP spiRegisterData = (PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP)SpiRegisterDataBuffer;
    BIOS_HSFSTS_CTL hsfsts_ctl = spiRegisterData->HSFSTS_CTL;
    BIOS_FADDR faddr = spiRegisterData->FADDR;

    UINT32 HSFSTS_CTL_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, HSFSTS_CTL); // 04
    UINT32 FADDR_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FADDR); // 08
    UINT32 FDATA_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FDATA[0]); // 10 - 4F
    
    PUINT8 hsfsts_ctl_pa = NULL;
    PUINT8 faddr_pa = NULL;

    UINT32 fdata[FDATA_COUNT];
    UINT32 fdataSize = FDATA_COUNT * sizeof(UINT32);

    //LARGE_INTEGER interval;

    UINT32 readPageFlags = CACHE_TYPE;
    UINT32 writePageFlags = 0;
    UINT32 bytesWritten = 0;
    


    *BytesWritten = 0;

    // TODO: remove 0x40 alignment restriction
    //       but there won't be a case, where it's necessary
    if ( Size % fdataSize != 0 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Size not aligned! (0x%x)\n", status);
        goto clean;
    }

    //if ( spiRegisterData->HSFSTS_CTL.FDV != 1 )
    //{
    //    status = STATUS_INVALID_PARAMETER;
    //    EPrint("FDV not valid! (0x%x)\n", status);
    //    goto clean;
    //}
    
    if ( !OutFile )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("No outfile given! (0x%x)\n", status);
        goto clean;
    }

    ULONG offset = LinearAddress;
    //ULONG readSize = fdataSize;
    ULONG parts = Size / fdataSize;
    //ULONG rest = Size % fdataSize;
    ULONG i;

    DPRINT_INT_H(LinearAddress, "  ");
    DPRINT_INT_H(Size, "  ");
    DPRINT_INT_H(parts, "  ");
    //DPRINT_INT_H(rest, "  ");


    faddr_pa = ((PUINT8)SpiBaseAddress + FADDR_Offset);

    hsfsts_ctl_pa = ((PUINT8)SpiBaseAddress + HSFSTS_CTL_Offset);

    PUINT8 fdata_pa = (PUINT8)SpiBaseAddress + FDATA_Offset;
    PUINT32 fdata_pa_ptr = NULL;


    //
    // check HSFS.H_SCIP first, if someone else currently dumps the memory
    
    status = checkCycleInProgressPA(Device, hsfsts_ctl_pa, (PUINT32)&hsfsts_ctl);
    if ( status != 0 )
        goto clean;


    //
    // do the cycle
    
    UINT32 updatePeriod = (parts > 100) ? parts / 100 : 1;
    UINT8 pc = 0;
    DPRINT_INT_H(updatePeriod, "  ");

    //
    // do the cycle
    
    try
    {
        for ( i = 0; i < parts; i++ )
        {
#ifdef DEBUG_PRINT
            pc = (UINT8)((i+1)*100/parts);
            DPrint("part 0x%x / 0x%x (%u%%): 0x%x\n", i+1, parts, pc, offset);
            DPrint("CurrentProcessorNumber: 0x%x\r", NtGetCurrentProcessorNumber());
#endif

#ifndef DEBUG_PRINT
            if ( i % updatePeriod == 0 )
            {
                pc = (UINT8)((i+1)*100/parts);
            }
            printf("part 0x%x / 0x%x (%u%%): 0x%x\r", i+1, parts, pc, offset);
#endif

            //DPrint("  HSFSTS_CTL: 0x%x\n",*(PUINT32)&hsfsts_ctl);
            //DPrint("  FCERR: 0x%x\n",hsfsts_ctl.FCERR);
            //DPrint("  H_AEL: 0x%x\n",hsfsts_ctl.H_AEL);
            //DPrint("  H_SCIP: 0x%x\n",hsfsts_ctl.H_SCIP);
            //DPrint("  FGO: 0x%x\n",hsfsts_ctl.FGO);
            //DPrint("  FCYCLE: 0x%x\n",hsfsts_ctl.FCYCLE);
            //DPrint("  WET: 0x%x\n",hsfsts_ctl.WET);
            //DPrint("  FADDR.FLA: 0x%x\n",faddr.FLA);

            //
            // write address

            faddr.FLA = offset;

            //DPrint("writing faddr\n");
            status = writePA(Device, (UINT64)faddr_pa, writePageFlags, (PUINT32)&faddr, 4, WRITE_PA_MODE_4);
            if ( status )
            {
                EPrint("Writing faddr (%p) failed! (0x%x)\n", faddr_pa, status);
                break;
            }


            //
            // write command
            
            hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
            hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
            hsfsts_ctl.FGO = 1;
            hsfsts_ctl.FCYCLE = FCYCLE_READ;
            hsfsts_ctl.FDBC = fdataSize - 1;
        
            //DPrint("writing hsfsts\n");
            status = writePA(Device, (UINT64)hsfsts_ctl_pa, writePageFlags, (PUINT32)&hsfsts_ctl, 4, WRITE_PA_MODE_4);
            if ( status )
            {
                EPrint("Writing hsfsts (%p) failed! (0x%x)\n", hsfsts_ctl_pa, status);
                break;
            }
        
        
            //
            // Wait for SPI read cycle completion (HSFS.H_SCIP == 0, HSFS.FDONE == 1)

            status = checkCycleInProgressPA(Device, hsfsts_ctl_pa, (PUINT32)&hsfsts_ctl);
            if ( status != 0 )
            {
                goto clean;
            }
            
            status = checkErrorPA(Device, hsfsts_ctl_pa, (PUINT32)&hsfsts_ctl);
            if ( status != 0 )
            {
                if ( status < 0 )
                {
                    goto clean;
                }
                else
                {
                    RtlFillMemory(fdata, fdataSize, 0xFF);
                    status = 0;
                    goto fill_buffer;
                }
            }


            //
            // read all FDATA[n] at once

            RtlZeroMemory(fdata, fdataSize);
            PUINT32 fdata_loc_ptr = (PUINT32)&fdata;

            //pa.QuadPart = (UINT64)SpiBaseAddress + FDATA_Offset;
            //pageOffset = pa.QuadPart % PAGE_SIZE;
            //va = (PUINT8)MappedSection + pageOffset;
            fdata_pa_ptr = (PUINT32)fdata_pa;
            //DPrint("read from %p\n", (PVOID)pa.QuadPart);
        
            DPrint("Reading FDATA\n");
            for ( UINT32 bi = 0; bi < FDATA_COUNT; bi++ )
            {
                DPrint("  fdata_pa_ptr: %p\n", fdata_pa_ptr);
                status = readPA(Device, (UINT64)fdata_pa_ptr, readPageFlags, fdata_loc_ptr, 4, READ_PA_MODE_4);
                if ( status )
                {
                    EPrint("Reading fdata (%p) failed! (0x%x)\n", fdata_pa_ptr, status);
                    break;
                }

                ++fdata_loc_ptr;
                //_disable();
                ++fdata_pa_ptr;
                //_enable();
            }

        
            //
            // write to file

        fill_buffer:
            ;
            status = kWriteFile(OutFile, fdata, fdataSize, (PULONG)&bytesWritten);
            //NTSTATUS kReadFileAtOffset(HANDLE handle, PVOID buffer, ULONG nbytes, PULONG pBytesRead, PLARGE_INTEGER pOffset)
            DPrint("kWriteFile status: 0x%x\n", status);
            DPrint("0x%x bytes written to dump file!\n", bytesWritten)
            if ( status != 0 )
            {
                EPrint("Writing failed! (0x%x)", status);
                goto clean;
            }

            *BytesWritten += fdataSize;

        //skip:
            offset += fdataSize;
        }
    } 
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        EPrint("Unable to read 0x%x bytes from %p! (0x%x)", 4, fdata_pa_ptr, status);
        goto clean;
    }
#ifdef DEBUG_PRINT
    printf("part 0x%x / 0x%x (%u%%): 0x%x\n", parts, parts, 100, offset);
#else
    printf("part 0x%x / 0x%x (%u%%): 0x%x\r", parts, parts, 100, offset);
#endif
    printf("\n");

clean:
    //if ( hsfsts_ctl_pa )
    //{
    //    hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
    //    hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
    //    //hsfsts_ctl.FGO = 1;
    //    //hsfsts_ctl.FCYCLE = FCYCLE_READ;
    //    //hsfsts_ctl.FDBC = fdataSize - 1;
    //    
    //    NTSTATUS status2 = writePA(Device, (UINT64)hsfsts_ctl_pa, writePageFlags, (PUINT32)&hsfsts_ctl, 4, WRITE_PA_MODE_4);
    //    if ( status2 )
    //    {
    //        EPrint("Resetting hsfsts (%p) failed! (0x%x)\n", hsfsts_ctl_pa, status);
    //    }
    //}

    return status;
}
#endif



int commandSpi(
    _In_ HANDLE Device,
    _In_ PCMD_PARAMS Params 
)
{
    FEnter();

    NTSTATUS status = 0;

    SIZE_T logBufferRestSize = 0;
    PCHAR logBufferPtr = NULL;

    PWCHAR filePath = NULL;
    UINT32 filePathCch = 0;
    
    LOG log = { 0 };
    log.Dir = Params->LogDir;
    log.BufferSize = PAGE_SIZE;

    //PVOID cfgBuffer = NULL;
    //UINT32 cfgBufferSize = PCI_CONFIG_SIZE;
    
    //PVOID barBuffer = NULL;
    //UINT32 barBufferSize = PAGE_SIZE;
    
    UINT64 pciExBarAddress = 0;
    UINT64 pciExBarSize = 0;

    BDF pchBdf = { 0 };
    
    COMMAND_SPI_PARAMS commandSpiParams = { 0 };
    
    SYSTEMTIME lt = { 0 };

    printf("Commanding SPI\n");
    
    GetLocalTime(&lt);
    printf("[Started %02u:%02u:%02u]\n\n", lt.wHour, lt.wMinute, lt.wSecond);

    //
    // init log buffer

    if ( Params->Flags.WriteToFile )
    {
        log.Buffer = malloc(log.BufferSize);
        if ( !log.Buffer )
            goto clean;

        logBufferRestSize = log.BufferSize;
        logBufferPtr = log.Buffer;
        
        PWCHAR fileBaseName = L"cspi.log";
        filePathCch = (Params->LogDirCb/2) + (UINT32)wcslen(fileBaseName) + 1; // + separator (1) 
        filePath = malloc(filePathCch*2 + 2); // + \0\0 (2)
        if ( !filePath )
            goto clean;

        RtlStringCchPrintfW(filePath, filePathCch+1, L"%s\\%s", log.Dir, fileBaseName);

        status = kOpenFile(filePath, filePathCch, &log.File, OPEN_FOR_WRITE_ONLY, FILE_OVERWRITE_IF, 0);
        if ( status != 0 )
        {
            EPrint("Opening file \"%ws\" failed! (0x%x)\n", filePath, status);
            goto clean;
        }
    }
    

    //
    // init params

    commandSpiParams.BDF = Params->PciIo.BDF;
    commandSpiParams.BarId = Params->PciIo.BarId;
    commandSpiParams.Rcbo = Params->PciIo.Rcbo;
    commandSpiParams.Flags.ReadMode = Params->PciIo.Flags.ReadMode;
    commandSpiParams.CycleCommand = Params->CommandSpi.CycleCommand;
    commandSpiParams.DataSize = Params->CommandSpi.DataSize;
    commandSpiParams.LinearAddress = Params->CommandSpi.LinearAddress;
    commandSpiParams.Data = Params->CommandSpi.Data;
    
    
    //
    // get pciExBarAddress
    // to us mmio cfg read if possible
        
    if ( commandSpiParams.Flags.ReadMode == 0 
      || commandSpiParams.Flags.ReadMode == READ_CFG_MODE_MM )
    {
        ////status = getPciExBarValuePortIo(Device, &pciExBarAddress, &pciExBarSize);
        //status = getPciExBarValueMCFG(Device, &pciExBarAddress, &pciExBarSize);
        //if ( status != 0 )
        //{
        //    EPrint("getPciExBarValue failed! (0x%x)\n", status);
        //    // exit if desired read mode is set to mm
        //    if ( commandSpiParams.Flags.ReadMode == READ_CFG_MODE_MM )
        //        goto clean;
        //    // else use port reading 
        //    commandSpiParams.Flags.ReadMode = READ_CFG_MODE_PORT;
        //    printf("Using Port IO Pci Configs.\n");
        //}
        //else
        //{
        //    commandSpiParams.Flags.ReadMode = READ_CFG_MODE_MM;
        //    DPrint("pciExBarAddress: 0x%llx\n", pciExBarAddress);
        //    DPrint("pciExBarSize: 0x%llx\n", pciExBarSize);
        //    printf("Using Memory Mapped Pci Configs.\n");
        //}

        
        status = getPciExBarValueMCFG(Device, &pciExBarAddress, &pciExBarSize);
        if ( status != 0 )
        {
            EPrint("getPciExBarValueMCFG failed! (0x%x)\n", status);
            status = getPciExBarValuePortIo(Device, &pciExBarAddress, &pciExBarSize);
            
            if ( status != 0 )
            {
                EPrint("getPciExBarValuePortIo failed! (0x%x)\n", status);
                if ( commandSpiParams.Flags.ReadMode == READ_CFG_MODE_MM )
                    goto clean;
                else
                    commandSpiParams.Flags.ReadMode = READ_CFG_MODE_PORT;
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
        
            //pciIoParams->PciExBarAddress = pciExBarAddress;
            //pciIoParams->PciExBarSize = pciExBarSize;
            commandSpiParams.Flags.ReadMode = READ_CFG_MODE_MM;
        }
    }
    
    //
    // read cfg values of 00:1f:00 (PCH/LPC)

    pchBdf.Bus = 0;
    pchBdf.Device = 0x1f;
    pchBdf.Function = 0;

    PCHAR lpcStr = NULL;

    UINT32 venDevId = (UINT32)-1;
    UINT16 vendorId = (UINT16)-1;
    UINT16 deviceId = (UINT16)-1;
    RCBA rcba = { 0 };
    
    //
    // read vendor device id value at once

    UINT32 offset = 0;
    if ( commandSpiParams.Flags.ReadMode == READ_CFG_MODE_PORT )
    {
        UINT32 index = PCI_CFG_PORT_IO_ADDR(pchBdf.Bus, pchBdf.Device, pchBdf.Function, offset);
        status = outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, index, &venDevId);
        if ( status != 0 )
        {
            EPrint("outIn32 0x%x:0x%x => 0x%x failed! (0x%x)\n", PCI_CONFIG_COMMAND_PORT, index, PCI_CONFIG_STATUS_PORT, status);
            goto clean;
        }
    }
    else
    {
        UINT64 index = PCI_CFG_MM_ADDR(pciExBarAddress, pchBdf.Bus, pchBdf.Device, pchBdf.Function, offset);
        status = readPA(Device, index, CACHE_TYPE, &venDevId, 4, READ_PA_MODE_4);
        if ( status != 0 )
        {
            EPrint("readPA4 0x%llx failed! (0x%x)\n", index, status);
            goto clean;
        }
    }
    DPrint("venDevId: 0x%x\n", venDevId);
    // split into vendor and device
    vendorId = venDevId & 0xFFFF;
    deviceId = (venDevId>>0x10) & 0xFFFF;
    DPrint("vendorId: 0x%x\n", vendorId);
    DPrint("deviceId: 0x%x\n", deviceId);

    //
    // read rcba value
    // could check chip series to know, 
    // if rcba is to be expected to prevent false positives

    offset = 0xF0;
    if ( commandSpiParams.Flags.ReadMode == READ_CFG_MODE_PORT )
    {
        UINT32 index = PCI_CFG_PORT_IO_ADDR(pchBdf.Bus, pchBdf.Device, pchBdf.Function, offset);
        status = outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, index, (PUINT32)&rcba);
        if ( status != 0 )
        {
            EPrint("outIn32 0x%x:0x%x => 0x%x failed! (0x%x)\n", PCI_CONFIG_COMMAND_PORT, index, PCI_CONFIG_STATUS_PORT, status);
            goto clean;
        }
    }
    else
    {
        UINT64 index = PCI_CFG_MM_ADDR(pciExBarAddress, pchBdf.Bus, pchBdf.Device, pchBdf.Function, offset);
        status = readPA(Device, index, CACHE_TYPE, (PUINT32)&rcba, 4, READ_PA_MODE_4);
        if ( status != 0 )
        {
            EPrint("readPA4 0x%llx failed! (0x%x)\n", index, status);
            goto clean;
        }
    }
    DPrint("rcba: 0x%x\n", *(PUINT32)&rcba);
    DPrint("  Enable: 0x%x\n", rcba.Enable);
    DPrint("  Reserved1: 0x%x\n", rcba.Reserved1);
    DPrint("  BA: 0x%x\n", rcba.BA);


    //
    // check vendor
    // currently just intel is supported

    if ( vendorId != PCI_VENDORID_INTEL )
    {
        status = STATUS_NOT_SUPPORTED;
        EPrint("Not supported vendor 0x%x!\n"
               "    Only Intel supported yet! (0x%x)\n", vendorId, status);

        goto clean;
    }
    
    
    if ( Params->Flags.SpiLocation == SPI_LOC_BDF )
    {
        // get spi interface from given bdf bar values
        status = commandSpiFromDevice(Device, pciExBarAddress, &commandSpiParams, &log);
    }
    else if ( Params->Flags.SpiLocation == SPI_LOC_RCBA )
    {
        // get spi interface from static rcba offset
        // TODO: make dynamic
        status = commandSpiFromRCBA(Device, &commandSpiParams, vendorId, deviceId, rcba, &log);
    }
    else 
    {
        commandSpiParams.Rcbo = RCBA_SPI_OFFSET; // use default offset

        //
        // Switch over known device ids to get the method that locates the spi register.
        // It's either rcba+offset or device 00:1f:05 bar[0]
        // If deviceId is not known
        //   try autodetection by
        //   checking rcba.Enable 
        //   and choose rcba or device method depending on the result.

        switch ( deviceId )
        {
            // knwon rcba cases
            case 0x1c4f: // 5 Series QM67 Express Chipset LPC Controller
            case 0x3b09: // HM55 Chipset LPC Interface Controller (6/7 series)
            case 0x9c43: // 8 Series LPC Controller
            case 0x9cc3: // 8 Series Wildcat Point-LP LPC Controller
                DPrint("Read SPI bar from RCBA + offset!\n")
                status = commandSpiFromRCBA(Device, &commandSpiParams, vendorId, deviceId, rcba, &log);
                break;

            // known bdf 00:1f:05::bar[0] cases
            case 0xa154: // CM238 Chipset LPC/eSPI Controller
            case 0x9d48: // Sunrise Point-LP LPC Controller
            case 0x9d4e: // Sunrise Point LPC/eSPI Controller
            case 0xa2c6: // pch 200 series
            case 0xa308: // 300 Series Chipset Family LPC Controller
                DPrint("Read SPI bar from 00:1f:05!\n")
                switch ( deviceId )
                {
                    case 0xa154: lpcStr = "100 Series: CM238 Chipset LPC"; break;
                    case 0x9d48: lpcStr = "200 Series: Sunrise Point-LP LPC Controller"; break;
                    case 0x9d4e: lpcStr = "200 Series: Sunrise Point LPC/eSPI Controller"; break;
                    case 0xa2c6: lpcStr = "200 Series"; break;
                    case 0xa308: lpcStr = "300 Series: Chipset Family LPC Controller"; break;
                    default: break; // should not happen
                }
                DPrint("  %s\n", lpcStr);

                commandSpiParams.BDF.Bus = 0;
                commandSpiParams.BDF.Device = 0x1f;
                commandSpiParams.BDF.Function = 5;
                status = commandSpiFromDevice(Device, pciExBarAddress, &commandSpiParams, &log);

                break;

            // auto select method
            default:
                //
                // not known device,
                // choose rcba if rcba.enable
                // else choose device 00:1f:05
                // 

                DPrint("Not known DeviceId: 0x%x. (0x%x)\n", deviceId, status);
                DPrint("Trying auto detection\n");
            
                if ( rcba.Enable )
                {
                    // just VendorId [0:1], DeviceId [2:3] and RCBA [f0:f7] are needed of cfg
                    status = commandSpiFromRCBA(Device, &commandSpiParams, vendorId, deviceId, rcba, &log);
                }
                else
                {
                    commandSpiParams.BDF.Bus = 0;
                    commandSpiParams.BDF.Device = 0x1f;
                    commandSpiParams.BDF.Function = 5;

                    status = commandSpiFromDevice(Device, pciExBarAddress, &commandSpiParams, &log);
                }
                break;
        }
    }
    
    GetLocalTime(&lt);
    printf("\n[Finished %02u:%02u:%02u]\n", lt.wHour, lt.wMinute, lt.wSecond);

clean:
    //if (cfgBuffer)
        //free(cfgBuffer);
    if ( log.Buffer )
        free(log.Buffer);
    if ( log.File )
        ZwClose(log.File);
    if ( filePath )
        free(filePath);
    //if ( barBuffer )
    //    free(barBuffer);
    
    FLeave();
    return status;
}

//  warning C6387: 'mmioMapOut.LockedAddress' could be '0':
DISABLE_WARNING(6387)
NTSTATUS
commandSpiFromRCBA(
    _In_ HANDLE Device,
    _In_ PCOMMAND_SPI_PARAMS CommandSpiParams,
    _In_ UINT16 VendorId,
    _In_ UINT16 DeviceId,
    _In_ RCBA RCBA,
    _In_ PLOG Log
)
{
    FEnter();

    NTSTATUS status = 0;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;

    PVOID rcba_ba = NULL;
    UINT32 spiOffset = CommandSpiParams->Rcbo;
    PVOID spiAddress = NULL;
    UINT64 spiSize = RCBA_SPI_SIZE;
    
    PVOID registerBuffer = NULL;
    ULONG registerBufferSize = PAGE_SIZE;
    
    ULONG seriesNr = 0;
    
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    PVOID lockedSpiBase = NULL;
    UINT8 mapOutParams[MEM_MAP_OUT_SIZE] = { 0 };
#endif

    if ( VendorId != PCI_VENDORID_INTEL )
    {
        status = STATUS_NOT_SUPPORTED;
        EPrint("Not supported vendor id 0x%x! (0x%x)\n", VendorId, status);
        goto clean;
    }

    switch ( DeviceId )
    {
        case 0x1c4f: // 5 Series QM67 Express Chipset LPC Controller
        case 0x3b09: // 7 Series LPC Controller
        case 0x9c43: // 8 Series LPC Controller
        case 0x9cc3: // 8 Series Wildcat Point-LP LPC Controller
        
            switch ( DeviceId ) {
                case 0x1c4f: 
                    seriesNr = 5; break;
                case 0x3b09: 
                    seriesNr = 6; break;
                case 0x9c43: 
                case 0x9cc3: 
                    seriesNr = 8; break;
                default:
                    goto clean;
            }
        
            DPrint("%u Series LPC Controller\n", seriesNr);
            
            if ( !RCBA.Enable )
            {
                status = STATUS_UNSUCCESSFUL;
                EPrint("RCBA not enabled! (0x%x)\n", status);
                goto clean;
            }

            rcba_ba = (PVOID)( (*(PUINT32)&RCBA) & (RCBA_BA_MASK) );
            // spiOffset = RCBA_SPI_OFFSET; // could be overwritten here
            
            break;
        
        default:
            // not known but trying to dump anyway from the hardcoded offset
            
            //status = STATUS_UNSUCCESSFUL;
            EPrint("Unknown device 0x%x! (0x%x)\n", DeviceId, status);
            
            // using it as default

            if ( !RCBA.Enable )
            {
                status = STATUS_UNSUCCESSFUL;
                EPrint("RCBA not enabled! (0x%x)\n", status);
                goto clean;
            }

            rcba_ba = (PVOID)( (*(PUINT32)&RCBA) & (RCBA_BA_MASK) );

            //goto clean;
            break;
    }

    spiAddress = (PVOID)((SIZE_T)rcba_ba + spiOffset);

    DPRINT_PTR(rcba_ba, "");
    DPRINT_INT_H(spiOffset, "");
    DPRINT_PTR(spiAddress, "");
    DPRINT_INT_H(spiSize, "");

    if ( Log->File )
    {
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "  rcba_ba: %p\r\n"
                            "  spiOffset: 0x%x\r\n"
                            "  spiAddress: %p\r\n"
                            "  spiSize: 0x%llx\r\n",
                            rcba_ba,
                            spiOffset,
                            spiAddress,
                            spiSize);
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }


    //
    // allocate register buffer

    registerBufferSize = (UINT32)spiSize;
    registerBuffer = malloc(registerBufferSize);
    if ( !registerBuffer )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }
    
    //
    // read spi register

    status = readSpiRegister(Device, spiAddress, spiSize, registerBuffer, registerBufferSize, Log);
    if ( status != 0 )
    {
        EPrint("readSpiRegister failed! (0x%x)\n", status);
        goto clean;
    }
    
    PVOID spiBase = spiAddress;
    
    
    //
    // map spi bar (mmio)
    
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    status = mapMemory(Device, spiBase, (UINT32)spiSize, &lockedSpiBase, &mapOutParams, MEM_MAP_OUT_SIZE);
    if ( status != 0 )
    {
        EPrint("mapMemory request failed! (0x%x)\n", status);
        goto clean;
    }
#endif


    //
    // issue spi command loop
    
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    status = issueSpiCommandLoop(Device, CommandSpiParams, registerBuffer, spiBase, lockedSpiBase);
#else
    status = issueSpiCommandLoop(Device, CommandSpiParams, registerBuffer, spiBase, NULL);
#endif
    if ( status != 0 )
    {
        EPrint("issueSpiCommandLoop() failed! (0x%x)\n", status);
        goto clean;
    }

clean:
    
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    if ( lockedSpiBase )
    {
        unmapMemory(Device, lockedSpiBase, mapOutParams, MEM_MAP_OUT_SIZE);
    }
#endif

    if ( Log->File )
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    if ( registerBuffer )
        free(registerBuffer);

    FLeave();
    return status;
}
DEFAULT_WARNING(6387)

//  warning C6387: 'mmioMapOut.LockedAddress' could be '0':
DISABLE_WARNING(6387)
NTSTATUS
commandSpiFromDevice(
    _In_ HANDLE Device,
    _In_ UINT64 PciExBarAddress,
    _In_ PCOMMAND_SPI_PARAMS CommandSpiParams,
    _In_ PLOG Log
)
{
    FEnter();

    NTSTATUS status = 0;
    ULONG i;

    UINT32 offset;

    SIZE_T logBufferRestSize = Log->BufferSize;
    PCHAR logBufferPtr = Log->Buffer;
    
    PVOID cfgBuffer = NULL; // nonpaged 
    ULONG cfgBufferSize = PCI_CONFIG_SIZE;
    PVOID registerBuffer = NULL; // paged
    ULONG registerBufferSize = 0;
    
    PBDF bdf = &CommandSpiParams->BDF;
    UINT16 barId = CommandSpiParams->BarId;

#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    UINT8 mapOutParams[MEM_MAP_OUT_SIZE] = { 0 };
    BOOLEAN spiBarMapped = FALSE;
    PVOID lockedSpiBar = NULL;
#endif

    //
    // allocate registerBuffer

    DPrint("cfgBufferSize: 0x%x\n", cfgBufferSize);
    cfgBuffer = malloc(cfgBufferSize);
    if ( !cfgBuffer )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }


    //
    // read pciExBarAddress
    // use mmio cfg read if possible
    // should be set already
        
    if ( PciExBarAddress )
        CommandSpiParams->Flags.ReadMode = READ_CFG_MODE_MM;
    else
        CommandSpiParams->Flags.ReadMode = READ_CFG_MODE_PORT;



    //
    // read entire cfg for log info,
    // just bar[x] is actually needed 

    if ( CommandSpiParams->Flags.ReadMode == READ_CFG_MODE_MM )
    {
        UINT64 pa = PCI_CFG_MM_ADDR(PciExBarAddress, bdf->Bus, bdf->Device, bdf->Function, 0);
        status = readPA(Device, pa, CACHE_TYPE, cfgBuffer, cfgBufferSize, READ_PA_MODE_CHUNKED_4);
        //status = readPAMmIoMapped4(Device, pa, CACHE_TYPE, cfgBuffer, cfgBufferSize);
        if ( status != 0 )
        {
            EPrint("readPAMmIoMapped4 config %02x:%02x:%02x failed! (0x%x)\n",bdf->Bus, bdf->Device, bdf->Function,  status);
            goto clean;
        }
    }
    else
    {
        ULONG parts = cfgBufferSize / 4;
        ULONG rest = cfgBufferSize % 4;

        PUINT32 bufferPtr = (PUINT32)cfgBuffer;
        UINT32 index = 0;
        offset = 0;
        UINT32 value32 = 0;

        if ( rest != 0 )
        {
            status = STATUS_INVALID_PARAMETER_3;
            EPrint("Length has to be 4-Byte aligned! (0x%x)\n", status);
            goto clean;
        }
    
        //RtlZeroMemory(cfgBuffer, cfgBufferSize);
        
        // read in 32 bit chunks
        for ( i = 0; i < parts; i++ )
        {
            index = PCI_CFG_PORT_IO_ADDR(bdf->Bus, bdf->Device, bdf->Function, offset);
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
#ifdef DEBUG_PRINT_MEM
    DPrintMemCol8(cfgBuffer, cfgBufferSize, 0);
#endif
    if ( Log->File )
    {
        status = logCfg(bdf, cfgBuffer, cfgBufferSize, Log);
        if ( status != 0 )
        {
            EPrint("Writing config to file failed! (0x%x)\n", status);
            //goto clean;
        }
    }

    if ( isReadDenied(cfgBuffer, 0x40) )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("Spi Cfg is read protected!\n");
        goto clean;
    }


    //
    // get spi bar address
    // Normally SPI Bar is always the first...

    UINT32 spiBarAddress = 0;
    offset = (UINT32)offsetof(PCI_CONFIG_GENERAL, Bar[barId]);
    DPrint("offset: 0x%x\n", offset);

    spiBarAddress = ((PPCI_CONFIG_GENERAL)cfgBuffer)->Bar[barId];
    DPrint("bar[%u]: %p\n", barId, (PVOID)spiBarAddress);

    if ( ! BAR_VALUE_VALID(spiBarAddress) )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("No valid bar value found! (0x%x)\n", status);
        goto clean;
    }

    DPrint("spiBarAddress: %p\n", (PVOID)spiBarAddress);
    

    
    //
    // calculate size of bar memory
    // [considering spi bar is not used now]
    // usually it's a page size

    UINT64 spiBarSize = (UINT64)-1;
    if ( CommandSpiParams->Flags.ReadMode == READ_CFG_MODE_MM )
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
        status = calculateBarSizeMMioMapped(Device, PciExBarAddress, bdf, offset, &spiBarSize);
#else
        status = calculateBarSizePA(Device, PciExBarAddress, bdf, offset, &spiBarSize);
#endif
    else if ( CommandSpiParams->Flags.ReadMode == READ_CFG_MODE_PORT )
        status = calculateBarSizePortIo(Device, bdf, offset, &spiBarSize);
    DPrint("Spi bar size: 0x%llx\n", spiBarSize);
    if ( status != 0 || spiBarSize == 0 )
        spiBarSize = PAGE_SIZE;


    //
    // alloc spi register buffer

    registerBufferSize = (UINT32)spiBarSize;
    registerBuffer = malloc(registerBufferSize);
    if ( !registerBuffer )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }
    
    //
    // read register

    PBAR bar = (PBAR)&spiBarAddress;
    PVOID spiBarBase = getBarBaseAddress(bar, NULL);
    if ( !spiBarBase )
    {
        status = STATUS_UNSUCCESSFUL; 
        EPrint("spiBarBase is NULL! (0x%x)\n", status);
        goto clean;
    }
    
    if ( Log->File )
    {
        RtlStringCchPrintfExA(logBufferPtr, logBufferRestSize, &logBufferPtr, &logBufferRestSize, 0,
                            "SpiBar (%s, %s)\r\n", 
                            BAR_TYPE_STR(bar),
                            BAR_ADDRESS_WIDTH(bar));
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);
    }

    status = readSpiRegister(Device, spiBarBase, spiBarSize, registerBuffer, registerBufferSize, Log);
    if ( status != 0 )
    {
        EPrint("readSpiRegister failed! (0x%x)\n", status);
        goto clean;
    }

    
    //
    // map spi bar (mmio)
    
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    status = mapMemory(Device, spiBarBase, 0x100, &lockedSpiBar, &mapOutParams, MEM_MAP_OUT_SIZE);
    if ( status != 0 )
    {
        EPrint("mapMemory request failed! (0x%x)\n", status);
        goto clean;
    }
    spiBarMapped = TRUE;
#endif




    //
    // issue spi command loop
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    status = issueSpiCommandLoop(Device, CommandSpiParams, registerBuffer, spiBarBase, lockedSpiBar);
#else
    status = issueSpiCommandLoop(Device, CommandSpiParams, registerBuffer, spiBarBase, NULL);
#endif
    if ( status != 0 )
    {
        EPrint("issueSpiCommandLoop() failed! (0x%x)\n", status);
        goto clean;
    }

clean:
    
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED 
    if ( spiBarMapped )
    {
        unmapMemory(Device, lockedSpiBar, mapOutParams, MEM_MAP_OUT_SIZE);
    }
#endif

    if ( Log->File )
        FlushLogFileBuffer(Log->File, Log->Buffer, (ULONG)Log->BufferSize, &logBufferPtr, &logBufferRestSize);

    if ( cfgBuffer )
        free(cfgBuffer);
    if ( registerBuffer )
        free(registerBuffer);

    FLeave();
    return status;
}
DEFAULT_WARNING(6387)

NTSTATUS issueSpiCommandLoop(
    _In_ HANDLE Device,
    _In_ PCOMMAND_SPI_PARAMS CommandSpiParams,
    _In_ PVOID RegisterBuffer,
    _In_ PVOID SpiBarBase,
    _In_opt_ PVOID MappedSection
)
{
    FEnter();

    NTSTATUS status = 0;
    
    // issue spi command params
    PUINT8 inputPartPtr = NULL;
    UINT32 inputPartSize = 0;
    PVOID output = NULL;
    UINT32 outputSize = 0;
    UINT32 outputWritten = 0;
    UINT32 startAddress = CommandSpiParams->LinearAddress;

    if ( CommandSpiParams->CycleCommand != FCYCLE_WRITE
      && CommandSpiParams->CycleCommand != FCYCLE_BLOCK_ERASE 
      && CommandSpiParams->CycleCommand != FCYCLE_SEC_ERASE 
      && CommandSpiParams->CycleCommand != FCYCLE_WRITE_STATUS )
    {
        outputSize = CommandSpiParams->DataSize;
        output = malloc(outputSize);
        if ( !output )
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean;
        }
    }
    else if ( CommandSpiParams->CycleCommand == FCYCLE_WRITE 
           || CommandSpiParams->CycleCommand == FCYCLE_WRITE_STATUS )
    {
        inputPartPtr = CommandSpiParams->Data;
        //inputSize = CommandSpiParams->DataSize;
        inputPartSize = FDATA_MAX_SIZE;
    }

    UINT32 parts = CommandSpiParams->DataSize / FDATA_MAX_SIZE;
    UINT32 rest = CommandSpiParams->DataSize % FDATA_MAX_SIZE;
    UINT32 part = 0;
    PUINT8 outputPtr = output;
    UINT32 outputRestSize = outputSize;

    // set zero based FDBC to max size -1
    CommandSpiParams->FDBC = FDATA_MAX_SIZE-1;
    for ( part = 0; part < parts; part++ )
    {
        //PVOID spiBarBase = getBarBaseAddress((PBAR)&spiBarAddress, NULL);
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
        status = issueSpiCommandMm(
                    Device, 
                    RegisterBuffer, 
                    SpiBarBase, 
                    CommandSpiParams, 
                    MappedSection,
                    inputPartPtr,
                    inputPartSize,
                    outputPtr,
                    outputRestSize,
                    &outputWritten
                );
#else
        status = issueSpiCommandPA(
                    Device, 
                    RegisterBuffer, 
                    SpiBarBase, 
                    CommandSpiParams, 
                    inputPartPtr,
                    inputPartSize,
                    outputPtr,
                    outputRestSize,
                    &outputWritten
                );
#endif
        if ( status != 0 )
        {
            EPrint("issueSpiCommand failed! (0x%x)\n", status);
            goto clean;
        }

        // advance pointer and address, decrement sizes
        outputPtr += FDATA_MAX_SIZE;
        outputRestSize -= FDATA_MAX_SIZE;
        if ( inputPartPtr )
            inputPartPtr += FDATA_MAX_SIZE;
        CommandSpiParams->LinearAddress += FDATA_MAX_SIZE;
    }

    if ( rest )
    {
        CommandSpiParams->FDBC = (UINT8)rest-1;
        if ( inputPartPtr )
            inputPartSize = rest;

        //PVOID spiBarBase = getBarBaseAddress((PBAR)&spiBarAddress, NULL);
#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
        status = issueSpiCommandMm(
                    Device, 
                    RegisterBuffer, 
                    SpiBarBase, 
                    CommandSpiParams, 
                    MappedSection,
                    inputPartPtr,
                    inputPartSize,
                    outputPtr,
                    outputRestSize,
                    &outputWritten
                );
#else
        status = issueSpiCommandPA(
                    Device, 
                    RegisterBuffer, 
                    SpiBarBase, 
                    CommandSpiParams, 
                    inputPartPtr,
                    inputPartSize,
                    outputPtr,
                    outputRestSize,
                    &outputWritten
                );
#endif
        if ( status != 0 )
        {
            EPrint("issueSpiCommand failed! (0x%x)\n", status);
            goto clean;
        }

        //outputPtr += rest;
        //outputRestSize -= rest;
    }
    
    if ( output )
    {
        //if ( Params->Flags.Print )
        PrintMemCol8(output, CommandSpiParams->DataSize, startAddress);
    }

clean:
    if ( output )
        free(output);

    FLeave();
    return status;
}

#ifdef RW_PRIM_MEM_MAPPING_SUPPORTED
//
// reading spi flash via commands
// spi bar is already mapped making virtual rw possible
// 
// 1. Write the size-1 of data bytes we’d like to read to HSFSTS_CTL.FDBC
// 2. Set HSFSTS_CTL.FCYCLE to 0b00 to indicate a flash read operation
// 3. Write the flash offset we’re interested in to the FADDR register
// 4. Trigger a flash cycle by setting the HSFSTS_CTL.FGO bit
// 5. Wait until data is ready by polling HSFSTS_CTL.H_SCIP
// 6. Read the returned data from the relevant FDATAn register
// 7. Jump to 3 and repeat as necessary
//
NTSTATUS issueSpiCommandMm(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ PCOMMAND_SPI_PARAMS CommandSpiParams,
    _In_ PVOID MappedSection,
    _In_opt_ PVOID Input,
    _In_ UINT32 InputSize,
    _Inout_opt_ PVOID Output,
    _In_ UINT32 OutputSize,
    _Out_opt_ PUINT32 OutputWritten
)
{
    FEnter();
    (Device);

    NTSTATUS status = 0;

    PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP spiRegisterData = (PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP)SpiRegisterDataBuffer;
    BIOS_HSFSTS_CTL hsfsts_ctl = spiRegisterData->HSFSTS_CTL;
    BIOS_FADDR faddr = spiRegisterData->FADDR;

    UINT32 HSFSTS_CTL_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, HSFSTS_CTL); // 04
    UINT32 FADDR_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FADDR); // 08
    UINT32 FDATA_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FDATA[0]); // 10 - 4F
    
    //LARGE_INTEGER pa;
    //CM_PAGE_DATA rwPageData = { 0 };
    volatile PUINT8 hsfsts_ctl_va = NULL;
    volatile PUINT8 faddr_va = NULL;

    UINT32 fdata[FDATA_COUNT];
    UINT32 fdataSize = (CommandSpiParams->FDBC+1);
    UINT32 fdataCount = ALIGN_UP_BY(fdataSize, 4) / sizeof(UINT32);

    //LARGE_INTEGER interval;
    
    if ( OutputWritten )
        *OutputWritten = 0;

    //if ( spiRegisterData->HSFSTS_CTL.FDV != 1 )
    //{
    //    status = STATUS_INVALID_PARAMETER;
    //    EPrint("FDV not valid! (0x%x)\n", status);
    //    goto clean;
    //}
    
    DPrint("CycleCommand: 0x%x (%s)\n", CommandSpiParams->CycleCommand, FCYCLE_TYPE_STR(CommandSpiParams->CycleCommand));
    DPrint("LinearAddress: 0x%x\n", CommandSpiParams->LinearAddress);
    DPrint("FDBC: 0x%x\n", CommandSpiParams->FDBC);
    DPrint("DataSize: 0x%x\n", CommandSpiParams->DataSize);
    DPrint("Data: %p\n", CommandSpiParams->Data);
    if ( Input )
    {
        DPrintMemCol8(Input, InputSize, 0);
    }


    //
    // save virtual addresses

    UINT32 faddrPageOffset = ((UINT64)SpiBaseAddress + FADDR_Offset)%PAGE_SIZE;
    faddr_va = ((PUINT8)MappedSection + faddrPageOffset);

    UINT32 hsfstsPageOffset = ((UINT64)SpiBaseAddress + HSFSTS_CTL_Offset)%PAGE_SIZE;
    hsfsts_ctl_va = ((PUINT8)MappedSection + hsfstsPageOffset);

    UINT32 fdataPageOffset = ((UINT64)SpiBaseAddress + FDATA_Offset) % PAGE_SIZE;
    PUINT8 fdata_va = (PUINT8)MappedSection + fdataPageOffset;
    PUINT32 fdata_va_ptr = NULL;


    //
    // check HSFS.H_SCIP first, if someone else currently dumps the memory
    
    //status = checkCycleDone((PUINT8)hsfsts_ctl);
    status = checkCycleInProgressMm(hsfsts_ctl_va);
    if ( status != 0 )
        goto clean;

    //
    // put write data into data regs
    
    if ( Input && InputSize )
    {
        fdata_va_ptr = (PUINT32)fdata_va;
        PUINT8 ipt_ptr = Input;
        //UINT32 wdata = 0;
        UINT32 parts = InputSize / 4;
        UINT32 restCb = InputSize % 4;

        for ( UINT32 bi = 0; bi < parts; bi++ )
        {
            *fdata_va_ptr = *(PUINT32)ipt_ptr;
                
            ipt_ptr += 4;
            ++fdata_va_ptr;
        }
        if ( restCb )
        {
            UINT32 restU32 = 0;
            RtlCopyMemory(&restU32, ipt_ptr, restCb);
            *fdata_va_ptr = restU32;
        }
    }


    //
    // do the cycle
    
    try
    {
        //DPrint("  HSFSTS_CTL: 0x%x\n",*(PUINT32)&hsfsts_ctl);
        //DPrint("  FCERR: 0x%x\n",hsfsts_ctl.FCERR);
        //DPrint("  H_AEL: 0x%x\n",hsfsts_ctl.H_AEL);
        //DPrint("  H_SCIP: 0x%x\n",hsfsts_ctl.H_SCIP);
        //DPrint("  FGO: 0x%x\n",hsfsts_ctl.FGO);
        //DPrint("  FCYCLE: 0x%x\n",hsfsts_ctl.FCYCLE);
        //DPrint("  WET: 0x%x\n",hsfsts_ctl.WET);
        //DPrint("  FADDR.FLA: 0x%x\n",faddr.FLA);

        //
        // write address

        faddr.FLA = CommandSpiParams->LinearAddress;

        *(PUINT32)faddr_va = *(PUINT32)&faddr;

        //
        // write command

        hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
        hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
        hsfsts_ctl.FGO = 1;
        hsfsts_ctl.FCYCLE = CommandSpiParams->CycleCommand;
        hsfsts_ctl.FDBC = CommandSpiParams->FDBC;

        *(PUINT32)hsfsts_ctl_va =  *(PUINT32)&hsfsts_ctl;
        
        
        //
        // Wait for SPI read cycle completion (HSFS.H_SCIP == 0, HSFS.FDONE == 1)
        
        status = checkCycleInProgressMm(hsfsts_ctl_va);
        if ( status != 0 )
            goto clean;
        
        
        //
        // check for error
        
        if ( ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->FCERR 
            || ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->H_AEL )
        {
            //status = STATUS_UNSUCCESSFUL;
            //EPrint("Reading not successful! (0x%x)\n", status);
            EPrint("Command 0x%x (%s) not successful!\n"
                   "    FCERR: 0x%x\n"
                   "    H_AEL: 0x%x\n",
                   CommandSpiParams->CycleCommand,
                   FCYCLE_TYPE_STR(CommandSpiParams->CycleCommand),
                   ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->FCERR, 
                   ((PBIOS_HSFSTS_CTL)hsfsts_ctl_va)->H_AEL);
            
            status = STATUS_UNSUCCESSFUL;
            goto clean;
        }
        
        //
        // skip reading if writing mode
        
        if ( CommandSpiParams->CycleCommand == FCYCLE_WRITE
            || CommandSpiParams->CycleCommand == FCYCLE_WRITE_STATUS )
        {
            goto clean;
        }
        
        //
        // read all FDATA[n] at once
        
        RtlZeroMemory(fdata, fdataSize);
        PUINT32 fdata_loc_ptr = (PUINT32)&fdata;
        
        //pa.QuadPart = (UINT64)SpiBaseAddress + FDATA_Offset;
        //pageOffset = pa.QuadPart % PAGE_SIZE;
        //va = (PUINT8)MappedSection + pageOffset;
        fdata_va_ptr = (PUINT32)fdata_va;
        //DPrint("read from %p\n", (PVOID)pa.QuadPart);
        
        for ( UINT32 bi = 0; bi < fdataCount; bi++ )
        {
            *fdata_loc_ptr = *fdata_va_ptr;
                
            ++fdata_loc_ptr;
            ++fdata_va_ptr;
        }
        
        DPrintMemCol8(fdata, fdataSize, CommandSpiParams->LinearAddress);
        //
        // write to buffer
        // TODO: can be written directly to output buffer
        
    //fill_buffer:
        if ( Output && OutputSize >= fdataSize )
        {
            RtlCopyMemory(Output, fdata, fdataSize);
            
            if ( OutputWritten )
                *OutputWritten += fdataSize;
        }
    }
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        EPrint("Spi cycle failed! (0x%x)", status);
        goto clean;
    }
    
clean:
    // reset command status register
    //if ( hsfsts_ctl_va )
    //{
    //    *(PUINT32)&hsfsts_ctl = 0;
    //    hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
    //    hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
    //     *(PUINT32)hsfsts_ctl_va =  *(PUINT32)&hsfsts_ctl;
    //}
    //restorePfn(&rwPageData);
    
    FLeave();
    return status;
}
#endif

#ifndef RW_PRIM_MEM_MAPPING_SUPPORTED
//
// reading spi flash via commands
// spi bar is already mapped making virtual rw possible
// 
// 1. Write the size-1 of data bytes we’d like to read to HSFSTS_CTL.FDBC
// 2. Set HSFSTS_CTL.FCYCLE to 0b00 to indicate a flash read operation
// 3. Write the flash offset we’re interested in to the FADDR register
// 4. Trigger a flash cycle by setting the HSFSTS_CTL.FGO bit
// 5. Wait until data is ready by polling HSFSTS_CTL.H_SCIP
// 6. Read the returned data from the relevant FDATAn register
// 7. Jump to 3 and repeat as necessary
//
NTSTATUS issueSpiCommandPA(
    _In_ HANDLE Device,
    _Inout_ PVOID SpiRegisterDataBuffer, // read out data, not really necessary
    _In_ PVOID SpiBaseAddress, // original address used for spi io
    _In_ PCOMMAND_SPI_PARAMS CommandSpiParams,
    _In_opt_ PVOID Input,
    _In_ UINT32 InputSize,
    _Inout_opt_ PVOID Output,
    _In_ UINT32 OutputSize,
    _Out_opt_ PUINT32 OutputWritten
)
{
    FEnter();
    (Device);

    NTSTATUS status = 0;

    PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP spiRegisterData = (PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP)SpiRegisterDataBuffer;
    BIOS_HSFSTS_CTL hsfsts_ctl = spiRegisterData->HSFSTS_CTL;
    BIOS_FADDR faddr = spiRegisterData->FADDR;

    UINT32 HSFSTS_CTL_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, HSFSTS_CTL); // 04
    UINT32 FADDR_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FADDR); // 08
    UINT32 FDATA_Offset = (UINT32)offsetof(INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, FDATA[0]); // 10 - 4F
    
    PUINT8 hsfsts_ctl_pa = NULL;
    PUINT8 faddr_pa = NULL;

    UINT32 fdata[FDATA_COUNT];
    UINT32 fdataSize = (CommandSpiParams->FDBC+1);
    UINT32 fdataCount = ALIGN_UP_BY(fdataSize, 4) / sizeof(UINT32);
    
    UINT32 readPageFlags = CACHE_TYPE;
    UINT32 writePageFlags = 0;

    if ( OutputWritten )
        *OutputWritten = 0;

    DPrint("CycleCommand: 0x%x (%s)\n", CommandSpiParams->CycleCommand, FCYCLE_TYPE_STR(CommandSpiParams->CycleCommand));
    DPrint("LinearAddress: 0x%x\n", CommandSpiParams->LinearAddress);
    DPrint("FDBC: 0x%x\n", CommandSpiParams->FDBC);
    DPrint("DataSize: 0x%x\n", CommandSpiParams->DataSize);
    DPrint("Data: %p\n", CommandSpiParams->Data);
    if ( Input )
    {
        DPrintMemCol8(Input, InputSize, 0);
    }


    //
    // save addresses
    
    faddr_pa = ((PUINT8)SpiBaseAddress + FADDR_Offset);

    hsfsts_ctl_pa = ((PUINT8)SpiBaseAddress + HSFSTS_CTL_Offset);

    PUINT8 fdata_pa = (PUINT8)SpiBaseAddress + FDATA_Offset;
    PUINT32 fdata_pa_ptr = NULL;


    //
    // check HSFS.H_SCIP first, if someone else currently dumps the memory
    
    //status = checkCycleDone((PUINT8)hsfsts_ctl);
    status = checkCycleInProgressPA(Device, hsfsts_ctl_pa, (PUINT32)&hsfsts_ctl);
    if ( status != 0 )
        goto clean;

    //
    // put write data into data regs
    
    if ( Input && InputSize )
    {
        fdata_pa_ptr = (PUINT32)fdata_pa;
        PUINT8 ipt_ptr = Input;
        //UINT32 wdata = 0;
        UINT32 parts = InputSize / 4;
        UINT32 restCb = InputSize % 4;

        for ( UINT32 bi = 0; bi < parts; bi++ )
        {
            status = writePA(Device, (UINT64)fdata_pa_ptr, writePageFlags, (PUINT32)ipt_ptr, 4, WRITE_PA_MODE_4);
            if ( status != 0 )
                goto clean;
                
            ipt_ptr += 4;
            ++fdata_pa_ptr;
        }
        if ( restCb )
        {
            UINT32 restU32 = 0;
            RtlCopyMemory(&restU32, ipt_ptr, restCb);
            
            status = writePA(Device, (UINT64)fdata_pa_ptr, writePageFlags, &restU32, restCb, WRITE_PA_MODE_DEF);
            if ( status != 0 )
                goto clean;
        }
    }


    //
    // do the cycle
    
    try
    {
        //DPrint("  HSFSTS_CTL: 0x%x\n",*(PUINT32)&hsfsts_ctl);
        //DPrint("  FCERR: 0x%x\n",hsfsts_ctl.FCERR);
        //DPrint("  H_AEL: 0x%x\n",hsfsts_ctl.H_AEL);
        //DPrint("  H_SCIP: 0x%x\n",hsfsts_ctl.H_SCIP);
        //DPrint("  FGO: 0x%x\n",hsfsts_ctl.FGO);
        //DPrint("  FCYCLE: 0x%x\n",hsfsts_ctl.FCYCLE);
        //DPrint("  WET: 0x%x\n",hsfsts_ctl.WET);
        //DPrint("  FADDR.FLA: 0x%x\n",faddr.FLA);

        //
        // write address

        faddr.FLA = CommandSpiParams->LinearAddress;

        status = writePA(Device, (UINT64)faddr_pa, writePageFlags, (PUINT32)&faddr, 4, WRITE_PA_MODE_4);
        if ( status )
        {
            EPrint("Writing faddr (%p) failed! (0x%x)\n", faddr_pa, status);
            goto clean;
        }

        //
        // write command

        hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
        hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
        hsfsts_ctl.FGO = 1;
        hsfsts_ctl.FCYCLE = CommandSpiParams->CycleCommand;
        hsfsts_ctl.FDBC = CommandSpiParams->FDBC;

        status = writePA(Device, (UINT64)hsfsts_ctl_pa, writePageFlags, (PUINT32)&hsfsts_ctl, 4, WRITE_PA_MODE_4);
        if ( status )
        {
            EPrint("Writing hsfsts (%p) failed! (0x%x)\n", hsfsts_ctl_pa, status);
            goto clean;
        }
        
        
        //
        // Wait for SPI read cycle completion (HSFS.H_SCIP == 0, HSFS.FDONE == 1)
        
        status = checkCycleInProgressPA(Device, hsfsts_ctl_pa, (PUINT32)&hsfsts_ctl);
        if ( status != 0 )
        {
            goto clean;
        }
        
        
        //
        // check for error
        
        status = checkErrorPA(Device, hsfsts_ctl_pa, (PUINT32)&hsfsts_ctl);
        if ( status != 0 )
        {
            EPrint("Command 0x%x (%s) not successful!\n",
                   CommandSpiParams->CycleCommand,
                   FCYCLE_TYPE_STR(CommandSpiParams->CycleCommand));

            status = STATUS_UNSUCCESSFUL;
            goto clean;
        }
        
        //
        // skip reading if writing mode
        
        if ( CommandSpiParams->CycleCommand == FCYCLE_WRITE
            || CommandSpiParams->CycleCommand == FCYCLE_WRITE_STATUS )
        {
            goto clean;
        }
        
        //
        // read all FDATA[n] at once
        
        RtlZeroMemory(fdata, fdataSize);
        PUINT32 fdata_loc_ptr = (PUINT32)&fdata;
        
        //pa.QuadPart = (UINT64)SpiBaseAddress + FDATA_Offset;
        //pageOffset = pa.QuadPart % PAGE_SIZE;
        //va = (PUINT8)MappedSection + pageOffset;
        fdata_pa_ptr = (PUINT32)fdata_pa;
        //DPrint("read from %p\n", (PVOID)pa.QuadPart);
        
        for ( UINT32 bi = 0; bi < fdataCount; bi++ )
        {
            status = readPA(Device, (UINT64)fdata_pa_ptr, readPageFlags, fdata_loc_ptr, 4, READ_PA_MODE_4);
            if ( status )
            {
                EPrint("Reading fdata (%p) failed! (0x%x)\n", fdata_pa_ptr, status);
                break;
            }
            
            ++fdata_loc_ptr;
            ++fdata_pa_ptr;
        }
        
        DPrintMemCol8(fdata, fdataSize, CommandSpiParams->LinearAddress);
        //
        // write to buffer
        // TODO: can be written directly to output buffer
        
    //fill_buffer:
        if ( Output && OutputSize >= fdataSize )
        {
            RtlCopyMemory(Output, fdata, fdataSize);
            
            if ( OutputWritten )
                *OutputWritten += fdataSize;
        }
    } 
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        EPrint("Spi cycle failed! (0x%x)", status);
        goto clean;
    }

clean:
    // reset command status register
    //if ( hsfsts_ctl_va )
    //{
    //    *(PUINT32)&hsfsts_ctl = 0;
    //    hsfsts_ctl.FCERR = 1; // software clears this bit by writing a 1 ?!
    //    hsfsts_ctl.H_AEL = 1; // software clears this bit by writing a 1 ?!
    //     *(PUINT32)hsfsts_ctl_va =  *(PUINT32)&hsfsts_ctl;
    //}
    //restorePfn(&rwPageData);
    
    FLeave();
    return status;
}
#endif
