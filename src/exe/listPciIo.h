#include <stdint.h>

#pragma pack(1)

typedef struct _CFG_DATA {
    BDF Bdf;
    UINT16 VendorId;
    UINT16 DeviceId;
} CFG_DATA, *PCFG_DATA;

typedef struct _BDF_DATA {
    PCFG_DATA List;
    UINT32 Count;
    UINT32 MaxCount;
    UINT32 Size;
} BDF_DATA, *PBDF_DATA;

typedef struct _ENUM_CTXT {
    HANDLE Device;
    PPCI_IO PciIoParams;
    BDF_DATA BdfData;
    struct {
        PVOID Buffer;
        UINT32 Size;
    } Cfg;
} ENUM_CTXT, *PENUM_CTXT;

#pragma pack()



NTSTATUS listPciIo(
    _In_ HANDLE Device, 
    _In_ PCMD_PARAMS Params,
    _Inout_ PVOID *OutputBuffer,
    _Inout_ PUINT32 OutputBufferSize,
    _Out_ PUINT32 Count
);
NTSTATUS checkAllBuses(PENUM_CTXT Ctxt);

NTSTATUS readCfg(_In_ HANDLE Device, _In_ PBDF bdf, _In_ PPCI_IO pciIoParams, _Inout_ PVOID cfgBuffer, _In_ UINT32 cfgBufferSize);

NTSTATUS checkDevice(PENUM_CTXT Ctxt, uint8_t bus, uint8_t device);
NTSTATUS checkBus(PENUM_CTXT Ctxt, uint8_t bus);
NTSTATUS checkFunction(PENUM_CTXT Ctxt, uint8_t b, uint8_t d, uint8_t f);

uint16_t getVendorID(HANDLE Device, uint8_t b, uint8_t d, uint8_t f);
uint16_t getDeviceID(HANDLE Device, uint8_t b, uint8_t d, uint8_t f);
uint8_t getHeaderType(HANDLE Device, uint8_t b, uint8_t d, uint8_t f);
uint8_t getClassCode(HANDLE Device, uint8_t b, uint8_t d, uint8_t f);
uint8_t getSubClass(HANDLE Device, uint8_t b, uint8_t d, uint8_t f);
uint8_t getSecondaryBus(HANDLE Device, uint8_t b, uint8_t d, uint8_t f);
uint32_t getCfgValue(HANDLE Device, uint8_t b, uint8_t d, uint8_t f, uint8_t o);

NTSTATUS addBdfData(
    _In_ PBDF_DATA BdfData, 
    _In_ uint8_t b, 
    _In_ uint8_t d, 
    _In_ uint8_t f,
    _In_ uint16_t vId,
    _In_ uint16_t dId
);



NTSTATUS listPciIo(
    _In_ HANDLE Device, 
    _In_ PCMD_PARAMS Params,
    _Inout_ PVOID *OutputBuffer,
    _Inout_ PUINT32 OutputBufferSize,
    _Out_ PUINT32 Count
)
{
    NTSTATUS status = 0;

    FEnter();

    //UINT64 barBufferSize = 0;
    //PVOID barBuffer = NULL;
    *Count = 0;

    PPCI_IO pciIoParams = &Params->PciIo;
    
    ENUM_CTXT ctxt = { 0 };
    ctxt.Device = Device;
    ctxt.PciIoParams = &Params->PciIo;

    LOG log = { 0 };
    log.Dir = Params->LogDir;
    log.BufferSize = PAGE_SIZE;
    
    SIZE_T logBufferRestSize = 0;
    PCHAR logBufferPtr = NULL;

    //PWCHAR filePath = NULL;
    //UINT32 filePathCch = 0;
    //HANDLE logFile = NULL;
    
    //UINT64 pciExBarAddress = 0;
    //UINT64 pciExBarSize = 0;
    
    //ctxt.Cfg.Size = PCI_CONFIG_SIZE;
    ctxt.Cfg.Size = 0x40;
    ctxt.Cfg.Buffer = malloc(ctxt.Cfg.Size);
    if ( !ctxt.Cfg.Buffer )
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean;
    }

    // alloc memory for bdf data array 
    if ( *OutputBuffer )
    {
        ctxt.BdfData.Size = *OutputBufferSize;
        ctxt.BdfData.List = *OutputBuffer;
    }
    else
    {
        ctxt.BdfData.Size = 0x100;
        SUPPRESS_WARNING( 6014 );
        ctxt.BdfData.List = (PCFG_DATA)malloc(ctxt.BdfData.Size);
        if ( !ctxt.BdfData.List )
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean;
        }
    }
    ctxt.BdfData.MaxCount = ctxt.BdfData.Size / sizeof(CFG_DATA);
    
    // alloc memory for logging
    if ( Params->Flags.WriteToFile )
    {
        log.Buffer = malloc(log.BufferSize);
        if ( !log.Buffer )
            goto clean;

        logBufferRestSize = log.BufferSize;
        logBufferPtr = log.Buffer;
    }
    
    // currently just port io is used
    // TODO: see getCfgValue
    // get pciExBarAddress if needed
    //if ( pciIoParams->Flags.ReadMode == 0
    //  || pciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
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
    //            if ( pciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
    //                goto clean;
    //            else
    //                pciIoParams->Flags.ReadMode = READ_CFG_MODE_PORT;
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
    //        pciIoParams->PciExBarAddress = pciExBarAddress;
    //        pciIoParams->PciExBarSize = pciExBarSize;
    //        pciIoParams->Flags.ReadMode = READ_CFG_MODE_MM;
    //    }
    //}

    // start enumerating
    status = checkAllBuses(&ctxt);

    //DPrint("BDF:\n");
    //DPrintMemCol8(ctxt.BdfData.List, ctxt.BdfData.Size, 0);

    *OutputBuffer = ctxt.BdfData.List;
    *OutputBufferSize = ctxt.BdfData.Size;
    *Count = ctxt.BdfData.Count;

