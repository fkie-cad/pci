#pragma once

//  4201: nonstandard extension used: nameless struct/union
#pragma warning( disable : 4201 )


#ifdef RING3
//#define RtlStringCchPrintfExA(_a_, _b_, _c_, _d_, _e_, _f_, ...) StringCchPrintfExA(_a_, _b_, _c_, _d_, _e_, _f_, ...)
//#define RtlStringCchPrintfExA StringCchPrintfExA
#endif


// For PCI or other memory-mapped resources
#define PCI_MEM_START (0xaeedbabe);

#ifndef PCI_CONFIG_COMMAND_PORT
#define PCI_CONFIG_COMMAND_PORT (0xCF8) // to 0xCFB
#endif

#ifndef PCI_CONFIG_STATUS_PORT
#define PCI_CONFIG_STATUS_PORT (0xCFC) // to 0xCFF
#endif

#ifndef PCI_CONFIG_SIZE
#define PCI_CONFIG_SIZE (0x100)
#endif

#ifndef PCI_INVALID_VENDORID
#define PCI_INVALID_VENDORID (0xFFFF)
#endif

#define PCI_VENDORID_AMD                  (0x1022)
#define PCI_VENDORID_INTEL                (0x8086)

#ifndef PCI_TYPE0_ADDRESSES
#define PCI_TYPE0_ADDRESSES (0x6)
#endif

#ifndef PCI_TYPE1_ADDRESSES
#define PCI_TYPE1_ADDRESSES (0x2)
#endif

// x86
#define IO_SPACE_LIMIT (0xffff)


#define _PRT_AS_UL (0x5452505f)

// for each bus, 
// we need 32 (device) * 8 (function) * 4KB of memory space, this is equal to 1MB; 
// for each device, we need 8 (function) * 4KB of memory space, this is equal to 32KB.
//- PCIEXBAR’s Bits 35:28
//- Bus 27:20 (8 bits)
//- Device 19:15 (5 bits)
//- Function 14:12 (3 bits)
//- Offset 11:0 (12 bits)
#define PCI_CFG_MM_ADDR(__xbar__, __b__, __d__, __f__, __o__) \
    /*__xbar__ + __b__ * 0x10'0000 + __d__ * 0x8000 + __f__ * 0x1000 + __o__*/ \
    (UINT64)(((UINT64)__xbar__) + (((UINT64)__b__) << 0x14) + (((UINT64)__d__) << 0xf ) + (((UINT64)__f__) << 0xc) + ((UINT64)__o__))



// Bit 31 (0x80000000) when set, all reads and writes to CONFIG_DATA are PCI Configuration transactions 
// Bits 30:24 are read-only and must return 0 when read
// Bits 23:16 select a specific Bus in the system (up to 256 buses)
// Bits 15:11 specify a Device on the given Bus (up to 32 devices)
// Bits 10:8 Specify the function of a device (up to 8 devices)
// Bits 7:0 Select an offset within the Configuration Space (256 bytes max, DWORD-aligned as bits 1:0 are hard-coded 0)

#define PCI_CFG_PORT_IO_ADDR(__b__, __d__, __f__, __o__) \
    (UINT32)( 0x80000000 | (((UINT32)(__b__)) << 0x10) | (((UINT32)(__d__)) << 0xb) | (((UINT32)(__f__)) << 8 | (__o__)) )


#define BAR_VALUE_VALID(__value__) \
    (__value__ && __value__ != 0xFFFFFFFF)


//isPciE ??
//check if PCI device config register 0x6 bit4 = 1(capability list exists status) ?
//check if PCI device config register 0x34 != 0(capability pointer valid) ?
//check if PCIe capability(ID = 0x10) exist ?

//#define IS_PCI_E(__cfg__) \
//    ((PPCI_CONFIG)__cfg__)->Status.CapabilitiesList && 
//    ((PPCI_CONFIG)__cfg__)->CapabilitiesPointer 
    

#pragma pack(1)

#ifndef _BDF_DEF
#define _BDF_DEF
typedef struct _BDF {
    UINT16 Bus;
    UINT16 Device;
    UINT16 Function;
} BDF, *PBDF;
#endif

#pragma pack()

