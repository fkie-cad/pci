#pragma once


#pragma pack(1)

typedef struct _FULL_PCI_PMC {
    UINT16 Version:3;
    UINT16 PMEClock:1;
    UINT16 Rsvd1:1;
    UINT16 DeviceSpecificInitialization:1;
    UINT16 AuxCurrent:3;
    UINT16 D1:1;
    UINT16 D2:1;
    UINT16 PME_D0:1;
    UINT16 PME_D1:1;
    UINT16 PME_D2:1;
    UINT16 PME_D3_Hot:1;
    UINT16 PME_D3_Cold:1;
} FULL_PCI_PMC, *PFULL_PCI_PMC;

// power management
typedef struct _FULL_PCI_PM_CAPABILITY {

    PCI_CAPABILITIES_HEADER Header;

    //
    // Power Management Capabilities (Offset = 2)
    //

    union {
        FULL_PCI_PMC    Capabilities;
        USHORT          AsUSHORT;
    } PMC;

    //
    // Power Management Control/Status (Offset = 4)
    //

    union {
        PCI_PMCSR       ControlStatus;
        USHORT          AsUSHORT;
    } PMCSR;

    //
    // PMCSR PCI-PCI Bridge Support Extensions
    //

    union {
        PCI_PMCSR_BSE   BridgeSupport;
        UCHAR           AsUCHAR;
    } PMCSR_BSE;

    //
    // Optional read only 8 bit Data register.  Contents controlled by
    // DataSelect and DataScale in ControlStatus.
    //

    UCHAR   Data;

} FULL_PCI_PM_CAPABILITY, *PFULL_PCI_PM_CAPABILITY;


//
// MSX
typedef struct _PCI_CAPABILITY_MSI {
    PCI_CAPABILITIES_HEADER Header;
    
    // MMC - MSI Message Control Register
    union {
        struct {
            USHORT ME:1; // 0 MSI Enable (ME)—R/W. 0 = an MSI may not be generated. 1 = an MSI will be generated instead of an INTx signal
            USHORT MMC:3; // 3:1 Multiple Message Capable (MMC)—RO. the number of requested vectors as the power of two 
            USHORT MME:3; // 6:4  the number of allocated vectors (equal to or less than the number of requested vectors) as the power of 2
            USHORT ADD64:1; // 7 64b Address Capability (64ADD)—RO. 1 indicates the ability to generate a 64-bit  message address.
            USHORT PVM:1; // 8 Per-vector masking capable 
            USHORT EMDC:1; // 9 Extended Message Data Capable - If Set, the Function is capable of providing Extended Message Data. If Clear, the Function does not support providing Extended Message Data
            USHORT EMDE:1; // 10 Extended Message Data Enable - If Set, the Function is enabled to provide Extended Message Data. If Clear, the Function is not enabled to provide Extended Message Data.
            USHORT Res_15_11:5; // 15:11
        } bits;
        USHORT AsUSHORT;
    } MMC;
} PCI_CAPABILITY_MSI, *PPCI_CAPABILITY_MSI;

typedef union _PCI_CAPABILITY_MSI_MMLA {
    // MMLA - MSI Message Lower Address Register
    struct {
        ULONG Res_1_0:2; // 1:0 Reserved
        ULONG MLA:30; // 31:2 Message Lower Address (MLA)—R/W. Lower address used for MSI message
    } bits;
    ULONG AsULONG;
} PCI_CAPABILITY_MSI_MMLA, *PPCI_CAPABILITY_MSI_MMLA;

typedef union _PCI_CAPABILITY_MSI_MMUA {
    // MMUA - MSI Message Upper Address Register
    struct {
        ULONG MUA:32; // 32:0 Message Upper Address (MUA)—R/W. Upper 32-bits of address used for MSI message.
    } bits;
    ULONG AsULONG;
} PCI_CAPABILITY_MSI_MMUA, *PPCI_CAPABILITY_MSI_MMUA;

typedef union _PCI_CAPABILITY_MSI_MMD {
    // MMD—MSI Message Data Register 
    struct {
        ULONG MD:16; // 15:0 Message Data (MD)—R/W. If the MSI Enable bit is Set, the Function sends a DWORD Memory Write transaction using Message Data for the lower 16 bits. All 4 Byte Enables are Set.
        ULONG EMD:16; // 15:0 Message Data (MD)—R/W. This register is optional. For the MSI Capability structures without Per-vector Masking, it must be implemented if the Extended Message Data Capable bit is Set; otherwise, it is outside the MSI Capability structure and undefined.
    } bits;
    ULONG AsULONG;

} PCI_CAPABILITY_MSI_MMD, *PPCI_CAPABILITY_MSI_MMD;