clean:

    if ( status != 0)
    {
        if ( ctxt.BdfData.List )
            free(ctxt.BdfData.List);
        *OutputBuffer = NULL;
        *OutputBufferSize = 0;
    }

    if ( ctxt.Cfg.Buffer)
        free(ctxt.Cfg.Buffer);

    FLeave();
    return status;
}

// The final step is to handle systems with multiple PCI host controllers correctly. 
// Start by checking if the device at bus 0, device 0 is a multi-function device. 
// If it's not a multi-function device, 
// then there is only one PCI host controller and bus 0, device 0, function 0 will be the PCI host controller responsible for bus 0. 
// If it's a multi-function device, then bus 0, device 0, function 0 will be the PCI host controller responsible for bus 0; 
// bus 0, device 0, function 1 will be the PCI host controller responsible for bus 1, etc (up to the number of functions supported).
NTSTATUS checkAllBuses(
    PENUM_CTXT Ctxt
)
{
    FEnter();

    NTSTATUS status = 0;

    uint8_t function;
    uint8_t bus;

    //
    // read in config of 0:0:0

    //BDF bdf = { 0, 0, 0 };
    //status = readCfg(Ctxt->Device, &bdf, Ctxt->PciIoParams, Ctxt->Cfg.Buffer, Ctxt->Cfg.Size);
    //if ( status != 0 )
    //{
    //    goto clean;
    //}

    uint8_t headerType = getHeaderType(Ctxt->Device, 0, 0, 0);
    DPrint("00:00:00 headerType: 0x%x\n", headerType);
    if ( (headerType & 0x80) == 0 )
    {
        // Single PCI host controller
        //status = addBdf(&Ctxt->BdfData, 0, 0, 0);
        //if ( status != 0 )
        //    goto clean;

        checkBus(Ctxt, 0);
    }
    else
    {
        // Multiple PCI host controllers
        for ( function = 0; function < 8; function++ )
        {
            UINT16 vendorId = getVendorID(Ctxt->Device, 0, 0, function);
            if ( vendorId == 0xFFFF )
                break;
            
            DPrint("00:00:%02x vendorId: 0x%x\n", function, vendorId);

            //status = addBdf(&Ctxt->BdfData, 0, 0, 0, vendorId, 0);
            //if ( status != 0 )
            //    goto clean;

            bus = function;
            checkBus(Ctxt, bus);
        }
    }

//clean:
    FLeave();
    return status;
}

// The first step for the recursive scan is to implement a function that scans one bus.
NTSTATUS checkBus(PENUM_CTXT Ctxt, uint8_t bus)
{
    FEnter();
    NTSTATUS status = 0;

    uint8_t device;

    for ( device = 0; device < 32; device++ )
    {
        checkDevice(Ctxt, bus, device);
    }

    FLeave();
    return status;
}