#pragma pack(1)
typedef struct _PRT {
    ULONG Address; // DWORD The address of the device (uses the same format as _ADR).
    ULONG Pin; // Byte The PCI pin number of the device (0-INTA, 1-INTB, 2-INTC, 3-INTD).
    union {
        PCHAR NamePath; // Name of the device that allocates the interrupt to which the above pin is connected.
                        // The name can be a fully qualified path, a relative path, or a simple name segment thatutilizes the namespace search rules. 
                        // Note: This field is a NamePath and not a String literal, meaning that it should not be surrounded by quotes. 
        ULONG Byte;     // If this field is the integer constant Zero (or a Byte value of 0), then the interrupt is allocated from the global interrupt pool.

    } Source;
    ULONG SourceIndex;  // DWORD Index that indicates which resource descriptor in the resource template of the device pointed to in the Source field this interrupt is allocated from. 
                        // If the Source field is the Byte value zero, then this field is the global system interrupt number to which the pin is connected.
} PRT, *PPRT;
#pragma pack()


// Address and size of the BAR
// When you want to retrieve the actual base address of a BAR, 
// be sure to mask the lower bits. 
// For 16-bit Memory Space BARs, you calculate (BAR[x] & 0xFFF0). 
// For 32-bit Memory Space BARs, you calculate (BAR[x] & 0xFFFFFFF0). 
// For 64-bit Memory Space BARs, you calculate ((BAR[x] & 0xFFFFFFF0) + ((BAR[x + 1] & 0xFFFFFFFF) << 32)) 
// For I/O Space BARs, you calculate (BAR[x] & 0xFFFFFFFC).
// To determine the amount of address space needed by a PCI device, 
// - save the original value of the BAR, 
// - write a value of all 1's (0xFFFFFFFF) to the register, then read it back. 
//   The value won't be 0xFFFFFFFF, because not all bits are writeable.
// - mask the information bits, I/O: bit 0, Memory: bit 0,1,2,3
// - performing a bitwise NOT ('~' in C), 
// - and incrementing the value by 1. 
// The original value of the BAR should then be restored. 
// The BAR register is naturally aligned and as such you can only modify the bits that are set. 
// For example, if a device utilizes 16 MB it will have BAR0 filled with 0xFF000000 (0x1000000 after decoding) and you can only modify the upper 8-bits. [1]

#define BAR_TYPE_MEMORY_SPACE (0x0)
#define BAR_TYPE_IO_SPACE (0x1)

#define MEMORY_SPACE_BAR_32_BIT (0b00)
#define MEMORY_SPACE_BAR_64_BIT (0b10)

typedef struct _BAR {
    union {
        UINT32 Value;
        struct {
            UINT32 BarType:1; // 0: cpu memory, 1: io space
            UINT32 Reserved:31; // 
        };
        struct {
            UINT32 BarType:1; // 0 for cpu memory
            UINT32 Type:2; // 00: 32-bit, 10: 64-bit
            UINT32 Prefetchable:1;
            UINT32 BaseAddress:28; // 16-bit aligned
        } MemorySpace;
        struct {
            UINT32 BarType:1; // 1 for io space
            UINT32 Reserved1:1;
            UINT32 BaseAddress:14; // 4-bit aligned
            UINT32 Reserved2:16; 
        } IOSpace;
    };
} BAR, *PBAR;

typedef struct _MEMORY_SPACE_BAR {
    UINT32 Zero:1;
    UINT32 Type:2; // 00: 32-bit, 10: 64-bit
    UINT32 Prefetchable:1;
    UINT32 BaseAddress:28; // 16-bit aligned
} MEMORY_SPACE_BAR, *PMEMORY_SPACE_BAR;

typedef struct _IO_SPACE_BAR {
    UINT32 One:1;
    UINT32 Reserved:1;
    UINT32 BaseAddress:30; // 4-bit aligned
} IO_SPACE_BAR, *PIO_SPACE_BAR;

typedef struct _X_ROM_BAR {
    UINT32 Enable:1;
    UINT32 Reserved:10;
    UINT32 BaseAddress:21;
} X_ROM_BAR, *PX_ROM_BAR;

#define PCI_ROM_ADDRESS_MASK    (~0x7ffUL)


#define BAR_TYPE_STR(__b__) \
    (__b__->BarType==BAR_TYPE_MEMORY_SPACE)?"Memory":"IoSpace"
#define BAR_ADDRESS_WIDTH(__b__) \
    (__b__->BarType==BAR_TYPE_IO_SPACE)?"32-bit" \
    :(__b__->MemorySpace.Type==MEMORY_SPACE_BAR_32_BIT)?"32-bit":"64-bit"


//#include "classCodes.h"