//
// MSX-X
typedef struct _PCI_CAPABILITY_MSIX {
    PCI_CAPABILITIES_HEADER Header;
    
    union {
        struct {
            UINT16 TableSize:11; // 10:00 RO Table Size System software reads this field to determine the MSI-X Table Size N, which is encoded as N-1. For example, a returned value of “00000000011” indicates a table size of 4. 
            UINT16 Res_13_11:3; // 13:11 RO Reserved Always returns 0 on a read. 
            UINT16 FunctionMask:1; // 14 RW Function Mask If 1, all of the vectors associated with the function are masked, regardless of their per-vector Mask bit states. If 0, each vector’s Mask bit determines whether the vector is masked or not. Setting or clearing the MSI-X Function Mask bit has no effect on the state of the per-vector Mask bits. This bit’s state after reset is 0 (unmasked). 
            UINT16 MSXE:1; // 15 RW MSI-X Enable If 1 and the MSI Enable bit in the MSI Message Control register (see Section 6.8.1.3) is 0, the function is permitted to use MSI-X to request service and is prohibited from using its INTx# pin (if implemented; see Section 6.2.4). System configuration software sets this bit to enable MSI-X. A device driver is prohibited from writing this bit to mask a function’s service request. If 0, the function is prohibited from using MSI-X to request service. This bit’s state after reset is 0 (MSI-X is disabled). 
        } bits;
        UINT16 AsUINT16;
    } MMC;

    union {
        struct {
            UINT32 Table:3; // 2:0 RO Table BIR Indicates which one of a function’s Base Address registers (BAR), is used to map the function’s MSI-X Table into Memory Space. 
            UINT32 Offset:29; // 31:3 RO Table Offset Used as an offset from the address contained by one of the function’s Base Address registers (BAR) to point to the base of the MSI-X Table. The lower 3 Table BIR bits are masked off (set to zero) by software to form a 32-bit QWORD-aligned offset. 
        } bits;
        UINT32 AsUINT32;
    } TableOffsetBIR;

    union {
        struct {
            UINT32 PBA:3; // 2:0 RO PBA BIR Indicates which one of a function’s Base Address registers (BAR), is used to map the function’s MSI-X PBA into Memory Space. 
            UINT32 Offset:29; // 31:3 RO PBA Offset Used as an offset from the address contained by one of the function’s Base Address registers (BAR) to point to the base of the MSI-X PBA. The lower 3 PBA BIR bits are masked off (set to zero) by software to form a 32-bit QWORD-aligned offset. 
        } bits;
        UINT32 AsUINT32;
    } PBAOffsetBIR;

} PCI_CAPABILITY_MSIX, *PPCI_CAPABILITY_MSIX;

typedef struct _MSIX_TABLE_STRUCTURE {

    union {
        struct {
            UINT32 Zero:2; // 01:00 RW Message Address For proper DWORD alignment, software must always write zeroes to these two bits; otherwise the result is undefined. The state of these bits after reset must be 0. 
            UINT32 MA:30; // 31:02 RW Message Address System-specified message lower address. For MSI-X messages, the contents of this field from an MSI-X Table entry specifies the lower portion of the DWORD-aligned address (AD[31::02]) for the memory write transaction. 
        } bits;
        UINT32 AsUINT32;
    } MessageAddress; // 00

    union {
        struct {
            UINT32 MUA:32; // 31:00 RW Message Upper Address System-specified message upper address bits. If this field is zero, Single Address Cycle (SAC) messages are used. If this field is non-zero, Dual Address Cycle (DAC) messages are used. 
        } bits;
        UINT32 AsUINT32;
    } MessageUpperAddress; // 04

    union {
        struct {
            UINT32 MD:32; // 31:00 Message Data System-specified message data. For MSI-X messages, the contents of this field from an MSI-X Table entry specifies the data driven on AD[31::00] during the memory write transaction’s data phase. C/BE[3::0]# are asserted during the data phase of the memory write transaction. In contrast to message data used for MSI messages, the low-order message data bits in MSI-X messages are not modified by the function. 
        } bits;
        UINT32 AsUINT32;
    } MessageData; // 08

    union {
        struct {
            UINT32 Masked:1; // 00 RW Mask Bit When this bit is set, the function is prohibited from sending a message using this MSI-X Table entry. However, any other MSI-X Table entries programmed with the same vector will still be capable of sending an equivalent message unless they are also masked. This bit’s state after reset is 1 (entry is masked). 
            UINT32 Res_31_01:31; // 31::01 Reserved After reset, the state of these bits must be 0. However, for potential future use, software must preserve the value of these reserved bits when modifying the value of other Vector Control bits. If software modifies the value of these reserved bits, the result is undefined. 
        } bits;
        UINT32 AsUINT32;
    } VectorControl; // 0C

} MSIX_TABLE_STRUCTURE, *PMSIX_TABLE_STRUCTURE;  // 10

typedef struct _MSIX_PBA_STRUCTURE {
    UINT64 PendingBits; // Bit 0:63, 127:64, ...
} MSIX_PBA_STRUCTURE, *PMSIX_PBA_STRUCTURE; // 08

typedef struct _PCI_X_CAP {
    UINT16 CV:4; // 3:0 1h RO Capability Version (CV): Indicates version #1 PCI Express capability
    UINT16 DPT:4; // 7:4 9h RO Device/Port Type (DPT): Indicates that this is a Root Complex IntegratedEndpoint Device.
    UINT16 SI:1; // 8 0h RO Slot Implemented (SI): Hardwired to 0.
    UINT16 IMN:5; // 13:9 0h RO Interrupt Message Number (IMN): Hardwired to 0.
    UINT16 RES_15_14:2; // 15:14 0h RO Reserved.
} PCI_X_CAP, *PPCI_X_CAP;

#pragma pack()