NTSTATUS checkDevice(PENUM_CTXT Ctxt, uint8_t bus, uint8_t device)
{
    FEnter();
    NTSTATUS status = 0;

    uint8_t function = 0;
    uint16_t deviceId = 0;

    uint16_t vendorId = getVendorID(Ctxt->Device, bus, device, function);
    
    DPrint("%02x:%02x:00 vendorId: 0x%x\n", bus, device, vendorId);

    if ( vendorId == 0xFFFF )
    {
        status = STATUS_NOT_FOUND; // Device doesn't exist
        goto clean;
    }
    
    deviceId = getDeviceID(Ctxt->Device, bus, device, function);
    DPrint("%02x:%02x:00 deviceId: 0x%x\n", bus, device, deviceId);

    status = addBdfData(&Ctxt->BdfData, bus, device, function, vendorId, deviceId);
    if ( status != 0 )
        goto clean;

    checkFunction(Ctxt, bus, device, function);
    
    uint8_t headerType = getHeaderType(Ctxt->Device, bus, device, function);
    DPrint("%02x:%02x:00 headerType: 0x%x\n", bus, device, headerType);
    if ( (headerType & 0x80) != 0 )
    {
        // It's a multi-function device, so check remaining functions
        for ( function = 1; function < 8; function++ )
        {
            vendorId = getVendorID(Ctxt->Device, bus, device, function);
            DPrint("%02x:%02x:%02x vendorId: 0x%04x\n", bus, device, function, vendorId);
            if ( vendorId == 0xFFFF )
                continue;

            deviceId = getDeviceID(Ctxt->Device, bus, device, function);
            DPrint("%02x:%02x:%02x deviceId: 0x%04x\n", bus, device, function, deviceId);
            status = addBdfData(&Ctxt->BdfData, bus, device, function, vendorId, deviceId);
            if ( status != 0 )
                goto clean;

            checkFunction(Ctxt, bus, device, function);
        }
    }

clean:
    FLeave();
    return status;
}

// The next step is to add code in "checkFunction()" that detects if the function is a PCI to PCI bridge. 
// If the device is a PCI to PCI bridge then you want to extract the "secondary bus number" from the bridge's configuration space and call "checkBus()" with the number of the bus on the other side of the bridge.
NTSTATUS checkFunction(PENUM_CTXT Ctxt, uint8_t b, uint8_t d, uint8_t f)
{
    FEnter();
    NTSTATUS status = 0;

    uint8_t classCode;
    uint8_t subClass;
    uint8_t secondaryBus;

    classCode = getClassCode(Ctxt->Device, b, d, f);
    subClass = getSubClass(Ctxt->Device, b, d, f);
    
    DPrint("%02x:%02x:%02x\n", b, d, f);
    DPrint("  baseClass: 0x%02x\n", classCode);
    DPrint("  subClass: 0x%02x\n", subClass);

    if ( classCode == PCI_CLASS_BRIDGE_DEV
      && subClass == PCI_SUBCLASS_BR_PCI_TO_PCI )
    {
        secondaryBus = getSecondaryBus(Ctxt->Device, b, d, f);
        checkBus(Ctxt, secondaryBus);
    }
    
    FLeave();
    return status;
}



uint16_t getVendorID(HANDLE Device, uint8_t b, uint8_t d, uint8_t f)
{
    //FEnter();
    uint16_t vendorId = 0xFFFF;
    UINT32 offset = (UINT32)offsetof(PCI_CONFIG, VendorId);
    
// possible loss of data
#pragma warning ( disable : 4242 4244 )
    vendorId = (uint16_t)getCfgValue(Device, b, d, f, offset);
#pragma warning ( default : 4242 4244 )
    
    //FLeave();
    return vendorId;
}

uint16_t getDeviceID(HANDLE Device, uint8_t b, uint8_t d, uint8_t f)
{
    //FEnter();
    uint16_t deviceId = 0xFFFF;
    UINT32 offset = (UINT32)offsetof(PCI_CONFIG, DeviceId);
// possible loss of data
#pragma warning ( disable : 4242 4244 )
    deviceId = (uint16_t)getCfgValue(Device, b, d, f, offset);
#pragma warning ( default : 4242 4244 )

    //FLeave();
    return deviceId;
}

uint8_t getHeaderType(HANDLE Device, uint8_t b, uint8_t d, uint8_t f)
{
    //FEnter();

    uint8_t headerType = 0xFF;
    UINT32 offset = (UINT32)offsetof(PCI_CONFIG, HeaderType);
    
// possible loss of data
#pragma warning ( disable : 4242 4244 )
    headerType = (uint8_t)getCfgValue(Device, b, d, f, offset);
#pragma warning ( default : 4242 4244 )
    
    //FLeave();
    return headerType;
}

uint8_t getClassCode(HANDLE Device, uint8_t b, uint8_t d, uint8_t f)
{
    //FEnter();

    uint8_t classCode = 0xFF;
    UINT32 offset = (UINT32)offsetof(PCI_CONFIG, ClassCode);
    
// possible loss of data
#pragma warning ( disable : 4242 4244 )
    classCode = (uint8_t)getCfgValue(Device, b, d, f, offset);
#pragma warning ( default : 4242 4244 )

    //FLeave();
    return classCode;
}