#define PCI_COMMAND_IO (0x1) /* Enable response in I/O space */
#define PCI_COMMAND_MEMORY (0x2) /* Enable response in Memory space */
#define PCI_COMMAND_DECODE_ENABLE (PCI_COMMAND_MEMORY | PCI_COMMAND_IO)



// Header Type: Identifies the layout of the rest of the header beginning at byte 0x10 of the header. 
// If bit 7 of this register is set, the device has multiple functions; 
// otherwise, it is a single function device. Types:
#define PCI_HEADER_GENERAL_DEVICE (0x0) // a general device
#define PCI_HEADER_PCI_TO_PCI_BRIDGE (0x1) // a PCI-to-PCI bridge
#define PCI_HEADER_PCI_TO_CARDBUS_BRIDGE (0x2) // a PCI-to-CardBus bridge.

#define HEADER_TYPE_STR(_t_) \
    ( (_t_==PCI_HEADER_GENERAL_DEVICE)?"General" \
      : (_t_==PCI_HEADER_PCI_TO_PCI_BRIDGE)?"PCI-to-PCI bridge" \
      : (_t_==PCI_HEADER_PCI_TO_CARDBUS_BRIDGE)?"PCI-to-CardBus bridge" \
      :"Unknown" )

// https://resources.infosecinstitute.com/topic/pci-expansion-rom/
// PCI devices follow little ENDIAN ordering

// == wdm.h::PCI_COMMON_HEADER
#pragma pack(1)
typedef struct _PCI_CONFIG {
    UINT16 VendorId; // 0x0 0x2 Identifies the manufacturer of the device. Where valid IDs are allocated by PCI-SIG (the list is here) to ensure uniqueness and 0xFFFF is an invalid value that will be returned on read accesses to Configuration Space registers of non-existent devices.
    UINT16 DeviceId; // 0x2 0x2 Identifies the particular device. Where valid IDs are allocated by the vendor.
    union {
        UINT16 Value;
        struct {
            UINT16 IoSpace:1; // 0 If set to 1 the device can respond to I/O Space accesses; otherwise, the device's response is disabled
            UINT16 MemorySpace:1; // 4 If set to 1 the device can respond to Memory Space accesses; otherwise, the device's response is disabled.
            UINT16 BusMaster:1; // 2 If set to 1 the device can behave as a bus master; otherwise, the device can not generate PCI accesses.
            UINT16 SpecialCycles:1; // 3
            UINT16 MemoryWriteAndInvalidateEnable:1; // 4
            UINT16 VGAPaletteSnoop:1; // 5
            UINT16 ParityErrorResponse:1; // 6
            UINT16 WaitCycle:1; // 7
            UINT16 SERREnable:1; // 8
            UINT16 FastBackToBackEnable:1; // 9 If set to 1 indicates a device is allowed to generate fast back-to-back transactions; otherwise, fast back-to-back transactions are only allowed to the same agent.
            UINT16 InterruptDisable:1; // 10 If set to 1 the assertion of the devices INTx# signal is disabled; otherwise, assertion of the signal is enabled.
            UINT16 Reserved2:5; // 15:11 
        };
    } Command; // 0x4 0x2 Provides control over a device's ability to generate and respond to PCI cycles. Where the only functionality guaranteed to be supported by all devices is, when a 0 is written to this register, the device is disconnected from the PCI bus for all accesses except Configuration Space access.;
    union {
        UINT16 Value;
        struct {
            UINT16 ImmediateReadiness:1; // 1 This optional bit, when Set, indicates the Function is guaranteed to be ready to successfully complete valid Configuration Requests at any time
            UINT16 Reserved_2_1:2; // 2:1
            UINT16 InterruptStatus:1; // 3 When Set, indicates that an INTx emulation interrupt is pending internally in the Function
            UINT16 CapabilitiesList:1; // 4 Indicates the presence of an Extended Capability list item. Since all PCI Express device Functions are required to implement the PCI Express Capability structure, this bit must be hardwired to 1b.
            UINT16 MHzCapable66:1; // 5 This bit was originally described in the [PCI]. Its functionality does not apply to PCI Express and the bit must be hardwired to 0b.
            UINT16 UdfSupported:1; // 6
            UINT16 FastBackToBackCapable:1; // 7 This bit was originally described in the [PCI]. Its functionality does not apply to PCI Express and the bit must be hardwired to 0b.
            UINT16 MasterDataParityError:1; // 8 This bit is Set by a Function with a Type 0 Configuration Space Header if the Parity Error Response bit in the Command Register is 1b and either of the following two conditions occurs: • Function receives a Poisoned Completion • Function transmits a Poisoned Request This bit is Set by a Function with a Type 1 Configuration Space Header if the Parity Error Response bit in the Command Register is 1b and either of the following two conditions occurs:• Port receives a Poisoned Completion going Downstream • Port transmits a Poisoned Request Upstream If the Parity Error Response bit is 0b, this bit is never Set. Default value of this bit is 0b.
            UINT16 DEVSELTiming:2; // 10:9 This field was originally described in the [PCI]. Its functionality does not apply to PCI Express and the field must be hardwired to 00b
            UINT16 SignaledTargetAbort:1; // 11
            UINT16 ReceivedTargetAbort:1; // 12
            UINT16 ReceivedMasterAbort:1; // 13
            UINT16 SignaledSystemError:1; // 14
            UINT16 DetectedParityError:1; // 15
        };
    } Status; // 0x6 0x2 A register used to record status information for PCI bus related events.
    UINT8 RevisionID; // 0x8 0x1 Specifies a revision identifier for a particular device. Where valid IDs are allocated by the vendor.
    UINT8 ProgIF; // 0x9 0x1  A read-only register that specifies a register-level programming interface the device has, if it has any at all.
    UINT8 Subclass; // 0xa 0x1  A read-only register that specifies the specific function the device performs.
    UINT8 ClassCode; // 0xb 0x1  A read-only register that specifies the type of function the device performs.
    UINT8 CacheLineSize; // 0xc 0x1 Specifies the system cache line size in 32-bit units. A device can limit the number of cacheline sizes it can support, if a unsupported value is written to this field, the device will behave as if a value of 0 was written.
    UINT8 LatencyTimer; // 0xd 0x1 Specifies the latency timer in units of PCI bus clocks.
    union {
        UINT8 Value;
        struct {
            UINT8 Id:7; // 6:0
            UINT8 MultiFunction:1; // 7
        };
    } HeaderType; // 0xe 0x1 Identifies the layout of the rest of the header beginning at byte 0x10 of the header. If bit 7 of this register is set, the device has multiple functions; otherwise, it is a single function device
    union {
        UINT8 Value;
        struct {
            UINT8 CompletionCode:4; // 3:0
            UINT8 Reserved:2; // 5:4
            UINT8 StartBIST:1; // 6
            UINT8 BISTCapable:1; // 7
        };
    } BIST; // 0xf 0x1 Represents that status and allows control of a devices BIST (built-in self test).
} PCI_CONFIG, *PPCI_CONFIG; // 0x10
#pragma pack()

// type 0 (general) header
#pragma pack(1)
typedef struct _PCI_CONFIG_GENERAL {
    PCI_CONFIG Header; // common header
    UINT32 Bar[PCI_TYPE0_ADDRESSES]; // 0x10 - 0x24 Base address #0 - #5 (BAR0)
    UINT32 CardbusCISPointer; // 0x28 Points to the Card Information Structure and is used by devices that share silicon between CardBus and PCI.
    UINT16 SubsystemVendorID; // 0x2C     Subsystem ID             Subsystem Vendor ID
    UINT16 SubsystemID; // 0x2E     Subsystem ID             Subsystem Vendor ID
    X_ROM_BAR ERBA; // 0x30 Expansion ROM base address (ERBA). This register is used to define the address and sizeinformation for boot-time access to the optional FLASH memory. If no Flash memory exists, this register reports 00000000h.
    UINT8 CapabilitiesPointer; // 0x34 Points (i.e. an offset into this function's configuration space) to a linked list of new capabilities implemented by the device. Used if bit 4 of the status register (Capabilities List bit) is set to 1. The bottom two bits are reserved and should be masked before the Pointer is used to access the Configuration Space.
    UINT8 Reserved1[3]; // 0x35
    UINT32 Reserved2; // 0x38
    UINT8 InterruptLine; // 0x3C Specifies which input of the system interrupt controllers the device's interrupt pin is connected to and is implemented by any device that makes use of an interrupt pin. For the x86 architecture this register corresponds to the PIC IRQ numbers 0-15 (and not I/O APIC IRQ numbers) and a value of 0xFF defines no connection.
    UINT8 InterruptPIN; // 0x3D Specifies which interrupt pin the device uses. Where a value of 0x1 is INTA#, 0x2 is INTB#, 0x3 is INTC#, 0x4 is INTD#, and 0x0 means the device does not use an interrupt pin.
    UINT8 MinGrant; // 0x3E A read-only register that specifies the burst period length, in 1/4 microsecond units, that the device needs (assuming a 33 MHz clock rate).
    UINT8 MaxLatency; // 0x3F A read-only register that specifies how often the device needs access to the PCI bus (in 1/4 microsecond units).
} PCI_CONFIG_GENERAL, *PPCI_CONFIG_GENERAL; // 0x40
#pragma pack()