uint8_t getSubClass(HANDLE Device, uint8_t b, uint8_t d, uint8_t f)
{
    //FEnter();

    uint8_t subClass = 0xFF;
    UINT32 offset = (UINT32)offsetof(PCI_CONFIG, Subclass);
    
// possible loss of data
#pragma warning ( disable : 4242 4244 )
    subClass = (uint8_t)getCfgValue(Device, b, d, f, offset);
#pragma warning ( default : 4242 4244 )
    
    //FLeave();
    return subClass;
}

uint8_t getSecondaryBus(HANDLE Device, uint8_t b, uint8_t d, uint8_t f)
{
    //FEnter();
    uint8_t secondaryBusNumber = 0xFF;
    UINT32 offset = (UINT32)offsetof(PCI_CONFIG_PCI_BRIDGE, SecondaryBusNumber);
    
// possible loss of data
#pragma warning ( disable : 4242 4244 )
    secondaryBusNumber = (uint8_t)getCfgValue(Device, b, d, f, offset);
#pragma warning ( default : 4242 4244 )
    
    //FLeave();
    return secondaryBusNumber;
}

// TODO: switch between portio and mmio
uint32_t getCfgValue(HANDLE Device, uint8_t b, uint8_t d, uint8_t f, uint8_t o)
{
    //FEnter();
    NTSTATUS status = 0;

    uint32_t value = 0xFFFFFFFF;

    UINT32 aligned_offset = ALIGN_DOWN_BY(o, 4);
    UINT32 offset_shift = ( o % 4 ) * 8;
    UINT32 index = PCI_CFG_PORT_IO_ADDR(b, d, f, aligned_offset);
    //DPrint("index: 0x%x\n", index);
    
    status = outIn32(Device, PCI_CONFIG_COMMAND_PORT, PCI_CONFIG_STATUS_PORT, index, &value);
    if ( status != 0 )
    {
        EPrint("getCfgValue 0x%x:0x%x => 0x%x failed! (0x%x)\n", PCI_CONFIG_COMMAND_PORT, index, PCI_CONFIG_STATUS_PORT, status);
    }

    //FLeave();
    return (value>>offset_shift);
}


NTSTATUS readCfg(
    _In_ HANDLE Device, 
    _In_ PBDF bdf,
    _In_ PPCI_IO pciIoParams,
    _Inout_ PVOID cfgBuffer,
    _In_ UINT32 cfgBufferSize
)
{
    FEnter();
    NTSTATUS status = 0;
    if ( pciIoParams->Flags.ReadMode == READ_CFG_MODE_MM )
    {
        UINT64 pa = PCI_CFG_MM_ADDR(pciIoParams->PciExBarAddress, bdf->Bus, bdf->Device, bdf->Function, 0);
        status = readPA(Device, pa, CACHE_TYPE, cfgBuffer, cfgBufferSize, READ_PA_MODE_DEF);
        if ( status != 0 )
        {
            EPrint("readPA config %02x:%02x:%02x failed! (0x%x)\n", bdf->Bus, bdf->Device, bdf->Function,  status);
            goto clean;
        }
    }
    else
    {
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
clean:
    FLeave();
    return status;
}

NTSTATUS addBdfData(
    _In_ PBDF_DATA BdfData, 
    _In_ uint8_t b, 
    _In_ uint8_t d, 
    _In_ uint8_t f,
    _In_ uint16_t vId,
    _In_ uint16_t dId
)
{
    //FEnter();
    NTSTATUS status = 0;

    if ( BdfData->Count >= BdfData->MaxCount )
    {
        size_t newSize = (size_t)BdfData->Size * 2;
        if ( newSize > (size_t)0xFFFFFFFF )
        {
            status = STATUS_BUFFER_OVERFLOW;
            goto clean;
        }
        PCFG_DATA newBlock = (PCFG_DATA)realloc(BdfData->List, newSize);
        if ( !newBlock )
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean;
        }
        BdfData->List = newBlock;
        BdfData->Size = (uint32_t)newSize;
        BdfData->MaxCount = (UINT32)(newSize / sizeof(CFG_DATA));
    }

    PCFG_DATA cfg = &BdfData->List[BdfData->Count];
    cfg->Bdf.Bus = b;
    cfg->Bdf.Device = d;
    cfg->Bdf.Function = f;
    cfg->VendorId = vId;
    cfg->DeviceId = dId;

    BdfData->Count++;
    
clean:
    //FLeave();
    return status;
}