// type 1 (pci-to-pci bridge) header
#pragma pack(1)
typedef struct _PCI_CONFIG_PCI_BRIDGE {
    PCI_CONFIG Header; // common header
    UINT32 Bar[PCI_TYPE1_ADDRESSES]; // 0x10 Base address #0 (BAR0), 0x14 Base address #1 (BAR1)
    UINT8 PrimaryBusNumber; // 0x18
    UINT8 SecondaryBusNumber; // 0x19
    UINT8 SubordinateBusNumber; // 0x1a
    UINT8 SecondaryLatencyTimer; // 0x1b
    UINT8 IOBase; // 0x1C
    UINT8 IOLimit; // 0x1D
    UINT16 SecondaryStatus; // 0x1E
    UINT16 MemoryBase; // 0x20
    UINT16 MemoryLimit; // 0x22
    UINT16 PrefetchableMemoryBase; // 0x24
    UINT16 PrefetchableMemoryLimit; // 0x26
    UINT32 PrefetchableBaseUpper32Bits; // 0x28
    UINT32 PrefetchableLimitUpper32Bits; // 0x2C
    UINT16 IOBaseUpper16Bits; // 0x30
    UINT16 IOLimitUpper16Bits; // 0x32
    UINT8 CapabilitiesPointer; // 0x34 Points (i.e. an offset into this function's configuration space) to a linked list of new capabilities implemented by the device. Used if bit 4 of the status register (Capabilities List bit) is set to 1. The bottom two bits are reserved and should be masked before the Pointer is used to access the Configuration Space.
    UINT8 Reserved1[3]; // 0x35
    X_ROM_BAR ERBA; // 0x38 Expansion ROM base address. This register is used to define the address and sizeinformation for boot-time access to the optional FLASH memory. If no Flash memory exists, thisregister reports 00000000h.
    UINT8 InterruptLine; // 0x3C
    UINT8 InterruptPIN; // 0x3D
    UINT16 BridgeControl; // 0x3E
} PCI_CONFIG_PCI_BRIDGE, *PPCI_CONFIG_PCI_BRIDGE;
#pragma pack() // 0x40

// type 2 (pci-to-cardbus bridge) header
#pragma pack(1)
typedef struct _PCI_CONFIG_PCI_CB_BRIDGE {
    PCI_CONFIG Header; // common header
    union {
        UINT32 CardBusSocket; // 0x10
        UINT32 ExCaBaseAddress; // 0x10
    };
    UINT8 OffsetOfCapabilitiesList; // 0x14
    UINT8 Reserved; // 0x15
    UINT16 SecondaryStatus; // 0x16
    UINT8 PCIBBusNumber; // 0x18
    UINT8 CardBusBusNumber; // 0x19
    UINT8 SubordinateBusNumber; // 0x1a
    UINT8 CardBusLatencyTimer; // 0x1b
    UINT32 MemoryBaseAddress0; // 0x1C
    UINT32 MemoryLimit0; // 0x20
    UINT32 MemoryBaseAddress1; // 0x24
    UINT32 MemoryLimit1; // 0x28
    UINT32 IOBaseAddress0; // 0x2C
    UINT32 IOLimit0; // 0x30
    UINT32 IOBaseAddress1; // 0x34
    UINT32 IOLimit1; // 0x38
    
    UINT8 InterruptLine; // 0x3C
    UINT8 InterruptPIN; // 0x3D
    UINT16 BridgeControl; // 0x3E

    UINT16 SubsystemDeviceID; // 0x40
    UINT16 SubsystemVendorID; // 0x42

    UINT32 PCCardLegacyModeBaseAddress; // 0x44	16-bit 
} PCI_CONFIG_PCI_CB_BRIDGE, *PPCI_CONFIG_PCI_CB_BRIDGE;
#pragma pack() // 0x48





#define OPT_ROM_SIGNATURE_16 (0xAA55)
#define OPT_ROM_ALIGNMENT (0x800)

#pragma pack(1)
typedef struct _OPT_ROM {
    union {
        UINT8 Signature8[0x2];
        UINT16 Signature; // 00-01 55 AA
    }; // 00
    union {
        struct {
            UINT8 Length; // 02 Length in units of 0x200 bytes
            UINT8 InitEntry[3]; // 03-05 Entry point for INIT function. Power-On Self-Test (POST)does a FAR CALL to this location
            UINT8 Data[0x12]; // 06-17 00 00 Reserved for processor architecture-unique data
        } x86_64;
        struct {
            UINT16 InitializationSize; // 02 2 size of this image in units of 512 bytes. The sizeincludes this header
            UINT32 Signature; // 0x4 4 from EFI image header : 0x0EF1
            UINT16 SubsystemValue; // 08 2 for EFI image header
            UINT16 MachineType; // 0a 2 from EFI image header
            UINT8 Reserved[0xa]; // 0xc 10
            UINT16 OffsetIH; // 0x16 2 Offset to EFI Image header
        } Uefi;
        UINT8 RData[0x16]; // 0x02-0x17 00 00 Reserved for processor architecture-unique data
    }; // 02
    UINT16 PCIDataOffset; // 0x18-0x19 1c 00 Pointer to PCI data structure (assuming PCI data structure follows immediately after PCI expansion PROM header)
    UINT16 ExpansionHeaderOffset; // 1A-1B Pointer to a linked list of Option ROM expansion headers.    
                                  // Various Expansion Headers (regardless of their type) may be chained together and accessible via this pointer. 
                                  // The offset specified in this field is the offset from the start of the option ROM header
} OPT_ROM, *POPT_ROM; // 1C
#pragma pack()



typedef enum _PCI_DATA_CODE_TYPE {
    CODE_TYPE_INTEL, // 0 - Intel x86, PC-AT compatible
    CODE_TYPE_OFS, // 1 - Open Firmware standard for PCI
    CODE_TYPE_HP_RISC, // 2 - Hewlett-Packard PA RISC
    CODE_TYPE_EFI, // 3 - Extensible Firmware Interface (EFI)
    CODE_TYPE_Reserved // 4 - FF
} PCI_DATA_CODE_TYPE;


#pragma pack(1)
typedef struct _PCI_DATA {
    union {
        UINT8 Signature8[0x4]; 
        UINT32 Signature; // 00-03 Signature : P C I R (50 43 49 52)
    };
    UINT16 VendorId; // 04-05 Vendor ID
    UINT16 DeviceId; // 06-07 Device ID 
    UINT16 ProductData; // 08-09 Pointer to vital product data
    UINT16 Length; // 0A-0B PCI data structure length (18 00)
    UINT8 Revision; // 0C PCI data structure revision
    UINT8 ProgInterfaceCode; // 0D Programming interface code
    UINT8 SubclassCode; // 0E Subclass code
    UINT8 ClassCode; // 0F Class code
    UINT16 ImageLength; // 10-11 Image length in 512 bytes
    UINT16 RevisionLevel; // 12-13 Revision level of code/data
    UINT8 CodeType; // 14 Code type (01)
    union {
        UINT8 IndicatorByte; // 15 Indicator byte. For last image (0x80)
        struct {
            UINT8 IBReserved:7;
            UINT8 LastImage:1;
        };
    };
    UINT16 MaxRunTimeImageLength; // 16-17 The Image Length field is a two-byte field that represents the maximum length of the image after the initialization code has been executed. This field is in little-endian format, and the value is in units of 512 bytes. This field will be used to determine if the run-time image size is small enough to fit in the memory remaining in the system. This field is only present in Revision 3.0 and later of the PCI Data structure.
    UINT16 ConfigUtilityCodeHeader; // 18-19 This pointer is a two-byte pointer in little-endian format that points to the Expansion ROM's Configuration Utility Code Header table at the beginning of the configuration code block. The beginning reference point ("offset zero") for this pointer is the beginning of the Expansion ROM image. This field is only present in Revision 3.0 (and greater) PCI Data structures. A value of 0000 will be present in this field if the Expansion ROM does not support a Configuration Utility Code Header.
    UINT16 DMTF_CLP_EntryPoint; // 1A-1B This pointer is a two-byte pointer in little-endian format that points to the execution entry point for the DMTP CLP code supported by this ROM. The beginning reference point ("offset zero") for this pointer is the beginning of the Expansion ROM image. This field is only present in Revision 3.0 (and greater) PCI Data structures. A value of 0000 will be present in this field if the Expansion ROM does not support a DMTF CLP code entry point.
} PCI_DATA, *PPCI_DATA; // 1C
#pragma pack()




#define PMM_SIGNATURE_32 (0x4d4d5024) // `$PMM`
#pragma pack(1)
typedef struct _PMM_HEADER {
    UINT32 Signature;   // 0x000 4 : $PMM
    UINT8 StructureRevision; // 0x004 1
    UINT8 StructureLength; // 0x005 1 Length of this structure in bytes
    UINT8 StructureChecksum; // 0x006 1
    union {
        UINT32 Value;
        struct {
            UINT16 Offset;
            UINT16 Segment;
        };
    } EntryPoint;  // 0x007 4 Segment:offset of PMM Services entry point.
    UINT8 Reserved[5]; // 0x00b 5
} PMM_HEADER, *PPMM_HEADER; // 0C
#pragma pack()



#define PNP_SIGNATURE_32 (0x506e5024) // `$PnP`

#pragma pack(1)
typedef struct _PNP_HEADER {
    union {
        UINT8 Signature8[0x4]; 
        UINT32 Signature; // 0h 4 BYTES $PnP (ASCII) Signature Generic
    };
    UINT8 Revision; // 04h BYTE Varies Structure Revision 01h
    UINT8 Length; // 05h BYTE Varies Length (in 16 byte increments) Generic
    UINT16 Next; // 06h WORD Varies Offset of next Header (0000h if none) Generic
    UINT8 Reserved1; // 08h BYTE 00h Reserved Generic
    UINT8 Checksum; // 09h BYTE Varies Checksum Generic. Add up all byte values in the Expansion Header, including the Checksum field, into an 8-bit value. A resulting sum of zero indicates a valid checksum operation.
    UINT32 DeviceId; // 0Ah DWORD Varies Device Identifier PnP Specific
    UINT16 Manufacturer; // 0Eh WORD Varies Pointer to Manufacturer String (Optional) PnP Specific
    UINT16 ProductName; // 10h WORD Varies Pointer to Product Name String (Optional) PnP Specific
    UINT8 DeviceTypeCode[3]; // 12h 3 BYTE Varies Device Type Code PnP Specific, [0]: GeneralType, [1] SubType, [2] IFType
    union {
        UINT8 DeviceIndicators; // 15h BYTE Varies Device Indicators PnP Specific
        struct {
            UINT8 Display:1; // 0 A 1 in this position indicates that this device is a Display device.
            UINT8 Input:1; // 1 A 1 in this position indicates that this device is an Input device.
            UINT8 InitialProgramLoad:1; // 2 A 1 in this position indicates that this device is an Initial Program Load (IPL) device.
            UINT8 DIReserved:1; // 3 Reserved (0)
            UINT8 BootRequired:1; // 4 A 1 indicates that this option ROM is only required if this device is selected as a boot device.
            UINT8 ReadCacheable:1; // 5 A 1 indicates that this ROM is Read Cacheable
            UINT8 Shadowed:1; // 6 A 1 indicates that this ROM may be Shadowed in RAM
            UINT8 InitializationModel:1; // 7 A 1 indicates that this ROM supports the Device Driver Initialization Model
        };
    };
    UINT16 BootConnectionVector; // 16h WORD Varies Boot Connection Vector - Real/Protected mode (0000h if none) PnP Specific
    UINT16 DisconnectVector; // 18h WORD Varies Disconnect Vector - Real/Protected mode (0000h if none) PnP Specific
    UINT16 BootstrapEntryPoint; // 1Ah WORD Varies Bootstrap Entry Point - Real/Protected mode    (0000h if none) PnP Specific
    UINT16 Reserved2; // 1Ch WORD 0000h Reserved PnP Specific
    UINT16 ResourceInformationVector; // 1Eh WORD Varies Static Resource Information Vector- Real/Protected mode (0000h if none) PnP Specific
} PNP_HEADER, *PPNP_HEADER;
#pragma pack()
#pragma pack(1)
typedef struct _PNP_IS {
    UINT32 Signature; // 0h 4
    UINT8 Revision; // 04h 1
    UINT8 Length; // 05h 1
    UINT16 ControlFlags; // 06h 2
    UINT8 Checksum; // 08h 1
    UINT32 EventFlagAddress; // 09h 4
    UINT16 RealModeEntryOffset; // 0dh 2
    UINT16 RealModeEntrySegment; // 0fh 2
    UINT16 ProtectModeEntryOffset; // 11h 2
    UINT32 ProtectModeEntryBase; // 13h 4
    UINT32 OEMDeviceID; // 17h 4
    UINT16 RealModeDataSegment; // 1bh WORD Varies Boot Connection Vector - Real/Protected mode (0000h if none) PnP Specific
    UINT32 ProtectModeDataBase; // 1dh 2
} PNP_IS, *PPNP_IS; // 21h
#pragma pack()



#define VBT_HEADER_SIGNATURE_32 (0x54425624)

#pragma pack(1)
typedef struct _VBT_HEADER {
    UINT8 Signature[20]; // 00 "$VBT ....."
    UINT16 Version;  // 20
    UINT16 HeaderSize; // 22 Size of this structure
    UINT16 VbtSize; // 24 Size of VBT (VBT Header, BDB Header and data blocks)
    UINT8 Checksum; // 26 
    UINT8 Reserved0; // 27
    UINT32 BdbOffset; // 28 Offset of struct BDB_HEADER from beginning of VBT
    UINT32 AimOffset[4]; // 2c Offsets of add-in data blocks from beginning of VBT
} VBT_HEADER, *PVBT_HEADER; // 30
#pragma pack()


#define BDB_HEADER_SIGNATURE_1_64 (0x5441445f534f4942)
#define BDB_HEADER_SIGNATURE_2_64 (0x204b434f4c425f41)

#pragma pack(1)
typedef struct _BDB_HEADER {
    UINT8 Signature[0x10]; // 00 "BIOS_DATA_BLOCK "
    UINT16 Version; // 10
    UINT16 HeaderSize; // 12 Size of this structure
    UINT16 BdbSize; // 14 - Size of BDB (BDB Header and data blocks)
} BDB_HEADER, *PBDB_HEADER; // 16
#pragma pack()



FORCEINLINE
PVOID getBarBaseAddress(
    _In_ PBAR Bar,
    _In_opt_ PBAR NextBar
)
{
    UINT64 baseAddress = 0;
    if ( Bar->Value == 0 )
        return 0;

    if ( Bar->BarType == BAR_TYPE_MEMORY_SPACE )
    {
        if ( Bar->MemorySpace.Type == MEMORY_SPACE_BAR_32_BIT )
        {
            baseAddress = (((UINT64)Bar->MemorySpace.BaseAddress)<<0x4);
        }
        else if ( Bar->MemorySpace.Type == MEMORY_SPACE_BAR_64_BIT )
        {
            if ( !NextBar )
                return 0;

            //DPrint("act bar addr : 0x%x\n", Bar->MemorySpace.BaseAddress);
            //DPrint("nxt bar addr : 0x%x\n", NextBar->MemorySpace.BaseAddress);
            //DPrint("nxt bar value: 0x%x\n", NextBar->Value);
            baseAddress = (((UINT64)NextBar->Value)<<0x20) | (((UINT64)Bar->MemorySpace.BaseAddress)<<0x4);
        }
    }
    else if ( Bar->BarType == BAR_TYPE_IO_SPACE )
    {
        //baseAddress = ((UINT64)Bar->IOSpace.BaseAddress);
        baseAddress = ((UINT64)Bar->IOSpace.BaseAddress)<<2;
    }
    else
    {
        return 0;
    }

    return (PVOID)baseAddress;
}



FORCEINLINE
BOOLEAN isReadDenied(_In_ PVOID Buffer, _In_ ULONG BufferSize)
{
    ULONG i = 0;

    if ( BufferSize % 8 == 0 )
    {
        for ( i = 0; i < BufferSize; i+=0x8 )
        {
            if ( *(PUINT64)&((PUINT8)Buffer)[i] != (UINT64)-1 )
                goto clean;
        }
    }
    else if ( BufferSize % 4 == 0 )
    {
        for ( i = 0; i < BufferSize; i+=0x4 )
        {
            if ( *(PUINT32)&((PUINT8)Buffer)[i] != (UINT32)-1 )
                goto clean;
        }
    }
    else if ( BufferSize % 2 == 0 )
    {
        for ( i = 0; i < BufferSize; i+=0x2 )
        {
            if ( *(PUINT16)&((PUINT8)Buffer)[i] != (UINT16)-1 )
                goto clean;
        }
    }
    else 
    {
        for ( i = 0; i < BufferSize; i+=0x1 )
        {
            if ( *(PUINT8)&((PUINT8)Buffer)[i] != (UINT8)-1 )
                goto clean;
        }
    }
clean:
    return i >= BufferSize;
}

#define IS_64BIT_BAR(__b__) \
    ( ((PBAR)__b__)->BarType==BAR_TYPE_MEMORY_SPACE \
    && ((PBAR)__b__)->MemorySpace.Type==MEMORY_SPACE_BAR_64_BIT )

