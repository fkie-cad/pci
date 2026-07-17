#pragma once

//#include "pci.h"

#define RCBA_SPI_OFFSET 0x3800 // from the specs
#define RCBA_SPI_SIZE 0x200 // from the specs : 39ff - 3800 + 1

#define RCBA_BA_MASK (0xFFFFC000)


//  4201: nonstandard extension used: nameless struct/union
#pragma warning( disable : 4201 )



#pragma pack(1)

// BDF 00:00:00
typedef struct _PAM {
    UINT8 LOENABLE:2; // 1:0 0h RW_L LOENABLE: This field controls the steering of read and write cycles that address the BIOS area from 0C0h to 0C3FFFh.
            //00: DRAM Disabled. All reads are sent to DRAM. All writes are forwarded to DMI.
            //01: Read Only. All reads are sent to DRAM. All writes are forwarded to DMI.
            //10: Write Only. All writes are sent to DRAM. All reads are serviced by DMI.
            //11: Normal DRAM Operation. All reads and writes are serviced by DRAM. 
    UINT8 Reserved1:2; // 3:2 0h RO Reserved (RSVD): Reserved. 
    UINT8 HIENABLE:2; // 5:4 0h RW_L HIENABLE: This field controls the steering of read and write cycles that address the BIOS area from 0C_4000h to 0C_7FFFh.
            //00: DRAM Disabled. All accesses are directed to DMI.
            //01: Read Only. All reads are sent to DRAM, all writes are forwarded to DMI.
            //10: Write Only. All writes are sent to DRAM, all reads are serviced by DMI.
            //11: Normal DRAM Operation. All reads and writes are serviced by DRAM.
    UINT8 Reserved2:2; // 7:6 0h RO Reserved (RSVD): Reserved. 
} PAM, *PPAM;

typedef struct _PCI_EX_BAR {
    UINT64 PCIEXBAREN:1; // 0 0h RW PCIEXBAREN: 
                //0: The PCIEXBAR register is disabled. Memory read and write transactions proceed s if there were no PCIEXBAR register. PCIEXBAR bits 38:26 are R/W with no functionality behind them.
                //1: The PCIEXBAR register is enabled. Memory read and write transactions whose address bits 38:26 match PCIEXBAR will be translated to configuration reads and writes within the Uncore. These Translated cycles are routed as shown in the above table.
    UINT64 LENGTH:3; // 3:1 0h RW LENGTH: This field describes the length of this region.
                //00: 256MB (buses 0-255). Bits 38:28 are decoded in the PCI Express Base Address Field. 
                //01: 128MB (buses 0-127). Bits 38:27 are decoded in the PCI Express Base Address Field. 
                //10: 64MB (buses 0-63). Bits 38:26 are decoded in the PCI Express Base Address Field.
                //11: Reserved.
                //This register is locked by Intel TXT. 
    UINT64 Reserved1:22; // 25:4 0h RO Reserved (RSVD): Reserved. 
    UINT64 ADMSK64:1; // 26 0h RW_V ADMSK64: This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register. 
    UINT64 ADMSK128:1; // 27 0h RW_V ADMSK128: This bit is either part of the PCI Express Base Address (R/W) or part of the Address Mask (RO, read 0b), depending on the value of bits [2:1] in this register. 
    UINT64 PCIEXBAR:11; // 38:28 0h RW PCIEXBAR: This field corresponds to bits 38 to 28 of the base address for PCI Express enhanced configuration space. 
                        // BIOS will program this register resulting in a base address for a contiguous memory address space. 
                        // The size of the range is defined by bits [2:1] of this register.
                        // This Base address shall be assigned on a boundary consistent with the number of buses (defined by the Length field in this register) above TOLUD and still within the 39-bit addressable memory space. 
                        // The address bits decoded depend on the length of the region defined by this register.
                        // This register is locked by Intel TXT.
                        // The address used to access the PCI Express configuration space for a specific device can be determined as follows: 
                        //   PCI Express Base Address + Bus Number * 1MB + Device Number * 32KB + Function Number * 4KB. 
                        // This address is the beginning of the 4KB space that contains both the PCI compatible configuration space and the PCI Express extended configuration space. 
    UINT64 RSVD:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
} PCI_EX_BAR, *PPCI_EX_BAR; // 60h PCI Express Register Range Base Address (PCIEXBAR). This is the base address for the PCI Express configuration space. This window of addresses contains the 4KB of configuration space for each PCI Express device that can potentially be part of the PCI Express Hierarchy associated with the Uncore. There is no actual physical memory within this window of up to 256MB that can be addressed. The actual size of this range is determined by a field in this register 

typedef struct _HOST_BRIDGE_DRAM_CFG {
    PCI_CONFIG_GENERAL Header;
    struct {
        UINT64 PXPEPBAREN:1; // 0 0h RW PXPEPBAREN: 0: PXPEPBAR is disabled and does not claim any memory1: PXPEPBAR memory mapped accesses are claimed and decoded appropriately. 
        UINT64 Reserved1:11; // 11:1 0h RO Reserved (RSVD): Reserved. 
        UINT64 PXPEPBAR:27; // 38:12 0h RW PXPEPBAR: This field corresponds to bits 38 to 12 of the base address PCI Express Egress Port MMIO configuration space. BIOS will program this register resulting in a base address for a 4KB block of contiguous memory address space. This register ensures that a naturally aligned 4KB space is allocated within the first 512GB of addressable memory space. System Software uses this base address to program the PCI Express Egress Port MMIO register set. 
        UINT64 Reserved2:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
    } PXPEPBAR; // 40h PCI Express* Egress Port Base Address  This register is locked by Intel TXT. 
    struct {
        UINT64 MCHBAREN:1; // 0 0h RW MCHBAREN: 0: MCHBAR is disabled and does not claim any memory 1: MCHBAR memory mapped accesses are claimed and decoded appropriately
        UINT64 Reserved1:14; // 14:1 0h RO Reserved (RSVD): Reserved. 
        UINT64 MCHBAR:24; // 38:15 0h RW MCHBAR: This field corresponds to bits 38 to 15 of the base address Host Memory Mapped configuration space. BIOS will program this register resulting in a base address for a 32KB block of contiguous memory address space. This register ensures that a naturally aligned 32KB space is allocated within the first 512GB of addressable memory space. System Software uses this base address to program the Host Memory Mapped register set. 
        UINT64 Reserved2:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
    } MCHBAR; // 48h Host Memory Mapped Register Range Base
    struct {
        UINT32 GGCLCK:1; // 0 0h RW_KL GGCLCK: When set to 1b, this bit will lock all bits in this register
        UINT32 IVD:1; // 1 0h RW_L IVD: 0: Enable. Device 2 (Processor Graphics) claims VGA memory and IO cycles, the SubClass Code within Device 2 Class Code register is 00.1: Disable. Device 2 (Processor Graphics) does not claim VGA cycles (Mem and IO), and the Sub- Class Code field within Device 2 function 0 Class Code register is 80.BIOS Requirement: BIOS should not set this bit to 0 if the GMS field (bits 7:3 of this register) pre-allocates no memory.
        UINT32 VAMEN:1; // 2 0h RW_L VAMEN: Enables the use of the iGFX engines for Versatile Acceleration.1: iGFX engines are in Versatile Acceleration Mode. Device 2 Class Code is 048000h.0:- iGFX engines are in iGFX Mode. Device 2 Class Code is 030h. 
        UINT32 Reserved1:3; // 5:3 0h RO Reserved (RSVD): Reserved. 
        UINT32 GGMS:2; // 7:6 0h RW_L GGMS: This field is used to select the amount of Main Memory that is pre-allocated to support the Processor Graphics Translation Table. The BIOS ensures that memory is pre-allocated only when Processor Graphics is enabled.GSM is assumed to be a contiguous physical DRAM space with DSM, and BIOS needs to allocate a contiguous memory chunk. Hardware will derive the base of GSM from DSM only using the GSM size programmed in the register.Hardware functionality in case of programming this value to Reserved is not guaranteed. 
        UINT32 GMS:8; // 15:8 5h RW_L GMS: This field is used to select the amount of Main Memory that is pre-allocated to support the Processor Graphics device in VGA (non-linear) and Native (linear) modes. The BIOS ensures that memory is pre-allocated only when Processor Graphics is enabled.This register is also Intel TXT lockable.Hardware does not clear or set any of these bits automatically based on Processor Graphics being disabled/enabled.BIOS Requirement: BIOS should not set this field to 0h if IVD (bit 1 of this register) is 0. 
    } GMCH; // 50h Graphics Control Register (GGC)
    struct {
        UINT32 D0EN:1; // 0 1h RO D0EN: Bus 0 Device 0 Function 0 may not be disabled and is therefore hardwired to 1.
        UINT32 D1F2EN:1; // 1 1h RW_L D1F2EN: 0: Bus 0 Device 1 Function 2 is disabled and hidden. 1: Bus 0 Device 1 Function 2 is enabled and visible.
        UINT32 D1F1EN:1; // 2 1h RW_L D1F1EN: 0: Bus 0 Device 1 Function 1 is disabled and hidden.1: Bus 0 Device 1 Function 1 is enabled and visible.
        UINT32 D1F0EN:1; // 3 1h RW_L D1F0EN: 0: Bus 0 Device 1 Function 0 is disabled and hidden.1: Bus 0 Device 1 Function 0 is enabled and visible.This bit will be set to 0b and remain 0b if PEG10 capability is disabled. 
        UINT32 D2EN:1; // 4 1h RW_L D2EN: 0: Bus 0 Device 2 is disabled and hidden1: Bus 0 Device 2 is enabled and visibleThis bit will be set to 0b and remain 0b if Device 2 capability is disabled. 
        UINT32 D3EN:1; // 5 1h RW_L D3EN: 0: Bus 0 Device 3 is disabled and hidden 1: Bus 0 Device 3 is enabled and visibleThis bit will be set to 0b and remain 0b if Device 3 capability is disabled. 
        UINT32 Reserved1:1; // 6 0h RO Reserved (RSVD): Reserved. 
        UINT32 D4EN:1; // 7 1h RW_L D4EN: 0: Bus 0 Device 4 is disabled and not visible.1: Bus 0 Device 4 is enabled and visible.This bit will be set to 0b and remain 0b if Device 4 capability is disabled. 
        UINT32 Reserved2:2; // 9:8 0h RO Reserved (RSVD): Reserved. 
        UINT32 D5EN:1; // 10 1h RW_L D5EN: 0: Bus 0 Device 5 is disabled and not visible.1: Bus 0 Device 5 is enabled and visible.This bit will be set to 0b and remain 0b if Device 5 capability is disabled. 
        UINT32 Reserved3:2; // 12:11 0h RO Reserved (RSVD): Reserved. 
        UINT32 D6EN:1; // 13 0h RW D6EN: Reserved (RSVD) 
        UINT32 D7EN:1; // 14 0h RW D7EN: 0: Bus 0 Device 7 is disabled and not visible.1: Bus 0 Device 7 is enabled and visible.Non-production BIOS code should provide a setup option to enable Bus 0 Device 7. When enabled, Bus 0 Device 7 should be initialized in accordance to standard PCI device initialization procedures. 
        UINT32 D8EN:1; // 15 1h RW_L D8EN: 0: Bus 0 Device 8 is disabled and not visible.1: Bus 0 Device 8 is enabled and visible.This bit will be set to 0b and remain 0b if Device 8 capability is disabled. 
        UINT32 Reserved4:16; // 31:16 0h RO Reserved (RSVD): Reserved. 
    } DEVEN; // 54h Device Enable 
    struct {
        UINT32 PCME:1; // 0 0h RW_L PCME: This field enables Protected Content Memory within Graphics Stolen Memory. This memory is the same as the WOPCM area, whose size is defined by bit 5 of this register. This register is locked when PAVPLOCK is set. A value of 0 in this field indicates that Protected Content Memory is disabled, and cannot be programmed in this manner when PAVP is enabled. A value of 1 in this field indicates that Protected Content Memory is enabled, and is the only programming option available when PAVP is enabled. (Note that the processor legacy Lite mode programming of PCME bit = 0 is not supported. For non-PAVP3 Mode, even for Lite mode configuration, this bit should be programmed to 1 and HVYMODESEL = 0). This bit should always be programmed to 1 if bits 1 and 2 (PAVPE and PAVP lock bits) are both set. With per-App Memory configuration support, the range check for the WOPCM memory area should always happen when this bit is set, regardless of Lite or Serpent mode, or PAVP2 or PAVP3 mode programming
        UINT32 PAVPE:1; // 1 0h RW_L PAVPE: 0: PAVP functionality is disabled.1: PAVP functionality is enabled.
        UINT32 PAVPLCK:1; // 2 0h RW_KL PAVPLCK: This bit locks all writeable contents in this register when set (including itself). Only a hardware reset can unlock the register again. This lock bit needs to be set only if PAVP is enabled (bit 1 of this register is asserted).
        UINT32 HVYMODSEL:1; // 3 0h RW_L HVYMODSEL: This bit is applicable only for PAVP2 operation mode. This bit is also applicable for PAVP3 mode only if the per-App memory config is disabled due to the clearing of bit 9 in the Crypto Function Control_1 register (address 0x320F0).0: Lite Mode (Non-Serpent mode)1: Serpent ModeFor enabled PAVP3 mode, this one type boot time programming has been replaced by per-App programming (through the Media Crypto Copy command). Note that PAVP2 or PAVP3 mode selection is done by programming bit 8 of the MFX_MODE - Video Mode register. 
        UINT32 OVTATTACK:1; // 4 0h RW_L OVTATTACK: Override of Unsolicited Connection State Attack and Terminate.0: Disable Override. Attack Terminate allowed.1: Enable Override. Attack Terminate disallowed.This register bit is locked when PAVPE is set.
        UINT32 RSVD1:1; // 5 0h RW_L RSVD1: These bits are reserved for future use.
        UINT32 ASMFEN:1; // 6 0h RW_L ASMFEN: ASMF method enabled0b Disabled (default). 1b Enabled.This register is locked when PAVPLCK is set. 
        UINT32 RSVD2:13; // 19:7 0h RW_L RSVD2: These bits are reserved for future use. 
        UINT32 PCMBASE:12; // 31:20 0h RW_L PCMBASE: Sizes supported in the processor: 1M, 2M, 4M and 8M. Base value programmed (from Top of StolenMemory) itself defines the size of the WOPCM. Separate WOPCM size programming is redundant information and not required. Default 1M size programming. 4M recommended for the processor. This register is locked (becomes read-only) when PAVPE = 1b.
    } PAVPC; // 58h Protected Audio Video Path Control
    struct {
        UINT32 LOCK:1; // 0 0h RW_KL LOCK: All bits which may be updated by SW in this register are locked down when this bit is set.
        UINT32 PRS:1; // 1 0h ROV PRS: This field indicates the status of DPR.
                    //0: DPR protection disabled
                    //1: DPR protection enabled
        UINT32 EPM:1; // 2 0h RW_L EPM: This field controls DMA accesses to the DMA Protected Range (DPR) region.
                    //0: DPR is disabled
                    //1: DPR is enabled. All DMA requests accessing DPR region are blocked.
                    //HW reports the status */of DPR enable/disable through the PRS field in this register.
        UINT32 Reserved1:1; // 3 0h RO Reserved (RSVD): Reserved. 
        UINT32 DPRSIZE:8; // 11:4 0h RW_L DPRSIZE: This is the size of memory, in MB, that will be protected from DMA accesses. A value of 0x00 in this field means no additional memory is protected. The maximum amount of memory that will be protected is 255 MB. The amount of memory reported in this field will be protected from all DMA accesses, including translated Processor accesses and graphics. The top of the protected range is the BASE of TSEG -1. Note: If TSEG is not enabled, then the top of this range becomes the base of stolen graphics, or ME stolen space or TOLUD, whichever would have been the location of TSEG, assuming it had been enabled. The DPR range works independently of any other range, including the NoDMA.TABLE protection or the PMRC checks in VTd, and is done post any VTd translation or Intel TXT NoDMA lookup. Therefore incoming cycles are checked against this range after the VTd translation and faulted if they hit this protected range, even if they passed the VTd translation or were clean in the NoDMA lookup. All the memory checks are OR'ed with respect to NOT being allowed to go to memory. So if either PMRC, DPR, NoDMA table lookup, NoDMA.TABLE.PROTECT OR a VTd translation disallows the cycle, then the cycle is not allowed to go to memory. Or in other words, all the above checks should pass before a cycle is allowed to DRAM.
        UINT32 Reserved2:8; // 19:12 0h RO Reserved (RSVD): Reserved. 
        UINT32 PAVPC:12; // 31:20 0h ROV TopOfDPR: Top address + 1 of DPR. This is the base of TSEG. Bits 19:0 of the BASE reported here are 0x0_0000. 
    } DPR; // 5Ch DMA Protected Range (DPR)
    PCI_EX_BAR PCIEXBAR; // 60h PCI Express Register Range Base Address (PCIEXBAR). This is the base address for the PCI Express configuration space. This window of addresses contains the 4KB of configuration space for each PCI Express device that can potentially be part of the PCI Express Hierarchy associated with the Uncore. There is no actual physical memory within this window of up to 256MB that can be addressed. The actual size of this range is determined by a field in this register 
    struct {
        UINT64 DMIBAREN:1; // 0 0h RW DMIBAREN: 
                // 0: DMIBAR is disabled and does not claim any memory
                // 1: DMIBAR memory mapped accesses are claimed and decoded appropriately
                // This register is locked by Intel TXT
        UINT64 Reserved1:11; // 11:1 0h RO Reserved (RSVD): Reserved. 
        UINT64 DMIBAR:27; // 38:12 0h RW DMIBAR: This field corresponds to bits 38 to 12 of the base address DMI configuration space. BIOS will program this register resulting in a base address for a 4KB block of contiguous memory address space. This register ensures that a naturally aligned 4KB space is allocated within the first 512GB of addressable memory space. System Software uses this base address to program the DMI register set. All the Bits in this register are locked in Intel TXT mode. 
        UINT64 Reserved2:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
    } DMIBAR; // 68h Root Complex Register Range Base Address (DMIBAR). This is the base address for the Root Complex configuration space. This window of addresses contains the Root Complex Register set for the PCI Express Hierarchy associated with the Host Bridge. There is no physical memory within this 4KB window that can be addressed. The 4KB reserved by this register does not alias to any PCI 2.3 compliant memory mapped space. On reset, the Root Complex configuration space is disabled and should be enabled by writing a 1 to DMIBAREN [Dev 0, offset 68h, bit 0]. All the bits in this register are locked in Intel TXT mode.
    struct {
        UINT64 Reserved1:20; // 19:0 0h RO Reserved (RSVD): Reserved. 
        UINT64 MEBASE:19; // 38:20 7FFFFh RW_L MEBASE: Corresponds to A[38:20] of the base address memory range that is allocated to the ME. 
        UINT64 Reserved2:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
    } MESEG_BASE; // 70h Manageability Engine Base Address Register (MESEG) This register determines the Base Address register of the memory range that is preallocated to the Manageability Engine. Together with the MESEG_MASK register it controls the amount of memory allocated to the ME
    struct {
        UINT64 Reserved1:10; // 9:0 0h RO Reserved (RSVD): Reserved. 
        UINT64 MELCK:1; // 10 0h RW_KL MELCK: This field indicates whether all bits in the MESEG_BASE and MESEG_MASK registers are locked. When locked, updates to any field for these registers should be dropped. 
        UINT64 ME_STLEN_EN:1; // 11 0h RW_L ME_STLEN_EN: Indicates whether the ME stolen Memory range is enabled or not. 
        UINT64 Reserved2:8; // 19:12 0h RO Reserved (RSVD): Reserved. 
        UINT64 MEMASK:19; // 38:20 0h RW_L MEMASK: This field indicates the bits that should match MEBASE in order to qualify as an ME Memory Range access.For example, if the field is set to 7FFFFh, then ME Memory is 1MB in size.Another example is that if the field is set to 7FFFEh, then ME Memory is 2MB in size.Mask value should be such that once a bit is set to 1 all the more significant bit should be 1. It is not legal to set up mask with 0 and 1's interspersed. In other words, the size of ME Memory Range is limited to power of 2 times 1MB. MEBASE should be naturally aligned to the size of ME region. 
        UINT64 Reserved3:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
    } MESEG_MASK; // 78h Manageability Engine Limit Address Register (MESEG) This register determines the Mask Address register of the memory range that is preallocated to the Manageability Engine. Together with the MESEG_BASE register it controls the amount of memory allocated to the ME
    struct {
        UINT8 Lock:1; // 0 0h RW_KL Lock: If this bit is set, all of the PAM* registers are locked (cannot be written)
        UINT8 Reserved1:3; // 3:1 0h RO Reserved (RSVD): Reserved. 
        UINT8 HIENABLE:2; // 5:4 0h RW_L HIENABLE: This field controls the steering of read and write cycles that address the BIOS area from 0F_0h to 0F_FFFFh.
                    // 00: DRAM Disabled. All accesses are directed to DMI.
                    // 01: Read Only. All reads are sent to DRAM, all writes are forwarded to DMI.
                    // 10: Write Only. All writes are sent to DRAM, all reads are serviced by DMI.
                    // 11: Normal DRAM Operation. All reads and writes are serviced by DRAM. 
        UINT8 Reserved2:2; // 7:6 0h RO Reserved (RSVD): Reserved. 
    } PAM0; // 80h Programmable Attribute Map 0 (PAM0)  This register controls the read, write and shadowing attributes of the BIOS range from F_0h to F_FFFFh. The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range. Seven Programmable Attribute Map (PAM) registers are used to support these features. Cacheability of these areas is controlled via the MTRR register in the core.
    PAM PAM1; // 81h Programmable Attribute Map 1 (PAM1) This register controls the read, write and shadowing attributes of the BIOS range from C_0h to C_7FFFh. The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range. Seven Programmable Attribute Map (PAM) registers are used to support these features. Cacheability of these areas is controlled via the MTRR register in the core.
    PAM PAM2; // 82h Programmable Attribute Map 2 (PAM2) This register controls the read, write and shadowing attributes of the BIOS range from C_8000h to C_FFFFh. The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range. Seven Programmable Attribute Map (PAM) registers are used to support these features. Cacheability of these areas is controlled via the MTRR register in the core
    PAM PAM3; // 83h Programmable Attribute Map 3 (PAM3) This register controls the read, write and shadowing attributes of the BIOS range from D0h to D7FFFh. The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range. Seven Programmable Attribute Map (PAM) registers are used to support these features. Cacheability of these areas is controlled via the MTRR register in the core.
    PAM PAM4; // 84h Programmable Attribute Map 4 (PAM4) This register controls the read, write and shadowing attributes of the BIOS range from D8000h to DFFFFh. The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range. Seven Programmable Attribute Map (PAM) registers are used to support these features. Cacheability of these areas is controlled via the MTRR register in the core.
    PAM PAM5; // 85h Programmable Attribute Map 5 (PAM5) This register controls the read, write and shadowing attributes of the BIOS range from E_0h to E_7FFFh. The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range. Seven Programmable Attribute Map (PAM) registers are used to support these features. Cacheability of these areas is controlled via the MTRR register in the core.
    PAM PAM6; // 86h Programmable Attribute Map 6 (PAM6) This register controls the read, write and shadowing attributes of the BIOS range from E_8000h to E_FFFFh. The Uncore allows programmable memory attributes on 13 legacy memory segments of various sizes in the 768KB to 1MB address range. Seven Programmable Attribute Map (PAM) registers are used to support these features. Cacheability of these areas is controlled via the MTRR register in the core.
    struct {
        UINT8 MDAP10:1; // 0 0h RW MDAP10: This bit works with the VGA Enable bits in the BCTRL register of Device 1 Function 0 to control the routing of Processor initiated transactions targeting MDA compatible I/O and memory address ranges. This bit should not be set if device 1 function 0 VGA Enable bit is not set.If device 1 function 0 VGA enable bit is not set, then accesses to IO address range x3BCh-x3BFh remain on the backbone.If the VGA enable bit is set and MDA is not present, then accesses to IO address range x3BCh-x3BFh are forwarded to PCI Express through device 1 function 0 if the address is within the corresponding IOBASE and IOLIMIT, otherwise they remain on the backbone.
                //MDA resources are defined as the following:
                //Memory: 0B0h - 0B7FFFh
                //I/O: 3B4h, 3B5h, 3B8h, 3B9h, 3BAh, 3BFh, (including ISA address aliases, A[15:10] are not used in decode) Any I/O reference that includes the I/O locations listed above, or their aliases, will remain on the backbone even if the reference also includes I/O locations not listed above. The following table shows the behavior for all combinations of MDA and VGA:
                // VGAEN MDAP Description
                // 0 0 All References to MDA and VGA space are not claimed by Device 1 Function 0.
                // 0 1 Illegal combination
                // 1 0 All VGA and MDA references are routed to PCI Express Graphics Attach device 1 function 0.
                // 1 1 All VGA references are routed to PCI Express Graphics Attach device 1 function 0. MDA references are not claimed by device 1 function 0.VGA and MDA memory cycles can only be routed across PEG10 when MAE (PCICMD10[1]) is set. VGA and MDA I/O cycles can only be routed across PEG10 if IOAE (PCICMD10[0]) is set. 
        UINT8 MDAP11:1; // 1 0h RW MDAP11: This bit works with the VGA Enable bits in the BCTRL register of Device 1 Function 1 to control the routing of Processor initiated transactions targeting MDA compatible I/O and memory address ranges. This bit should not be set if device 1 function 1 VGA Enable bit is not set.If device 1 function 1 VGA enable bit is not set, then accesses to IO address range x3BCh-x3BFh remain on the backbone.If the VGA enable bit is set and MDA is not present, then accesses to IO address range x3BCh-x3BFh are forwarded to PCI Express through device 1 function 1 if the address is within the corresponding IOBASE and IOLIMIT, otherwise they remain on the backbone.
                //MDA resources are defined as the following:
                // Memory: 0B0h - 0B7FFFh
                // I/O: 3B4h, 3B5h, 3B8h, 3B9h, 3BAh, 3BFh,
                // (including ISA address aliases, A[15:10] are not used in decode) Any I/O reference that includes the I/O locations listed above, or their aliases, will remain on the backbone even if the reference also includes I/O locations not listed above.
                //The following table shows the behavior for all combinations of MDA and VGA: 
                // VGAEN MDAP Description
                // 0 0 All References to MDA and VGA space are not claimed by Device 1 Function 1.
                // 0 1 Illegal combination
                // 1 0 All VGA and MDA references are routed to PCI Express Graphics Attach device 1 function 1.
                // 1 1 All VGA references are routed to PCI Express Graphics Attach device 1 function 1. MDA references are not claimed by device 1 function 1.VGA and MDA memory cycles can only be routed across PEG11 when MAE (PCICMD11[1]) is set. VGA and MDA I/O cycles can only be routed across PEG11 if IOAE (PCICMD11[0]) is set.
        UINT8 MDAP12:1; // 2 0h RW MDAP12: This bit works with the VGA Enable bits in the BCTRL register of Device 1 Function 2 to control the routing of Processor initiated transactions targeting MDA compatible I/O and memory address ranges. This bit should not be set if device 1 function 2 VGA Enable bit is not set.If device 1 function 2 VGA enable bit is not set, then accesses to IO address range x3BCh-x3BFh remain on the backbone.If the VGA enable bit is set and MDA is not present, then accesses to IO address range x3BCh-x3BFh are forwarded to PCI Express through device 1 function 2 if the address is within the corresponding IOBASE and IOLIMIT, otherwise they remain on the backbone.
                //MDA resources are defined as the following:
                // Memory: 0B0h - 0B7FFFh
                // I/O: 3B4h, 3B5h, 3B8h, 3B9h, 3BAh, 3BFh, (including ISA address aliases, A[15:10] are not used in decode)
                //Any I/O reference that includes the I/O locations listed above, or their aliases, will remain on the backbone even if the reference also includes I/O locations not listed above.
                //The following table shows the behavior for all combinations of MDA and VGA:
                // VGAEN MDAP Description
                // 0 0 All References to MDA and VGA space are not claimed by Device 1 Function 2.
                // 0 1 Illegal combination
                // 1 0 All VGA and MDA references are routed to PCI Express Graphics Attach device 1 function 2.
                // 1 1 All VGA references are routed to PCI Express Graphics Attach device 1 function 2. MDA references are not claimed by device 1 function 2.VGA and MDA memory cycles can only be routed across PEG12 when MAE (PCICMD12[1]) is set. VGA and MDA I/O cycles can only be routed across PEG12 if IOAE (PCICMD12[0]) is set. 
        UINT8 MDAP60:1; // 3 0h RW MDAP60: This bit works with the VGA Enable bits in the BCTRL register of Device 1 Function 2 to control the routing of Processor initiated transactions targeting MDA compatible I/O and memory address ranges. This bit should not be set if device 1 function 2 VGA Enable bit is not set.If device 1 function 2 VGA enable bit is not set, then accesses to IO address range x3BCh-x3BFh remain on the backbone.If the VGA enable bit is set and MDA is not present, then accesses to IO address range x3BCh-x3BFh are forwarded to PCI Express through device 1 function 2 if the address is within the corresponding IOBASE and IOLIMIT, otherwise they remain on the backbone.
                //MDA resources are defined as the following:
                // Memory: 0B0h - 0B7FFFh
                // I/O: 3B4h, 3B5h, 3B8h, 3B9h, 3BAh, 3BFh, (including ISA address aliases, A[15:10] are not used in decode)
                //Any I/O reference that includes the I/O locations listed above, or their aliases, will remain on the backbone even if the reference also includes I/O locations not listed above.
                //The following table shows the behavior for all combinations of MDA and VGA: 
                // VGAEN MDAP Description
                // 0 0 All References to MDA and VGA space are not claimed by Device 1 Function 2.
                // 0 1 Illegal combination
                // 1 0 All VGA and MDA references are routed to PCI Express Graphics Attach device 1 function 2.
                // 1 1 All VGA references are routed to PCI Express Graphics Attach device 1 function 2. MDA references are not claimed by device 1 function 2. VGA and MDA memory cycles can only be routed across PEG12 when MAE (PCICMD12[1]) is set. VGA and MDA I/O cycles can only be routed across PEG12 if IOAE (PCICMD12[0]) is set.
        UINT8 Reserved1:3; // 6:4 0h RO Reserved (RSVD): Reserved. 
        UINT8 HEN:1; // 7 0h RW HEN: This field enables a memory hole in DRAM space. The DRAM that lies “behind” this space is not remapped.
                //0: No memory hole.
                //1: Memory hole from 15MB to 16MB. This bit is Intel TXT lockable. 
    } LAC; // 87h Legacy Access Control (LAC) This 8-bit register controls steering of MDA cycles and a fixed DRAM hole from 15-16MB
    struct {
        UINT8 C_BASE_SEG:3; // 2:0 2h RO C_BASE_SEG: This field indicates the location of SMM space. SMM DRAM is not remapped. It is simply made visible if the conditions are right to access SMM space, otherwise the access is forwarded to DMI. Only SMM space between A_0h and B_FFFFh is supported, so this field is hardwired to 010b. 
        UINT8 G_SMRAME:1; // 3 0h RW_L G_SMRAME: If set to '1', then Compatible SMRAM functions are enabled, providing 128KB of DRAM accessible at the A_0h address while in SMM. Once D_LCK is set, this bit becomes RO. 
        UINT8 D_LCK:1; // 4 0h RW_KL D_LCK: When D_LCK=1, then D_OPEN is reset to 0 and all writeable fields in this register are locked (become RO). D_LCK can be set to 1 via a normal configuration space write but can only be cleared by a Full Reset.The combination of D_LCK and D_OPEN provide convenience with security. The BIOS can use the D_OPEN function to initialize SMM space and then use D_LCK to “lock down” SMM space in the future so that no application software (or even BIOS itself) can violate the integrity of SMM space, even if the program has knowledge of the D_OPEN function. 
        UINT8 D_CLS:1; // 5 0h RW_L D_CLS: When D_CLS = 1, SMM DRAM space is not accessible to data references, even if SMM decode is active. Code references may still access SMM DRAM space. This will allow SMM software to reference through SMM space to update the display even when SMM is mapped over the VGA range. Software should ensure that D_OPEN = 1 and D_CLS = 1 are not set at the same time. 
        UINT8 D_OPEN:1; // 6 0h RW_LV D_OPEN: When D_OPEN = 1 and D_LCK = 0, the SMM DRAM space is made visible even when SMM decode is not active. This is intended to help BIOS initialize SMM space. Software should ensure that D_OPEN = 1 and D_CLS = 1 are not set at the same time. 
        UINT8 Reserved1:1; // 7 0h RO Reserved (RSVD): Reserved. 
    } SMRAMC; // 88h System Management RAM Control (SMRAMC) The SMRAMC register controls how accesses to Compatible SMRAM spaces are treated. The Open, Close and Lock bits function only when G_SMRAME bit is set to 1. Also, the Open bit should be reset before the Lock bit is set
    UINT8 Reserved1[6]; // 89h - 8fh
    struct {
        UINT64 LOCK:1; // 0 0h RW_KL LOCK: This bit will lock all writeable settings in this register, including itself.
        UINT64 Reserved1:19; // 19:1 0h RO Reserved (RSVD): Reserved. 
        UINT64 REMAPBASE:19; // 38:20 7FFFFh RW_L REMAPBASE: The value in this register defines the lower boundary of the Remap window. The Remap window is inclusive of this address. In the decoder A[19:0] of the Remap Base Address are assumed to be 0's. Thus the bottom of the defined memory range will be aligned to a 1MB boundary. When the value in this register is greater than the value programmed into the Remap Limit register, the Remap window is disabled.These bits are Intel TXT lockable. 
        UINT64 Reserved2:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
    } REMAPBASE; // 90h Remap Base Address Register (REMAPBASE)
    struct {
        UINT64 LOCK:1; // 0 0h RW_KL LOCK: This bit will lock all writeable settings in this register, including itself.
        UINT64 Reserved1:19; // 19:1 0h RO Reserved (RSVD): Reserved. 
        UINT64 REMAPLMT:19; // 38:20 0h RW_L REMAPLMT: The value in this register defines the upper boundary of the Remap window. The Remap window is inclusive of this address. In the decoder A[19:0] of the remap limit address are assumed to be F's. Thus the top of the defined range will be one byte less than a 1MB boundary.When the value in this register is less than the value programmed into the Remap Base register, the Remap window is disabled.These Bits are Intel TXT lockable. 
        UINT64 Reserved2:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
    } REMAPLIMIT; // 98h Remap Limit Address Register (REMAPLIMIT)
    struct {
        UINT64 LOCK:1; // 0 0h RW_KL LOCK: This bit will lock all writeable settings in this register, including itself.
        UINT64 Reserved1:19; // 19:1 0h RO Reserved (RSVD): Reserved. 
        UINT64 TOM:19; // 38:20 7ffffh RW_L TOM: This register reflects the total amount of populated physical memory. This is NOT necessarily the highest main memory address (holes may exist in main memory address map due to addresses allocated for memory mapped IO). These bits correspond to address bits 38:20 (1MB granularity). Bits 19:0 are assumed to be 0. All the bits in this register are locked in Intel TXT mode
        UINT64 Reserved2:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
    } TOM; // A0h Top of Memory (TOM) This Register contains the size of physical memory. BIOS determines the memory size reported to the OS using this Register.
    struct {
        UINT64 LOCK:1; // 0 0h RW_KL LOCK: This bit will lock all writeable settings in this register, including itself.
        UINT64 Reserved1:19; // 19:1 0h RO Reserved (RSVD): Reserved. 
        UINT64 TOUUD:19; // 38:20 0 RW_L This register contains bits 38 to 20 of an address one byte above the maximum DRAM memory above 4G that is usable by the operating system. Configuration software should set this value to TOM minus all ME stolen memory if reclaim is disabled. If reclaim is enabled, this value should be set to reclaim limit 1MB aligned since reclaim limit + 1byte is 1MB aligned. Address bits 19:0 are assumed to be 000_0h for the purposes of address comparison. The Host interface positively decodes an address towards DRAM if the incoming address is less than the value programmed in this register and greater than 4GB.All the bits in this register are locked in Intel TXT mode.
        UINT64 Reserved2:25; // 63:39 0h RO Reserved (RSVD): Reserved. 
    } TOUUD; // A8h Top of Upper Usable DRAM (TOUUD) This 64 bit register defines the Top of Upper Usable DRAM.Configuration software should set this value to TOM minus all ME stolen memory if reclaim is disabled. If reclaim is enabled, this value should be set to reclaim limit + 1byte, 1MB aligned, since reclaim limit is 1MB aligned. Address bits 19:0 are assumed to be 000_0h for the purposes of address comparison. The Host interface positively decodes an address towards DRAM if the incoming address is less than the value programmed in this register and greater than or equal to 4GB.BIOS Restriction: Minimum value for TOUUD is 4GB.These bits are Intel TXT lockable
    struct {
        UINT32 LOCK:1; // 0 0h RW_KL LOCK: This bit will lock all writeable settings in this register, including itself.
        UINT32 Reserved1:19; // 19:1 0h RO Reserved (RSVD): Reserved. 
        UINT32 BDSM:12; // 31:20 0 RW_L BDSM: This register contains bits 31 to 20 of the base address of stolen DRAM memory. BIOS determines the base of graphics stolen memory by subtracting the graphics stolen memory size (PCI Device 0 offset 50 bits 15:8) from TOLUD (PCI Device 0 offset BC bits 31:20). 
    } BDSM; // B0h Base Data of Stolen Memory (BDSM) This register contains the base address of graphics data stolen DRAM memory. BIOS determines the base of graphics data stolen memory by subtracting the graphics data stolen memory size (PCI Device 0 offset 52 bits 7:4) from TOLUD (PCI Device 0 offset BC bits 31:20)
    struct {
        UINT32 LOCK:1; // 0 0h RW_KL LOCK: This bit will lock all writeable settings in this register, including itself.
        UINT32 Reserved1:19; // 19:1 0h RO Reserved (RSVD): Reserved. 
        UINT32 BGSM:12; // 31:20 0 RW_L BGSM: This register contains the base address of stolen DRAM memory for the GTT. BIOS determines the base of GTT stolen memory by subtracting the GTT graphics stolen memory size (PCI Device 0 offset 50 bits 7:6) from the Graphics Base of Data Stolen Memory (PCI Device 0 offset B0 bits 31:20).
    } BGSM; // B4h Base of GTT stolen Memory (BGSM) This register contains the base address of stolen DRAM memory for the GTT. BIOS determines the base of GTT stolen memory by subtracting the GTT graphics stolen memory size (PCI Device 0 offset 52 bits 9:8) from the Graphics Base of Data Stolen Memory (PCI Device 0 offset B0 bits 31:20).
    struct {
        UINT32 LOCK:1; // 0 0h RW_KL LOCK: This bit will lock all writeable settings in this register, including itself.
        UINT32 Reserved1:19; // 19:1 0h RO Reserved (RSVD): Reserved. 
        UINT32 TSEGMB:12; // 31:20 0 RW_L TSEGMB: This register contains the base address of TSEG DRAM memory. BIOS determines the base of TSEG memory which should be at or below Graphics Base of GTT Stolen Memory (PCI Device 0 Offset B4 bits 31:20). BIOS should program the value of TSEGMB to be the same as BGSM when TSEG is disabled. 
    } TSEGMB; // B8h TSEG Memory Base (TSEGMB) This register contains the base address of TSEG DRAM memory. BIOS determines the base of TSEG memory which should be at or below Graphics Base of GTT Stolen Memory (PCI Device 0 Offset B4 bits 31:20)
    struct {
        UINT32 LOCK:1; // 0 0h RW_KL LOCK: This bit will lock all writeable settings in this register, including itself.
        UINT32 Reserved1:19; // 19:1 0h RO Reserved (RSVD): Reserved. 
        UINT32 TOLUD:12; // 31:20 1 RW_L TOLUD: This register contains bits 31 to 20 of an address one byte above the maximum DRAM memory below 4G that is usable by the operating system. Address bits 31 down to 20 programmed to 01h implies a minimum memory size of 1 MB. Configuration software should set this value to the smaller of the following 2 choices: maximum amount memory in the system minus ME stolen memory plus one byte or the minimum address allocated for PCI memory. Address bits 19:0 are assumed to be 0_0h for the purposes of address comparison. The Host interface positively decodes an address towards DRAM if the incoming address is less than the value programmed in this register.The Top of Low Usable DRAM is the lowest address above both Graphics Stolen memory and Tseg. BIOS determines the base of Graphics Stolen Memory by subtracting the Graphics Stolen Memory Size from TOLUD and further decrements by Tseg size to determine base of Tseg. All the Bits in this register are locked in Intel TXT mode.This register should be 1 MB aligned when reclaim is enabled. 
    } TOLUD; // BCh Top of Low Usable DRAM (TOLUD) This 32 bit register defines the Top of Low Usable DRAM. TSEG, GTT Graphics memory and Graphics Stolen Memory are within the DRAM space defined. From the top, the Host optionally claims 1 to 64MBs of DRAM for Processor Graphics if enabled, 1or 2MB of DRAM for GTT Graphics Stolen Memory (if enabled) and 1, 2, or 8 MB of DRAM for TSEG if enabled.
    UINT8 Reserved2[8]; // C0h - C7h
    struct {
        UINT16 DSERR:1; // 0 0h RW1CS DSERR: If this bit is set to 1, a memory read data transfer had a single-bit correctable error and the corrected data was returned to the requesting agent. When this bit is set the column, row, bank, and rank where the error occurred and the syndrome of the error are logged in the ECC Error Log register in the channel where the error occurred. Once this bit is set the ECCERRLOGx fields are locked to further single-bit error updates until the Processor clears this bit by writing a 1. A multiple bit error that occurs after this bit is set will overwrite the ECCERRLOGx fields with the multiple-bit error signature and the DMERR bit will also be set. A single bit error that occurs after a multi-bit error will set this bit but will not overwrite the other fields. 
        UINT16 DMERR:1; // 1 0h RW1CS DMERR: If this bit is set to 1, a memory read data transfer had an uncorrectable multiple-bit error. When this bit is set, the column, row, bank, and rank that caused the error, and the error syndrome, are logged in the ECC Error Log register in the channel where the error occurred. Once this bit is set, the ECCERRLOGx fields are locked until the Processor clears this bit by writing a 1. Software uses bits [1:0] to detect whether the logged error address is for a Single-bit or a Multiple-bit error. 
        UINT16 Reserved1:14; // 15:2 0h RO Reserved (RSVD): Reserved. 
    } ERRSTS; // C8h Error Status (ERRSTS) This register is used to report various error conditions via the SERR DMI messaging mechanism. An SERR DMI message is generated on a zero to one transition of any of these flags (if enabled by the ERRCMD and PCICMD registers).
    struct {
        UINT16 DSERR:1; // 0 0h RW DSERR: 
                //1: The Host Bridge generates an SERR special cycle over DMI when the DRAM controller detects a single bit error.
                //0: Reporting of this condition via SERR messaging is disabled.For systems that do not support ECC, this bit should be disabled. 
        UINT16 DMERR:1; // 1 0h RW DMERR: 
                //1: The Host Bridge generates an SERR message over DMI when it detects a multiplebit error reported by the DRAM controller.
                //0: Reporting of this condition via SERR messaging is disabled. For systems not supporting ECC this bit should be disabled. 
        UINT16 Reserved1:14; // 15:2 0h RO Reserved (RSVD): Reserved. 
    } ERRCMD; // CAh Error Command (ERRCMD) This register controls the Host Bridge responses to various system errors. Since the Host Bridge does not have an SERRB signal, SERR messages are passed from the Processor to the PCH over DMI
    struct {
        UINT16 DSESMI:1; // 0 0h RW DSESMI: 
                //1: The Host generates an SMI DMI special cycle when the DRAM controller detects a single bit error.
                //0: Reporting of this condition via SMI messaging is disabled. For systems that do not support ECC this bit should be disabled. 
        UINT16 DMESMI:1; // 1 0h RW DMESMI:
                //1: The Host generates an SMI DMI message when it detects a multiple-bit error reported by the DRAM controller.
                //0: Reporting of this condition via SMI messaging is disabled. For systems not supporting ECC this bit should be disabled. 
        UINT16 Reserved1:14; // 15:2 0h RO Reserved (RSVD): Reserved. 
    } SMICMD; // CCh SMI Command (SMICMD) This register enables various errors to generate an SMI DMI special cycle. When an error flag is set in the ERRSTS register, it can generate an SERR, SMI, or SCI DMI special cycle when enabled in the ERRCMD, SMICMD, or SCICMD registers, respectively. Note that one and only one message type can be enabled.
    struct {
        UINT16 DSESCI:1; // 0 0h RW DSESCI: 
                        //1: The Host generates an SCI DMI special cycle when the DRAM controller detects a single bit error.
                        //0: Reporting of this condition via SCI messaging is disabled. For systems that do not support ECC this bit should be disabled. 
        UINT16 DMESCI:1; // 1 0h RW DMESCI: 
                        //1: The Host generates an SCI DMI message when it detects a multiple-bit error reported by the DRAM controller.
                        //0: Reporting of this condition via SCI messaging is disabled. For systems not supporting ECC this bit should be disabled. 
        UINT16 Reserved1:14; // 15:2 0h RO Reserved (RSVD): Reserved. 
    } SCICMD; // CEh SCI Command (SCICMD) This register enables various errors to generate an SMI DMI special cycle. When an error flag is set in the ERRSTS register, it can generate an SERR, SMI, or SCI DMI special cycle when enabled in the ERRCMD, SMICMD, or SCICMD registers, respectively. Note that one and only one message type can be enabled.
    UINT8 Reserved3[12]; // D0h - DBh
    struct {
        UINT32 SKPD:32; // 31:0 0h RW SKPD: 1 DWORD of data storage. 
    } SKPD; // DCh Scratchpad Data (SKPD) This register holds 32 writable bits with no functionality behind them. It is for the convenience of BIOS and graphics drivers
    UINT32 Reserved4;
    struct {
        UINT32 Reserved1:12; // 11:0 0h RO Reserved (RSVD): Reserved. 
        UINT32 PDCD:1; // 12 0h RO PDCD: 
                    // 0: Capable of Dual Channels
                    // 1: Not Capable of Dual Channel - only single channel capable. 
        UINT32 X2APIC_EN:1; // 13 0h RO X2APIC_EN: Extended Interrupt Mode.
                        //0: Hardware does not support Extended APIC mode.
                        //1: Hardware supports Extended APIC mode. 
        UINT32 DDPCD:1; // 14 0h RO DDPCD: Allows Dual Channel operation but only supports 1 DIMM per channel.
                        //0: 2 DIMMs per channel enabled
                        //1: 2 DIMMs per channel disabled. This setting hardwires bits 2 and 3 of the rank population field for each channel to zero. (MCHBAR offset 260h, bits 22-23 for channel 0 and MCHBAR offset 660h, bits 22-23 for channel 1) 
        UINT32 Reserved2:8; // 22:15 0h RO Reserved (RSVD): Reserved. 
        UINT32 VTDD:1; // 23 0h RO_KFW VTDD: 
                    //0: Enable VTd
                    //1: Disable VTd 
        UINT32 Reserved3:1; // 24 0h RO Reserved (RSVD): Reserved. 
        UINT32 ECCDIS:1; // 25 0h RO ECCDIS: 
                        //0: ECC capable
                        //1: Not ECC capable 
        UINT32 Reserved4:6; // 31:26 0h RO Reserved (RSVD): Reserved. 
    } CAPID0; // E4h Capabilities A (CAPID0) Control of bits in this register are only required for customer visible SKU differentiation.
    struct {
        UINT32 Reserved1:2; // 1:0 0h RO Reserved (RSVD): Reserved. 
        UINT32 LPDDR3_EN:1; // 2 0h RO LPDDR3_EN: Allow LPDDR3 operation 
        UINT32 Reserved2:1; // 3 0h RO Reserved (RSVD): Reserved. 
        UINT32 DMFC_DDR3:3; // 6:4 0h RO DMFC_DDR3: This field controls which values may be written to the Memory Frequency Select field 6:4 of the Clocking Configuration registers (MCHBAR Offset C00h). Any attempt to write an unsupported value will be ignored.
                            //000: MC capable of DDR3 2667 (2667 is the upper limit)
                            //001: MC capable of up to DDR3 2667
                            //010: MC capable of up to DDR3 2400
                            //011: MC capable of up to DDR3 2133
                            //100: MC capable of up to DDR3 1867
                            //101: MC capable of up to DDR3 1600
                            //110: MC capable of up to DDR3 1333
                            //111: MC capable of up to DDR3 1067 
        UINT32 Reserved3:1; // 7 0h RO Reserved (RSVD): Reserved. 
        UINT32 GMM_DIS:1; // 8 0h RO_KFW GMM_DIS: 
                        //0: Device 8 associated memory spaces are accessible.
                        //1: Device 8 associated memory and IO spaces are disabled by hardwiring the D8EN field, bit 1 of the Device Enable register, (DEVEN Dev 0 Offset 54h) to '0'. 
        UINT32 Reserved4:6; // 14:9 0h RO Reserved (RSVD): Reserved. 
        UINT32 DMIG3DIS:1; // 15 0h RO DMIG3DIS: DMI Gen 3 Disable.
        UINT32 Reserved5:1; // 16 0h RO Reserved (RSVD): Reserved. 
        UINT32 ADDGFXCAP:1; // 17 0h RO ADDGFXCAP: 
                        //0: Capable of Additive Graphics
                        //1: Not capable of Additive Graphics 
        UINT32 ADDGFXEN:1; // 18 0h RO ADDGFXEN: 
                        //0: Additive Graphics Disabled
                        //1: Additive Graphics Enabled 
        UINT32 Reserved6:1; // 19 0h RO Reserved (RSVD): Reserved. 
        UINT32 PEGG3_DIS:1; // 20 0h RO PEGG3_DIS: the processor: PCIe Gen 3 Disable. 
                            //0: Capable of running any of the Gen 3-compliant PEG controllers in Gen 3 mode (Devices 0/1/0, 0/1/1, 0/1/2) 
                            //1: Not capable of running any of the PEG controllers in Gen 3 mode 
        UINT32 Reserved7:4; // 24:21 0h RO Reserved (RSVD): Reserved. 
        UINT32 CACHESZ:3; // 27:25 0h RO CACHESZ: This setting indicates the supporting cache sizes. 
        UINT32 SMT:1; // 28 0h RO SMT: This setting indicates whether or not the Processor is SMT capable. 
        UINT32 Reserved8:2; // 30:29 0h RO Reserved (RSVD): Reserved. 
        UINT32 IMGU_DIS:1; // 31 0h RO_KFW IMGU_DIS: 
                        //0: Device 5 associated memory spaces are accessible.
                        //1: Device 5 associated memory and IO spaces are disabled by hardwiring the D1F2EN field, bit 1 of the Device Enable register, (DEVEN Dev 0 Offset 54h) to '0'. 
    } CAPID1; // E8h Capabilities B (CAPID1) Control of bits in this register are only required for customer visible SKU differentiation
    struct {
        UINT32 Reserved1:14; // 13:0 0h RO Reserved (RSVD): Reserved.
        UINT32 DMFC_LPDDR3:3; // 16:14 0h RO DMFC_LPDDR3: hardware will update this field with the value of maximum frequency for LPDDR3. 
        UINT32 DMFC_DDR4:3; // 19:17 0h RO DMFC_DDR4: hardware will update this field with the value of maximum frequency for DDR4. 
        UINT32 Reserved2:12; // 31:20 0h RO Reserved (RSVD): Reserved. 
    } CAPID2; // EC Capabilities C (CAPID0) Control of bits in this register are only required for customer visible SKU differentiation.
} HOST_BRIDGE_DRAM_CFG, *PHOST_BRIDGE_DRAM_CFG; // f0

//
// DMI BAR
//typedef struct _DMI_BAR {
//0–3h 4 DMI Virtual Channel Enhanced Capability (DMIVCECH) 
//4–7h 4 DMI Port VC Capability Register 1 (DMIPVCCAP1) 
//8–Bh 4 DMI Port VC Capability Register 2 (DMIPVCCAP2) 
//C–Dh 2 DMI Port VC Control (DMIPVCCTL) 
//10–13h 4 DMI VC0 Resource Capability (DMIVC0RCAP) 
//14–17h 4 DMI VC0 Resource Control (DMIVC0RCTL) 
//1A–1Bh 2 DMI VC0 Resource Status (DMIVC0RSTS) 
//1C–1Fh 4 DMI VC1 Resource Capability (DMIVC1RCAP) 
//20–23h 4 DMI VC1 Resource Control (DMIVC1RCTL) 
//26–27h 2 DMI VC1 Resource Status (DMIVC1RSTS) 
//34–37h 4 DMI VCm Resource Capability (DMIVCMRCAP) 
//38–3Bh 4 DMI VCm Resource Control (DMIVCMRCTL) 
//3E–3Fh 2 DMI VCm Resource Status (DMIVCMRSTS) 
//40–43h 4 DMI Root Complex Link Declaration (DMIRCLDECH) 
//44–47h 4 DMI Element Self Description (DMIESD) 
//50–53h 4 DMI Link Entry 1 Description (DMILE1D) 
//58–5Bh 4 DMI Link Entry 1 Address (DMILE1A) 
//5C–5Fh 4 DMI Link Upper Entry 1 Address (DMILUE1A) 
//60–63h 4 DMI Link Entry 2 Description (DMILE2D) 
//68–6Bh 4 DMI Link Entry 2 Address (DMILE2A) 
//84–87h 4 Link Capabilities (LCAP) 
//88–89h 2 Link Control (LCTL) 
//8A–8Bh 2 DMI Link Status (LSTS) 
//98–99h 2 Link Control 2 (LCTL2) 
//9A–9Bh 2 Link Status 2 (LSTS2) 
//1C4–1C7h 4 DMI Uncorrectable Error Status (DMIUESTS) 
//1C8–1CBh 4 DMI Uncorrectable Error Mask (DMIUEMSK) 
//1CC–1CFh 4 DMI Uncorrectable Error Severity (DMIUESEV) 
//1D0–1D3h 4 DMI Correctable Error Status (DMICESTS) 
//1D4–1D7h 4 DMI Correctable Error Mask (DMICEMSK) 
//} DMI_BAR, *PDMI_BAR;


//
// MCH BAR
//typedef struct _MCH_BAR {
//4000h 4 MCHBAR_CH0_CR_TC_PRE_0_0_0_MCHBAR 
//401Ch 4 MCHBAR_CH0_CR_SC_GS_CFG_0_0_0_MCHBAR 
//4048–404Bh 4 ECCERRLOG0 
//404C–404Fh 4 ECC Error Log 1 (ECCERRLOG1) 
//4070h 4 MCHBAR_CH0_CR_TC_ODT_0_0_0_MCHBAR 
//4238–423Bh 4 Refresh Parameters (TC) 
//423C–423Fh 4 Refresh Timing Parameters (TC) 
//4260–4263h 4 Power Management DIMM Idle Energy (PM) 
//4264–4267h 4 Power Management DIMM Power Down Energy (PM) 
//4268–426Bh 4 Power Management DIMM Activate Energy (PM) 
//426C–426Fh 4 Power Management DIMM RdCas Energy (PM) 
//4270–4273h 4 Power Management DIMM WrCas Energy (PM) 
//4400h 4 MCHBAR_CH1_CR_TC_PRE_0_0_0_MCHBAR 
//441Ch 4 MCHBAR_CH0_CR_SC_GS_CFG_0_0_0_MCHBAR 
//4448–444Bh 4 ECCERRLOG0 
//444C–444Fh 4 ECC Error Log 1 (ECCERRLOG1) 
//4470h 4 MCHBAR_CH0_CR_TC_ODT_0_0_0_MCHBAR 
//4638–463Bh 4 Refresh Parameters (TC) 
//463C–463Fh 4 Refresh Timing Parameters (TC) 
//4660–4663h 4 Power Management DIMM Idle Energy (PM) 
//4664–4667h 4 Power Management DIMM Power Down Energy (PM) 
//4668–466Bh 4 Power Management DIMM Activate Energy (PM) 
//466C–466Fh 4 Power Management DIMM RdCas Energy (PM) 
//4670–4673h 4 Power Management DIMM WrCas Energy (PM) 
//4C1Ch 4 MCSCHEDS_CR_SC_GS_CFG_0_0_0_MCHBAR 
//4C40–4C43h 4 PM 
//4C48–4C4Bh 4 ECCERRLOG0 
//4C4C–4C4Fh 4 ECC Error Log 1 (ECCERRLOG1) 
//4C70h 4 MCSCHEDS_CR_TC_ODT_0_0_0_MCHBAR 
//4E38–4E3Bh 4 Refresh Parameters (TC) 
//4E3C–4E3Fh 4 Refresh Timing Parameters (TC) 
//4E60–4E63h 4 Power Management DIMM Idle Energy (PM) 
//4E64–4E67h 4 Power Management DIMM Power Down Energy (PM) 
//4E68–4E6Bh 4 Power Management DIMM Activate Energy (PM) 
//4E6C–4E6Fh 4 Power Management DIMM RdCas Energy (PM)
//4E70–4E73h 4 Power Management DIMM WrCas Energy (PM) 
//5000–5003h 4 Address Decoder Inter Channel Configuration Register (MAD)—Offset 
//5000h 0h 
//5004–5007h 4 Address Decoder Intra Channel Configuration Register (MAD)—Offset 
//5004h 0h 
//5008–500Bh 4 Address Decoder Intra Channel Configuration Register (MAD)—Offset 
//5008h 0h 
//500C–500Fh 4 Address Decode DIMM Parameters. (MAD) 
//5010–5013h 4 Address Decode DIMM Parameters (MAD) 
//5034h 4 MCDECS_CR_MRC_REVISION_0_0_0_MCHBAR_MCMAIN—Offset 
//5034h 0h 
//5040–5043h 4 Request Count from GT (DRAM) 
//5044–5047h 4 Request Count from IA (DRAM) 
//5048–504Bh 4 Request Count from IO (DRAM) 
//5050–5053h 4 RD Data Count (DRAM) 
//5054–5057h 4 WR Data Count (DRAM) 
//5060–5063h 4 Self Refresh Configuration Register (PM) 
//5090–5093h 4 Address Compare for ECC Error Injection (ECC) 
//5094–5097h 4 Address Mask for ECC Error Injection (ECC) 
//5400h 4 NCDECS_CR_GFXVTBAR_0_0_0_MCHBAR_NCU 
//5410h 4 NCDECS_CR_VTDPVC0BAR_0_0_0_MCHBAR_NCU 
//5820–5823h 4 PACKAGE 
//5828–582Fh 8 PKG 
//5830–5837h 8 PKG 
//5838–583Fh 8 PKG 
//5840–5847h 8 PKG 
//5848–584Fh 8 PKG 
//5858–585Fh 8 PKG 
//5880–5883h 4 DDR 
//5884–5887h 4 DRAM 
//5888–588Bh 4 DRAM 
//588C–588Fh 4 DDR 
//5890–5893h 4 DDR 
//5894–5897h 4 DDR 
//5898–589Bh 4 DDR 
//589C–589Fh 4 DDR 
//58A0–58A3h 4 DDR 
//58A8–58ABh 4 PACKAGE 
//58B0–58B3h 4 DDR 
//58B4–58B7h 4 DDR 
//58C0–58C7h 8 DDR 
//58C8–58CFh 8 DDR
//58D0–58D3h 4 DDR 
//58D4–58D7h 4 DDR 
//58D8–58DBh 4 DDR 
//58DC–58DFh 4 DDR 
//58F0–58F3h 4 PACKAGE 
//58FC–58FFh 4 IA 
//5900–5903h 4 GT 
//5918–591Bh 4 SA 
//5948–594Bh 4 GT 
//594C–594Fh 4 EDRAM 
//5978–597Bh 4 Package 
//597C–597Fh 4 PP0 
//5980–5983h 4 PP1 
//5994–5997h 4 RP 
//5998–599Bh 4 RP 
//5D10–5D17h 8 SSKPD 
//5DA8–5DABh 4 BIOS_RESET_CPL 
//5E00h 4 PCU_CR_MC_BIOS_REQ_0_0_0_MCHBAR_PCU 
//5F3C–5F3Fh 4 CONFIG 
//5F40–5F47h 8 CONFIG 
//5F48–5F4Fh 8 CONFIG 
//5F50–5F53h 4 CONFIG 
//5F54–5F57h 4 TURBO 
//6200–6203h 4 Package Thermal DPPM Status (PKG) 
//6204–6207h 4 Memory Thermal DPPM Status (DDR)
//} MCH_BAR, *PMCH_BAR;

//
// BDF 00:04:00
// Dynamic Power Performance Management
typedef struct _DPPM {
    PCI_CONFIG_GENERAL Header; // 00h - 3Fh
    UINT8 RES_53_40[0x14]; // 50-40
    UINT32 DEVEN; // 54–57h 4 Device Enable
    UINT8 RES_E3_58[0x8C]; // E3-58
    UINT32 CAPID0; // E4–E7h 4 Capabilities A
    UINT32 CAPID1; // E8–EBh 4 Capabilities B
} DYNAMIC_POWER_PERFORMANCE_MANAGEMENT_CFG, *PDYNAMIC_POWER_PERFORMANCE_MANAGEMENT_CFG; // 4c

//
//BDF 00:12:00
typedef struct _TSS {
    PCI_CONFIG_GENERAL Header; // 00h - 3Fh
    struct {
        UINT32 SPTYPEN:1; // 0 0h RW Space Type Enable (SPTYPEN): When set to 1b by software, enables the decode of this memory BAR.
        UINT32 ADDRNG:2; // 2:1 2h RO Address Range (ADDRNG): Indicates that this BAR can be located anywhere in 64 bit address space.
        UINT32 PREF:1; // 3 0h RO Prefetchable (PREF): Indicates that this BAR is NOT pre-fetchable.
        UINT32 RES_11_4:8; // 11:4 0h RO Reserved.
        UINT32 TBA:20; // 31:12 0h RW Thermal Base Address (TBA): Base address for the Thermal logic memory mapped  configuration registers. 4KB bytes are requested by hardwiring bits 11:4 to 0's.
    } TBARB; // 40h 43h BIOS Assigned Thermal Base Address (TBARB) This BAR creates 4K bytes of memory space to signify the base address of Thermal memory mapped configuration registers. This BAR is owned by the BIOS, and allows the BIOS to locate the Thermal registers in system memory space.  BIOS and/or SMM use this register outside of official OS visibility. Therefore this BAR must not be affected by OS setting or clearing of MSE or the power state.
    struct {
        UINT32 TBAH:32; // 31:0 0h RW Thermal Base Address High (TBAH): TBAR bits 61:32
    } TBARBH; // 44h 47h BIOS Assigned Thermal Base High DWord (TBARBH) This BAR extension holds the high 32 bits of the 64 bit TBARB
    struct {
        UINT8 URRE:1; // 0 Reporting Enable (URRE): When '1', the agent will set the URD bit. If SERR# enable (SEN) is set, then the agent will also send SERR# to the system. Note that both URRE and SEN must be set to generate an SERR#.
        UINT8 RES_7_1:7; // 7:1 Reserved
    } CB; // 48h 48h Control Bits (CB)
} THERMAL_SUBSYSTEM_CFG, *PTHERMAL_SUBSYSTEM_CFG; // 4c

// bar registers of thermal subsystem
typedef struct _TRMMR {
    struct {
        UINT16 TSR:9; // 8:0 TS Reading The die temperature with resolution of 0.5 degree C and an offset of -50C. Thus a reading of 0x121 is 94.5C.
        UINT16 RES_15_9:7; // 15:9 
    } TEMP; // 0h 1h Temperature (TEMP)
    UINT16 RES_3_2; // 
    struct {
        UINT16 CPDE:1; // 0 0h RW/L Catastrophic Power-Down Enable (CPDE): When set to 1, the power  management logic (PMC) transitions to the S5 state when a catastrophic temperature is detected by the sensor. 
        UINT16 RES_6_1:6; // 6:1 0h RO Reserved.
        UINT16 PLDB:1; // 7 0h RW/1L Policy Lock-Down Bit (PLDB): When written to 1, this bit prevents any more writes to this register (offset 04h) and to CTT (offset 0x10)
        UINT16 RES_15_8:8; // 15:8 0h RO Reserved.
    } TSC; // 4h 4h Thermal Sensor Control (TSC)
    struct {
        UINT16 RES_1_0:2; // 1:0 0h RO Reserved.
        UINT16 SMIS:1; // 2 0h RW/1C SMI Status (SMIS): Set when SMI is enabled for a trip event. SW must write a 1 to this bit to clear the SMI status. As long as this bit is set, the SMI indication to the global SMI logic is asserted.
        UINT16 GPES:1; // 3 0h RW/1C GPE Status (GPES): Set when GPE is enabled for a trip event. SW must write a 1 to this bit to clear the GPE status. Note that GPE can be configured to cause an SMI or SCI. As long as this bit is set, the GPE indication to the global GPE logic is asserted..
        UINT16 TSDSS:1; // 4 0h RO Thermal Sensor Dynamic Shutdown Status (TSDSS): Thermal Sensor Dynamic Shutdown Status (TSDSS): This bit indicates the status of the thermal sensor circuit when TSEL.ETS=1. 1: thermal sensor is fully operational 0: thermal sensor is in a dynamic shutdown state
        UINT16 RES_7_5:3; // 7:5 0h RO Reserved.
        UINT16 RES_15_8:8; // 15:8 0h RO Reserved.
    } TSS; // 6h 6h Thermal Sensor Status (TSS)
    struct {
        UINT16 ETS:1; // 0 0h RW/L Enable TS (ETS): 1: Enables the thermal sensor. Until this bit is set, no thermometer readings or trip events will occur. If SW reads the TEMP register before the sensor is enabled, it will read 0x0. The value of this bit is sent to the thermal sensor. NOTE: if the sensor is running and valid temperatures have been captured in TEMP and then ETS is cleared, TEMP will retain its old value. Clearing ETS does not force TEMP to 0x00. 0: Disables the senso
        UINT16 RES_6_1:6; // 6:1 0h RO Reserved.
        UINT16 PLDB:1; // 7 0h RW/1L Policy Lock-Down Bit: When written to 1, this bit prevents any more writes to this register and to TTCB, Test1, Test2, Test3, Test4, Test5, Test6 and Test7 registers.
        UINT16 RES_15_8:8; // 15:8 0h RO Reserved.
    } TSEL; // 8h 8h Thermal Sensor Enable And Lock (TSEL)
    struct {
        UINT16 ESTR:1; // 0 0h RW/L Enable SMBus Temperature Reporting (ESTR): 1: Enables the reporting of the PCH temperature to the SMBus. Note that this must also be set if ME needs access to the PCH temperature. Once enabled this bit should not be cleared by SW. If it is cleared then the EC may get an undefined value. SW has no need to dynamically disable and then re-enable this bit. 0: Disables EC temperature reporting.
        UINT16 RES_6_1:6; // 6:1 0h RO Reserved.
        UINT16 PLDB:1; // 7 0h RW/1L Policy Lock-Down Bit (PLDB): When written to 1, this bit prevents any more writes to this register (offset 04h) and to CTT (offset 0x10)
        UINT16 RES_15_8:8; // 15:8 0h RO Reserved.
    } TSREL; // Ah Ah Thermal Sensor Reporting Enable And Lock (TSREL)
    struct {
        UINT16 ATST:1; // 0 0h RW/L SMI Enable on Alert Thermal Sensor Trip (ATST): 1: Enables SMI# assertions on alert thermal sensor events for either low-to-high or high-to-low events. (Both edges are enabled by this one bit.) 0: Disables SMI# assertions for alert thermal events
        UINT16 RES_6_1:6; // 6:1 0h RO Reserved.
        UINT16 PLDB:1; // 7 0h RW/1L Policy Lock-Down Bit (PLDB): When written to 1, this bit prevents any more writes to this register (offset 04h) and to CTT (offset 0x10)
        UINT16 RES_15_8:8; // 15:8 0h RO Reserved.
    } TSMIC; // Ch Ch Thermal Sensor SMI Control (TSMIC)
    UINT16 RES_F_E; // F:E
    struct {
        UINT16 CTRIP:9; // 8:0 0h RW 8:0 Catastrophic Temperature TRIP (CTRIP): When the current temperature reading is = to the value in this register, a catastrophic trip event is signaled. The value of this register must not be changed while TSEL.ETS is set. The value in this register is sent to the thermal sensor. This register is locked by TSC[7]
        UINT16 RES_15_9:7; // 15:9 0h RO Reserved.
    } CTT; // 10h 11h Catastrophic Trip Point (CTT)
    UINT16 RES_13_12; // 
    struct {
        UINT16 AH:9; // 8:0 0h RW Alert High (AH): Sets the high value for the alert indication. The value of this register must not be changed while TSEL.ETS is set. The value in this register is sent to the thermal sensor.This register is not lockable, so that SW can change the values during runtime.
        UINT16 RES_15_9:7; // 15:9 0h RO Reserved.
    } TAHV; // 14h 15h Thermal Alert High Value (TAHV)
    UINT16 RES_17_16; // 17:16
    struct {
        UINT16 AL:9; // 8:0 0h RW Alert Low (AL): Sets the low value for the alert indication. The value of this register must not be changed while TSEL.ETS is set. The value in this register is sent to the thermal sensor.This register is not lockable, so that SW can change the values during runtime.
        UINT16 RES_15_9:7; // 15:9 0h RO Reserved.
    } TALV; // 18h 19h Thermal Alert Low Value (TALV)
    UINT16 RES_1B_1A; // 
    struct {
        UINT16 LTT:9; // 8:0 0h RW Low Temp Threshold (LTT): Low Temp Threshold (LTT) - Sets the low maximum temp value used for dynamic thermal sensor shutdown consideration. Refer DTSSS0EN for details. This register field is not lockable, so that SW can change the values during runtime.
        UINT16 MAXTSST:3; // 11:9 4h RW/L Maximum Thermal Sensor Shutdown Time (MAXTSST): Maximum Thermal Sensor Shutdown Time (MAXTSST) - sets the maximum time that the thermal sensor will be held in a shutdown state assuming no other wake conditions. This register is used to set the expiration time of a timer that is used to wake up the thermal sensor on expiration. 000: 1 s 001: 2 s 010: 4 s 011: 8 s 100: 16 s 101-111: Reserved
        UINT16 RES_12:1; // 12 0h RO Reserved.
        UINT16 DTSSIC0:1; // 13 0h RW/L Dynamic Thermal Sensor Shutdown in C0 Allowed (DTSSIC0): Dynamic Thermal Sensor Shutdown in C0 Allowed (DTSSIC0) 0: CPU must be in a non-C0 state to allow PCH thermal sensor shutdown 1: CPU can be in a C0 or non-C0 state to allow PCH thermal sensor shutdown.
        UINT16 DTSSS0EN:1; // 14 0h RW/L Dynamic Thermal Sensor Shutdown in S0 Idle Enable (DTSSS0EN): Dynamic Thermal Sensor Shutdown in S0 idle Enable (DTSSS0EN): 1: Dynamic thermal sensor shutdown in S0 idle is enabled. When set to 1, the power management logic shuts down the thermal sensor when the CPU is in a C-state and TEMP.TSR andlt;= LTT.LTT. 0: Dynamic thermal sensor shutdown in S0 idle is disabled
        UINT16 TSPMLOCK:1; // 15 0h RW/1L Thermal Sensor Power Management Lock (TSPMLOCK): Thermal Sensor Power Management Lock (TSPMLOCK): Setting this bit to a 1 causes the rest of the bits in this register to be locked.
    } TSPM; // 1Ch 1Dh Thermal Sensor Power Management (TSPM)
    UINT16 RES_1F_1F; // 
    UINT8 RES_3F_20[0x20]; // 
    struct {
        UINT32 T0L:9; // 8:0 0h RW/LT0 Level (T0L): When TEMP.TSR[8:0] is less than or equal to T0L OR TT.Enable is 0 OR TSE = 0, then the system is in T0 state.
        UINT32 RES_9:1; // 9 0h RO Reserved.
        UINT32 T1L:9; // 18:10 0h RW/L T1 Level (T1L): When TTEN = 1 AND TSE = 1 AND (T1L = TSR[8:0] T0L), then the system is in T1 state. SW NOTE: T1L must be programmed to a value greater than T0L if TTEN=1
        UINT32 RES_19:1; // 19 0h RO Reserved.
        UINT32 T2L:9; // 28:20 0h RW/L T2 Level (T2L): When TTEN = 1 AND TSE = 1 AND (T2L = TSR[8:0] T1L), then the system is in T2 state. When TTEN = 1 AND TSE = 1 AND (TSR[8:0] T2L), then the system is in T3 state. NOTE: the T3 condition overrides PMSync[13] and forces the system to T3 if both cases are true. SW NOTE: T2L must be programmed to a value greater than T1L if TTEN=1
        UINT32 TTEN:1; // 29 0h RW/L TT Enable (TTEN): When set the thermal throttling states are enabled. At reset, BIOS must set bits 28:0 and then do a separate write to set bit 29 to enable throttling. SW may set bit 31 at the same time it sets bit 29 if it wishes to lock the register. If SW wishes to change the values of 28:0, it must first clear the TTEN bit, then change the values in 28:0, and then re-enable TTEN. It is legal to set bits 31, 30 and 29 with the same write. This bit must not be set unless the thermal sensor is already enabled (set TSC[7]=1 and TSC[3:2] = 10).
        UINT32 TT13EN:1; // 30 0h RW/L TT.State13 Enable (TT13EN): When set to 1, then PMSync state 13 will force at least T2 state.
        UINT32 TTL:1; // 31 0h RW/1L TT.Lock (TTL): When set to 1, this entire register (TL) is locked and remains locked until the next platform reset.
    } TL; // 40h 43h Throttle Levels (TL)
    UINT8 RES_4F_44[0xC]; // 
    struct {
       UINT32 RES_13_0:14; // 13:0 0h RO Reserved.
       UINT32 PMCTEN:1; // 14 0h RW/L PMC Throttling Enable (PMCTEN): PMC Throttling Enable (PMCTEN) - When set to 1 and the PMC is requesting throttling, force at least the T-state that PMC is requesting.
       UINT32 TL2LOCK:1; // 15 0h RW/1L TL2 lock (TL2LOCK): TL2.Lock - When set to 1, this entire register (TL2) is locked and remains locked until the next platform reset.
       UINT32 RES_31_16:16; // 31:16 0h RO Reserved.
    } TL2; // 50h 53h Throttle Level 2 (TL2)
    UINT8 RES_5F_54[0xC]; // 
    struct {
        UINT16 PHLL:9; // 8:0 0h RW/L PHL Level (PHLL): Temperature value used for PCHHOT# pin.
        UINT16 RES_14_9:6; // 14:9 0h RO Reserved.
        UINT16 PHLE:1; // 15 0h RW/L PHL Enable (PHLE): When set and the current temperature reading, TSR, is greater  than or equal to PHLL, then the PCHHOT# pin will be asserted (active low).
    } PHL; // 60h 61h PCH Hot Level (PHL)
    struct { 
        UINT16 PHLL:1; // 0 0h RW/1L PHL Lock (PHLL): When written to a 1, then both PHL and PHLC are locked
        UINT16 RES_7_1:7; // 7:1 0h RO Reserved.
        UINT16 RES_15_9:7; // 15:9 0h RO Reserved.
    } PHLC; // 62h 62h PHL Control (PHLC)
    UINT8 RES_7F_64[0x1C]; // 
    struct {
        UINT16 ALHE:1; // 0 0h RW/1C Alert Low-to-High Event (ALHE): 1: Indicates that an Aux Thermal Sensor trip event occurred based on a lower to higher temperature transition thru the trip point. 0: No trip for this event Software must write a 1 to clear this status bit
        UINT16 AHLE:1; // 1 0h RW/1C Alerth High-to-Low Event (AHLE): 1: Indicates that a Hot Thermal Sensor trip event occurred based on a lower to higher temperature transition thru the trip point.  0: No trip for this event Software must write a 1 to clear this status bit.
        UINT16 RES_7_2:6; // 7:2 0h RO Reserved.
        UINT16 RES_15_9:7; // 15:9 0h RO Reserved.
    } TAS; // 80h 80h Thermal Alert Status (TAS)
    struct {
        UINT16 ALHEN:1; // 0 0h RW/1C Alert Low-to-High Enable (ALHEN): Refer the description for bit 1.
        UINT16 AHLEN:1; // 1 0h RW/1C Alert High-to-Low Enable (AHLEN): When set to 1, the thermal sensor logic asserts the Thermal logic PCI INTx signal when the corresponding status bit is set in the Thermal Error Status register. When cleared, the corresponding status bit does not result in PCI INTx.
        UINT16 RES_7_2:6; // 7:2 0h RO Reserved.
        UINT16 RES_15_9:7; // 15:9 0h RO Reserved.
    } TSPIEN; // 82h 82h PCI Interrupt Event Enables (TSPIEN)
    struct {
        UINT16 ALHEN:1; // 0 0h RW/1C Alert Low-to-High Enable (ALHEN): Refer the description for bit 1.
        UINT16 AHLEN:1; // 1 0h RW/1C Alert High-to-Low Enabble (AHLEN): When set to 1, the thermal sensor logic asserts its General Purpose Event signal to the GPE block when the corresponding status bit is set in the Thermal Error Status register. When cleared, the corresponding status bit does not result in the GPE signal assertion.
        UINT16 RES_7_2:6; // 7:2 0h RO Reserved.
        UINT16 RES_15_9:7; // 15:9 0h RO Reserved.
    } TSGPEN; // 84h 84h General Purpose Event Enables (TSGPEN)
    UINT8 RES_DF_86[0x6A]; // 
    struct {
        UINT16 TCD:1; // 1 0h RW Thermal Controller Disable (TCD): Thermal Controller Disable (TCD): When set, the the Thermal Controller, is disabled
        UINT16 RES_7_1:7; // 7:2 0h RO Reserved.
        UINT16 RES_15_9:7; // 15:9 0h RO Reserved.
    } TCFD; // F0h F0h Thermal Controller Function Disable (TCFD)
} THERMAL_REPORTING_MEMORY_MAPPED_REGISTERS, *PTHERMAL_REPORTING_MEMORY_MAPPED_REGISTERS; // f4


//
//BDF 00:16:00
//BDF 00:16:01
//BDF 00:16:04
//BDF 00:16:05
typedef struct _IMEI_CONFIG {
    PCI_CONFIG_GENERAL Header; // 00h - 3Fh
    struct {
        UINT32 FS_HA:32; // 31:0 0h RO/V Host Firmware Status (FS_HA): Indicates current status of the firmware for the controller. This field is the host's read only access to the FS field in the ME Firmware Status register. This field is reset during CSE partition reset flow.
    } HFSTS1; // 40h 43h Host Firmware Status Register 1 (HFSTS1)
    UINT32 RES_47_44;
    struct {
        UINT32 GSS1:32; // 31:0 0h RO/V Host Firmware Status (GSS1): This field is host side shadow of General Status 1 (CSE_GS1) register.This field is reset during ME partition reset flow.
    } HFSTS2; // 48h 4Bh Host Firmware Status Register 2 (HFSTS2)
    struct {
        UINT32 H_GS1:32; // 31:0 0h RW Host General Status (H_GS1): General status of Host. This field is not used by hardware
    } HECI1_H_GS1; // 4Ch 4Fh Host General Status (HECI1_H_GS1)
    UINT8 RES_5F_50[0x10];
    struct {
        UINT32 GSS2:32; // 31:0 0h RO/V Host Firmware Status (GSS2): This field is host side shadow of ME General Status 2 (CSE_GS2). This field is reset during ME partition reset flow
    } HFSTS3; // 60h 63h Host Firmware Status Register 3 (HFSTS3)
    struct {
        UINT32 GSS3:32; // 31:0 0h RO/V Host Firmware Status (GSS3): This field is host side shadow of CSE General Status 3 (CSE_GS3). This field is reset during ME partition reset flow.
    } HFSTS4; //64h 67h Host Firmware Status Register 4 (HFSTS4)
    struct {
        UINT32 GSS4:32; // 31:0 0h RO/V Host Firmware Status (GSS4): This field is host side shadow of ME General Status 4 (CSE_GS4). This field is reset during ME partition reset flow
    } HFSTS5; //68h 6Bh Host Firmware Status Register 5 (HFSTS5)
    struct {
        UINT32 GSS5:32; // 31:0 0h RO/V Host Firmware Status (GSS5): This field is host side shadow of ME General Status 5 (CSE_GS5). This field is reset during ME partition reset flow.
    } HFSTS6; //6Ch 6Fh Host Firmware Status Register 6 (HFSTS6)
    struct {
        UINT32 H_GS2:32; // 31:0 0h RW Host General Status 2 (H_GS2): General status of Host. This field is not used by hardware.
    } HECI1_H_GS2; //70h 73h Host General Status 2 (HECI1_H_GS2)
    struct {
        UINT32 H_GS3:32; // 31:0 0h RW Host General Status 3 (H_GS3): General status of Host. This field is not used by hardware.
    } HECI1_H_GS3; //74h 77h Host General Status 3 (HECI1_H_GS3)
    UINT8 RES_9F_78[0x28];
    struct {
        UINT32 HIDM:2; // 1:0 0h RW/L HECI Interrupt Delivery Mode (HIDM): These bits control what type of interrupt the controller will send when ME FW writes to set the CSE_IG bit. They are interpreted as follows: 00 - Generate Legacy or MSI interrupt; 01 - Generate SCI; 10 - Generate SMI; This field may be locked by writing 1 to HIDM_L bit
        UINT32 HIDM_L:1; // 2 0h RW/1S/V HIDM Lock (HIDM_L): Writing 1 to this bit locks the HIDM field.
        UINT32 RES_7_3:5; // 7:3 0h RO Reserved.
        UINT32 RES_31_8:24; // 
    } HECI1_HIDM; // A0h A0h Interrupt Delivery Mode (HECI1_HIDM)
} INTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG, *PINTEL_MANAGEMENT_ENGINE_INTERFACE_CONFIG;

typedef struct _HECI_BAR0 {
    UINT32 H_CB_WW; // Host Circular Buffer Write Window H_CB_WW 0 3 00000000h W;
    struct {
        UINT32 H_IE:1; // 0 RW 0b Host Interrupt Enable (H_IE): Host sets this bit to 1 to enable the host interrupt (INTR# or MSI) to be asserted when H_IS is set to 1.
        UINT32 H_IS:1; // 1 RWC 0b Host Interrupt Status (H_IS): HW sets this bit to 1 when ME_IG bit is set to 1. Host clears this bit to 0 by writing a 1 to this bit position. H_IE has no effect on this bit.
        UINT32 H_IG:1; // 2 RW 0b Host Interrupt Generate (H_IG): Once message(s) are written into its CB, the host sets this bit to one for the HW to set the ME_IS bit in the ME_CSR and to generate an interrupt message to ME. HW will send the interrupt message to ME only if the ME_IE is enabled. HWthen clears this bit to 0.
        UINT32 H_RDY:1; // 3 RW 0b Host Ready (H_RDY): This bit indicates that the host is ready toprocess messages.
        UINT32 H_RST:1; // 4 RW 0b Host Reset (H_RST): Setting this bit to 1 will initiate a HECI reset sequence to get the circular buffers into a known good state for host and ME communication. When this bit transitions from 0 to 1, hardware will clear the H_RDY and ME_RDY bits.
        UINT32 RES_7_5:3; // 7:5 RO 000b RSVD (RSVD): Reserved 
        UINT32 H_CBRP:8; // 15:8 RO 00h Host CB Read Pointer (H_CBRP): Points to next location in the H_CB where a valid data is available for embedded controller to read. Software uses this field along with H_CBWR and H_CBD fields to calculate the number of valid entries in the host CB to read or number of entries available for write.
        UINT32 H_CBWP:8; // 23:16 RO 00h Host CB Write Pointer (H_CBWP): Points to next location in the H_CB for host to write the data. Software uses this field along with H_CBRP and H_CBD fields to calculate the number of valid entries in the H_CB to read or number of entries available for write.
        UINT32 H_CBD:8; // 31:24 RO 02h Host Circular Buffer Depth (H_CBD): This field indicates the maximum number of 32 bit entries available in the host circular buffer (H_CB). Host software uses this field along with the H_CBRP and H_CBWP fields to calculate the number of valid entries in the H_CB to read or # of entries available for write. This field is programmed by the ME 
    } H_CSR; // Host Control Status H_CSR 4 7 02000000h RO; RW; RWC;
    UINT32 ME_CB_RW; // ME Circular Buffer Read Window ME_CB_RW 8 B FFFFFFFFh RO;
    struct {
        UINT32 ME_IE_HRA:1; // 0 RO 0b ME Interrupt Enable Host Read Access (ME_IE_HRA): Host read only access to ME_IE.
        UINT32 ME_IS_HRA:1; // 1 RO 0b ME Interrupt Status Host Read Access (ME_IS_HRA): Host read only access to ME_IS.
        UINT32 ME_IG_HRA:1; // 2 RO 0b ME Interrupt Generate Host Read Access (ME_IG_HRA): Host read only access to ME_IG.
        UINT32 ME_RDY_HRA:1; // 3 RO 0b ME Ready Host Read Access (ME_RDY_HRA): Host read access to ME_RDY.
        UINT32 ME_RST_HRA:1; // 4 RO 0b ME Reset Host Read Access (ME_RST_HRA): Host read access to ME_RST.
        UINT32 RES_7_5:1; // 7:5 RO 000b RSVD (RSVD): Reserved
        UINT32 ME_CBRP_HRA:1; // 15:8 RO 00h ME CB Read Pointer Host Read Access (ME_CBRP_HRA): Host read only access to ME_CBRP.
        UINT32 ME_CBWP_HRA:1; // 23:16 RO 00h ME CB Write Pointer Host Read Access (ME_CBWP_HRA): Host read only access to ME_CBWP.
        UINT32 ME_CBD_HRA:1; // 31:24 RO 02h ME Circular Buffer Depth Host Read Access (ME_CBD_HRA): Host read only access to ME_CBD.
    } ME_CSR_HA; // ME Control Status Host Access ME_CSR_HA C F 02000000h RO;
} HECI_BAR0, *PHECI_BAR0; // 


//
//BDF 00:1f:00
typedef struct _RCBA {
    UINT32 Enable:1; // 0 Enable (EN)—R/W. When set, this bit enables the range specified in BA to be claimed as the Root Complex Register Block.
    UINT32 Reserved1:13; // 13:1 Reserved 
    UINT32 BA:18; // 31:14 Base Address (BA)—R/W. Base Address for the root complex register block decode range. This address is aligned on a 16-KB boundary.
} RCBA, *PRCBA; // F0h Root Complex Base Address Register

typedef struct _GIR {
    UINT32 LPC_DECODE_ENABLE:1; // 0 0h RW LPC Decode Enable (LPC_DECODE_ENABLE): When this bit is set to 1, then the range specified in this register is enabled for decoding to LPC.
    UINT32 Reserved1:1; // 1 0h RO Reserved.
    UINT32 ADDRESS:14; // 15:2 0h RW Address[15:2] (ADDRESS_15_2): DWord-aligned address. Note that PCH does not provide decode down to the word or byte level.
    UINT32 Reserved2:2; // 17:16 0h RO Reserved.
    UINT32 ADDRESS_MASK:6; // 23:18 0h RW Address[7:2] Mask (ADDRESS_7_2_MASK): A 1 in any bit position indicates that any value in the corresponding address bit in a received cycle will be treated as a match. The corresponding bit in the Address field, below, is ignored. The mask is only provided for the lower 6 bits of the DWord address, allowing for decoding blocks up to 256 bytes in size.
    UINT32 Reserved3:8; // 31:24 0h RO Reserved.
} GIR, *PGIR;

typedef struct _LPC_CFG {
    PCI_CONFIG_GENERAL Header; // 00h - 3Fh
    UINT8 Reserved1[0x24]; // 40h
    struct {
        UINT8 SFPW:2; // 1:0 0h RW Start Frame Pulse Width (SFPW): This is the number of 33 MHz clocks that the SERIRQ pin will be driven low by the Serial IRQ controller to signal a start frame. In continuous mode, the controller will drive the start frame for the number of clocks specified. In quiet mode, the controller will drive the start frame for the number of clocks specified minus one, as the first clock was driven by the peripheral. Bits Clocks: 00=4, 01=6, 10=8, 11=Reserved
        UINT8 FS:4; // 5:2 4h RO Frame Size (FS): Fixed field that indicates the size of the SERIRQ frame as 21 frames.
        UINT8 MD:1; // 6 0h RW Mode (MD): When set, the serial IRQ machine will be in continuous mode. When cleared, the serial IRQ machine will be in quiet mode. When setting the EN bit, this bit must also be written as a one to guarantee that the first action of the  serial IRQ machine will be a start frame.
        UINT8 EN:1; // 7 0h RW Enable (EN): When set, serial IRQs will be recognized. 
    } SCNT; // 64h 2.1.9 Serial IRQ Control (SCNT)
    UINT8 Reserved2[0x1b]; // 65h
    struct {
        UINT16 CA:3; // 2:0 0h RW ComA Range (CA): The following table describes which range to decode for the COMA Port
                    //Bits Decode Range
                    //000 3F8h - 3FFh (COM 1)
                    //001 2F8h - 2FFh (COM 2)
                    //010 220h - 227h
                    //011 228h - 22Fh
                    //100 238h - 23Fh
                    //101 2E8h - 2EFh (COM 4)
                    //110 338h - 33Fh
                    //111 3E8h - 3EFh (COM 3)
        UINT16 Reserved1:1; // 3 0h RO Reserved.
        UINT16 CB:3; // 6:4 0h RW ComB Range (CB): The following table describes which range to decode for the COMB Port
                    //Bits Decode Range
                    //000 3F8h - 3FFh (COM 1)
                    //001 2F8h - 2FFh (COM 2)
                    //010 220h - 227h
                    //011 228h - 22Fh
                    //100 238h - 23Fh
                    //101 2E8h - 2EFh (COM 4)
                    //110 338h - 33Fh
                    //111 3E8h - 3EFh (COM 3)
        UINT16 Reserved2:1; // 7 0h RO Reserved.
        UINT16 LPT:2; // 9:8 0h RW LPT Range (LPT): The following table describes which range to decode for the LPT Port:
                    //Bits Decode Range
                    //00 378h - 37Fh and 778h - 77Fh
                    //01 278h - 27Fh (port 279h is read only) and 678h - 67Fh
                    //10 3BCh - 3BEh and 7BCh - 7BEh
                    //11 Reserved
        UINT16 Reserved3:2; // 11:10 0h RO Reserved.
        UINT16 FDD:1; // 12 0h RW FDD Range (FDD): The following table describes which range to decode for the  FDD Port
                    // Bits Decode Range
                    // 0 3F0h - 3F5h, 3F7h (Primary)
                    // 1 370h - 375h, 377h (Secondary)
        UINT16 Reserved4:3; // 15:13 0h RO Reserved.
    } IOD; // 80h 2.1.10 I/O Decode Ranges (IOD)
    struct {
        UINT16 CAE:1; // 0 0h RW Com Port A Enable (CAE): Enables decoding of the COMA range to LPC. Range is selected LIOD.CA.
        UINT16 CBE:1; // 1 0h RW Com Port B Enable (CBE): Enables decoding of the COMB range to LPC. Range is selected LIOD.CB.
        UINT16 PPE:1; // 2 0h RW Parallel Port Enable (PPE): Enables decoding of the LPT range to LPC. Range is selected by LIOD.LPT.
        UINT16 DDE:1; // 3 0h RW Floppy Drive Enable (FDE): Enables decoding of the FDD range to LPC. Range is selected by LIOD.FDE
        UINT16 Reserved1:4; // 7:4 0h RO Reserved.
        UINT16 LGE:1; // 8 0h RW Low Gameport Enable (LGE): Enables decoding of the I/O locations 200h to 207h to LPC.
        UINT16 HGE:1; // 9 0h RW High Gameport Enable (HGE): Enables decoding of the I/O locations 208h to 20Fh to LPC.
        UINT16 KE:1; // 10 0h RW Keyboard Enable (KE): Enables decoding of the keyboard I/O locations 60h and 64h to LPC.
        UINT16 ME1:1; // 11 0h RW Microcontroller Enable #1 (ME1): Enables decoding of I/O locations 62h and 66h to LPC.
        UINT16 SE:1; // 12 0h RW SuperI/O Enable (SE): Enables decoding of I/O locations 2Eh and 2Fh to LPC.
        UINT16 ME2:1; // 13 0h RW Microcontroller Enable #2 (ME2): Enables decoding of I/O locations 4Eh and  4Fh to LPC.
        UINT16 Reserved2:2; // 15:14 0h RO Reserved.
    } IOE; // 82h 2.1.11 I/O Enables (IOE)
    GIR LGIR1; // 84h 2.1.12 LPC Generic IO Range 1 (LGIR1)
    GIR LGIR2; // 88h 2.1.13 LPC Generic IO Range 2 (LGIR2)
    GIR LGIR3; // 8Ch 2.1.14 LPC Generic IO Range 3 (LGIR3)
    GIR LGIR4; // 90h 2.1.15 LPC Generic IO Range 4 (LGIR4)
    struct {
        UINT32 S60REN:1; // 0 0h RW SMI on Port 60 Reads Enable (S60REN): When set, a 1 in bit 8 will cause an SMI event.
        UINT32 S60WEN:1; // 1 0h RW SMI on Port 60 Writes Enable (S60WEN): When set, a 1 in bit 9 will cause an SMI event.
        UINT32 S64REN:1; // 2 0h RW SMI on Port 64 Reads Enable (S64REN): When set, a 1 in bit 10 will cause an SMI event.
        UINT32 S64WEN:1; // 3 0h RW SMI on Port 64 Writes Enable (S64WEN): When set, a 1 in bit 11 will cause an SMI event.
        UINT32 Reserved1:1; // 4 0h RO Reserved.
        UINT32 A20PASSEN:1; // 5 0h RW A20Gate Pass-Through Enable (A20PASSEN): When enabled, allows A20GATE sequence Pass-Through function. When enabled, a specific cycle sequence involving writes to port 60h and port 64h does not result in the setting of the SMI
        UINT32 PSTATE:1; // 6 0h RO Pass Through State (PSTATE): This read-only bit indicates that the state machine is in the middle of an A20GATE pass-through sequence. If software needs to reset this bit, it should set Bit 5 0.
        UINT32 SMIATENDPS:1; // 7 0h RW SMI at End of Pass-through Enable (SMIATENDPS): May need to cause SMI at the end of a pass-through. Can occur if an SMI is generated in the middle of a pass through, and needs to be serviced later.
        UINT32 TRAPBY60R:1; // 8 0h RW/1C SMI Caused by Port 60 Read (TRAPBY60R): Indicates if the event occurred. Note that even if the corresponding enable bit is not set in the Bit 0, then this bit will still be active. It is up to the SMM code to use the enable bit to determine the exact cause of the SMI#. Writing a 1 to this bit will clear the latch.
        UINT32 TRAPBY60W:1; // 9 0h RW/1C SMI Caused by Port 60 Write (TRAPBY60W): Indicates if the event occurred. Note that even if the corresponding enable bit is not set in the Bit 1, then this bit will still be active. It is up to the SMM code to use the enable bit to determine the exact cause of the SMI#. Writing a 1 to this bit will clear the latch. Note that the A20Gate Pass-Through Logic allows specific port 60h Writes to complete without setting this bit.
        UINT32 TRAPBY64R:1; // 10 0h RW/1C SMI Caused by Port 64 Read (TRAPBY64R): Indicates if the event occurred. Note that even if the corresponding enable bit is not set in the Bit 2, then this bit will still be active. It is up to the SMM code to use the enable bit to determine the exact cause of the SMI#. Writing a 1 to this bit will clear the latch.
        UINT32 TRAPBY64W:1; // 11 0h RW/1C SMI Caused by Port 64 Write (TRAPBY64W): Indicates if the event occurred. Note that even if the corresponding enable bit is not set in the Bit 3, then this bit will still be active. It is up to the SMM code to use the enable bit to determine the exact cause of the SMI#. Writing a 1 to this bit will clear the latch. Note that the A20Gate Pass-Through Logic allows specific port 64h Writes to complete without setting this bit.
        UINT32 Reserved2:3; // 14:12 0h RO Reserved.
        UINT32 SMIBYENDPS:1; // 15 0h RW/1C SMI Caused by End of Pass-through (SMIBYENDPS): Indicates if the event occurred. Note that even if the corresponding enable bit is not set in the Bit 7, then this bit will still be active. It is up to the SMM code to use the enable bit to determine the exact cause of the SMI#. Writing a 1 to this bit will clear the latch.
        UINT32 Reserved3:16; // 31:16 0h RO Reserved.
    } ULKMC; // 94h 2.1.16 USB Legacy Keyboard/Mouse Control (ULKMC)
    struct {
        UINT32 LMRD_EN:1; // 0 0h RW LPC Memory Range Decode Enable (LMRD_EN): When this bit is set to 1, then the range specified in this register is enabled for decoding to LPC
        UINT32 Reserved1:15; // 15:1 0h RO Reserved.
        UINT32 MA:16; // 31:16 0h RW Memory Address[31:16] (MA_31_16): This field specifies a 64KB memory block anywhere in the 4GB memory space that will be decoded to LPC as standard LPC Memory Cycle if enabled.
    } LGMR; // 98h 2.1.17 LPC Generic Memory Range (LGMR)
    UINT8 Reserved3[0x34]; // 9C
    struct {
        UINT32 IC0:4; // 3:0 3h RW C0-C7 IDSEL (IC0): IDSEL to use in FWH cycle for range enabled by BDE.EC0.
        UINT32 IC8:4; // 7:4 3h RW C8-CF IDSEL (IC8): IDSEL to use in FWH cycle for range enabled by BDE.EC8.
        UINT32 ID0:4; // 11:8 2h RW D0-D7 IDSEL (ID0): IDSEL to use in FWH cycle for range enabled by BDE.ED0.
        UINT32 ID8:4; // 15:12 2h RW D8-DF IDSEL (ID8): IDSEL to use in FWH cycle for range enabled by BDE.ED8.
        UINT32 IE0:4; // 19:16 1h RW E0-E7 IDSEL (IE0): IDSEL to use in FWH cycle for range enabled by BDE.EE0.
        UINT32 IE8:4; // 23:20 1h RW E8-EF IDSEL (IE8): IDSEL to use in FWH cycle for range enabled by BDE.EE8.
        UINT32 IF0:4; // 27:24 0h RW F0-F7 IDSEL (IF0): IDSEL to use in FWH cycle for range enabled by BDE.EF0.
        UINT32 IF8:4; // 31:28 0h RO F8-FF IDSEL (IF8): IDSEL to use in FWH cycle for range enabled by BDE.EF8.
    } FS1; // D0h 2.1.18 FWH ID Select 1 (FS1)
    struct {
        UINT16 I40:4; // 3:0 7h RW 40-4F IDSEL (I40): IDSEL to use in FWH cycle for range enabled by BDE.E4
        UINT16 I50:4; // 7:4 6h RW 50-5F IDSEL (I50): IDSEL to use in FWH cycle for range enabled by BDE.E50.
        UINT16 I60:4; // 11:8 5h RW 60-6F IDSEL (I60): IDSEL to use in FWH cycle for range enabled by BDE.E60.
        UINT16 I70:4; // 15:12 4h RW 70-7F IDSEL (I70): IDSEL to use in FWH cycle for range enabled by BDE.E70.
    } FS2; // D4h 2.1.19 FWH ID Select 2 (FS2)
    UINT16 Reserved4; // D6h
    struct {
        UINT32 E40:1; // 0 1b RW 40-4F Enable (E40) Enables decoding of 1MB of the following BIOS ranges:
                    //FF400000h - FF4FFFFFh
                    //FF000000h - FF0FFFFFh
        UINT32 E50:1; // 1 1b RW 50-5F Enable (E50) Enables decoding of 1MB of the following BIOS ranges:
                    //FF500000h - FF5FFFFFh
                    //FF100000h - FF1FFFFFh
        UINT32 E60:1; // 2 1b RW 60-6F Enable (E60) Enables decoding of 1MB of the following BIOS ranges:
                    //FF600000h - FF6FFFFFh
                    //FF200000h - FF2FFFFFh
        UINT32 E70:1; // 3 1b RW 70-7F Enable (E70) Enables decoding of 1MB of the following BIOS ranges:
                    //FF700000h - FF7FFFFFh
                    //FF300000h - FF3FFFFFh
        UINT32 Reserved1:2; //5:4 - - Reserved
        UINT32 LEE:1; // 6 1b RW Legacy E Segment Enable (LEE) Legacy E Segment Enable (LFE): This enables the decoding of the
                     //legacy 64KB range at E0000h - EFFFFh
        UINT32 LFE:1; // 7 1b RW Legacy F Segment Enable (LFE) Legacy F Segment Enable (LFE): This enables the decoding of the
                    //legacy 64KB range at F0000h - FFFFFh
        UINT32 EC0:1; // 8 1b RW C0-C7 Enable (EC0) Enables decoding of 512K of the following BIOS ranges:
                    //FFC00000h - FFC7FFFFh
                    //FF800000h - FF87FFFFh
        UINT32 EC8:1; // 9 1b RW C8-CF Enable (EC8) Enables decoding of 512K of the following BIOS ranges:
                    //FFC80000h - FFCFFFFFh
                    //FF880000h - FF8FFFFFh
        UINT32 ED0:1; // 10 1b RW D0-D7 Enable (ED0) Enables decoding of 512K of the following BIOS ranges:
                    //FFD00000h - FFD7FFFFh
                    //FF900000h - FF97FFFFh
        UINT32 ED8:1; // 11 1b RW D8-DF Enable (ED8) Enables decoding of 512K of the following BIOS ranges:
                    //FFD80000h - FFDFFFFFh
                    //FF980000h - FF9FFFFFh
        UINT32 EE0:1; // 12 1b RW E0-E8 Enable (EE0) Enables decoding of 512K of the following BIOS ranges:
                    //FFE00000h - FFE7FFFFh
                    //FFA00000h - FFA7FFFFh
        UINT32 EE8:1; // 13 1b RW E8-EF Enable (EE8) Enables decoding of 512K of the following BIOS ranges:
                    //FFE80000h - FFEFFFFFh
                    //FFA80000h - FFAFFFFFh
        UINT32 EF0:1; // 14 1b RW F0-F8 Enable (EF0) Enables decoding of 512K of the following BIOS ranges:
                    //FFF00000h - FFF7FFFFh
                    //FFB00000h - FFB7FFFFh
        UINT32 EF8:1; // 15 1b RO F8-FF Enable (EF8) Enables decoding of 512K of the following BIOS ranges:
                    //FFF80000h - FFFFFFFFh
                    //FFB80000h - FFBFFFFFh
        UINT32 Reserved2:16; //31:16 - - Reserved
    } BDE; // D8h 2.1.20 BIOS Decode Enable (BDE)
    struct {
        UINT8 WPD:1; // 0 0h RW Write Protect Disable (WPD): When set, access to the BIOS space is enabled for both read and write cycles to BIOS. When cleared, only read cycles are permitted to the FWH or SPI flash. When this bit is written from a 0 to a 1 and the LE bit is also set, an SMI# is generated. This ensures that only SMM code can update BIOS.
        UINT8 LE:1; // 1 0h RW/1L Lock Enable (LE): When set, setting the WP bit will cause SMI. When cleared, setting the WP bit will not cause SMI. Once set, this bit can only be cleared by a PLTRST#. When this bit is set, EISS - bit [5] of this register is locked down.
        UINT8 Reserved1:2; // 3:2 0h RO Reserved.
        UINT8 TS:1; // 4 0h RO Top Swap (TS): When set, PCH will invert either A16, A17, A18, A19 or A20 for cycles going to the BIOS space (but not the feature space). When cleared, PCH will not invert the lines. If booting from LPC (FWH), then the Boot Blook size is 64KB and A16 is inverted if Top Swap is enabled. If booting from SPI, then the BOOT_BLOCK_SIZE soft strap determines if A16, A17, A18, A19 or A20 should be inverted if Top Swap is enabled. *If PCH is strapped for Top-Swap is low at rising edge of PWROK, then this bit cannot be cleared by software. The strap jumper should be removed and the system rebooted. 
                //BIOS Note: 
                //1) This bit provides a read-only path to view the state of the Top Swap strap. It is backed up and driven from the RTC well. Bios will need to program the corresponding register in the RTC Controller (in RTC well), which will be reflected in this register. 
                //2) The Register portion of the Top Swap is lockable by the Bios Interface Lockdown Bit (BC.BILD)
        UINT8 EISS:1; // 5 1h RW/L Enable InSMM.STS (EISS): When this bit is set, the BIOS region is not writable until SMM sets the InSMM.STS bit. Today BIOS Flash is writable if WPD is a 1.If this bit [5] is set, then WPD must be a 1 and InSMM.STS (0xFED3_0880[0]) must be 1 also. If this bit [5] is clear, then BIOS is writable based only on WPD = 1 and the InSMM.STS is a dont care.
        UINT8 BBS:1; // 6 0h RW/L Boot BIOS Destination (BBS): This field determines the destination of accesses to the BIOS memory range. For the default, Functional Strap section of Signal Description chapter for details.
                    //0: SPI
                    //1: LPC 
                    //When SPI or LPC is selected, the range that is decoded is further qualified by other configuration bits described in the respective sections. The value in this field can be overwritten by software as long as the BIOS Interface Lock-Down is not set.
        UINT8 BILD:1; // 7 0h RW/1L BIOS Interface Lock-Down (BILD): When set, prevents BC.TS and BC. BBS from being changed. This bit can only be written from 0 to 1 once. BIOS Note: This bit is not backed up in the RTC well. This bit should also be set in the BUC register in the RTC device to record the last state of this value following a cold reset.
    } BC; // DCh 2.1.21 BIOS Control (BC)
    UINT8 Reserved5[0x3]; // DDh - DFh
    struct {
        UINT32 CLKRUN_EN:1; // 0 0h RW Clock Run Enable (CLKRUN_EN): Enables the CLKRUN# logic to stop the LPC clocks. If the SLP_EN bit is set, then the Intel PCH will drive CLKRUN# low. This will keep the LPC and LPC clocks running on the way to the sleeping state. This is required to meet an LPC specification. This does not necessarily mean that the CLKRUN_EN bit is forced low when SLP_EN is set. Even though the CLKRUN# signal will be low when SLP_EN is set, the state of the CLKRUN_EN bit is ignored when SLP_EN bit is set. This gives flexibility in the implementation.
        UINT32 Reserved1:1; // 1 0h RO Reserved
        UINT32 PCLKVLD_CFG:2; // 3:2 0h RW LPC Clock Valid Configuration (PCLKVLD_CFG): This field determines the relationship between the internally broadcast indication of the external LPC clock being valid vs. the STP_PCI# pin. 
                //Encodings: 
                //00: 1 flop stage of delay from STP_PCI# (default)
                //01: No delay (edges match STP_PCI#
                //10: 2 flop stages of delay from STP_PCI#
                //11: Tie high (indicate that LPC clock is always valid)
        UINT32 STP_PCI_OVR:1; // 4 0h RO/V Stop PCI# Override (STP_PCI_OVR): When set to 1, Firmware is in control of the STP_PCI# and the value in STP_PCI_VAL will be propagated to the internal STP_PCI#. When this bit is '0', HW will determine the value of the pin.Note:Bios cannot control the STP_PCI# pin while PLTRST# is asserted (the pin will be at its reset default value).
        UINT32 STP_PCI_VAL:1; // 5 0h RO/V Stop PCI# Value (STP_PCI_VAL): Either Hardware or Software may own control of the internal STP_PCI#. This bit provides the value to drive on the STP_PCI# if STP_PCI_OVR is set to 1. Note: SW cannot control the STP_PCI# pin while PLTRST# is asserted (the pin will be at its reset default value).
        UINT32 CLKRUN_VAL:1; // 6 0h RO/V CLKRUN# Pin Output Value (CLKRUN_VAL): Either HW or SW may own control of the CLKRUN# pin. This bit provides the value to drive on the pin if CLKRUN_OVR is set to '1'.
        UINT32 CLKRUN_EN_VAL:1; // 7 0h RO/V CLKRUN# Buffer Enable Value (CLKRUN_EN_VAL): Either HW or SW may own control of the CLKRUN# pin. This bit provides the value to drive on the active low CLKRUN# buffer enable if CLKRUN_EN_OVR is set to '1'.
        UINT32 CLKRUN_OVR:1; // 8 0h RO/V CLKRUN# Override (CLKRUN_OVR): When set to '1', SW is in control of the CLKRUN# pin and the value in CLKRUN_VAL will be propagated to the output pin. When this bit is '0', HW will determine the value of the pin.
        UINT32 CLKRUN_EN_OVR:1; // 9 0h RO/V CLKRUN# Buffer Enable Override (CLKRUN_EN_OVR): When set to '1', SW is in control of the CLKRUN# buffer enable and the value in CLKRUN_EN_VAL will be propagated to the output buffer enable. When this bit is '0', HW will determine the value of the buffer enable.
        UINT32 Reserved2:22; // 31:10 0h RO Reserved.
    } PCCTL; // E0h 2.1.22 PCI Clock Control (PCCTL)
    UINT8 Reserved6[0xC]; // E4h - EFh 
    RCBA RCBA; // F0h Root Complex Base Address Register
} LPC_CFG, *PLPC_CFG;


//
// BDF 00:1f:01
typedef struct _PS2B {
    PCI_CONFIG_GENERAL Header; // 00h - 3Fh
    UINT8 RES_4F_40[0x10];
    struct {
        UINT16 FUNC:3; // 2:0 0h RW Function Number (FUNC): VLW Function Number
        UINT16 DEV:5; // 7:3 1Fh RW Device Number (DEV): VLW Device Number
        UINT16 BUS:8; // 15:8 0h RW Bus Number (BUS): VLW Bus Number
    } VBDF; //50h 51h VLW Bus:Device:Function (VBDF)
    struct {
        UINT16 FUNC:3; // 2:0 0h RW Function Number (FUNC): Error Function Number
        UINT16 DEV:5; // 7:3 1Fh RW Device Number (DEV): Error Device Number
        UINT16 BUS:8; // 15:8 0h RW Bus Number (BUS): Error Bus Number
    } EBDF; //52h 53h ERROR Bus:Device:Function (EBDF)
    struct {
        UINT32 RSE:1; // 0 0h RW RTC Shadow Enable (RSE): When set, all IO writes to the RTC will be also sent to the PMC. This allows cases where the battery backed storage is in an external PMIC.
        UINT32 RES_7_1:7; // 7:1 0h RO Reserved.
        UINT32 RPRID:8; // 15:8 C7h RW Reserved Page Register Destination ID (RPRID): Specifies the IOSF-SB destination ID for sending Reserved Page Register cycles (e.g. Port 80h). By default this will load to the ID of the LPC or eSPI device depending on which has been strapped active in the system.
        UINT32 RES_31_16:16; // 31:16 0h RO Reserved.
    } RCFG; //54h 57h Routing Configuration (RCFG)
    struct {
        UINT32 AS:2; // 1:0 0h RW Address Select (AS): This 2-bit field selects 1 of 4 possible memory address ranges for the High Performance Timer functionality. The encodings are:
                    //00 : FED0_0000h - FED0_03FFFh
                    //01 : FED0_1000h - FED0_13FFFh
                    //10 : FED0_2000h - FED0_23FFFh
                    //11 : FED0_3000h - FED0_33FFFh
        UINT32 RES_6_2:5; // 6:2 0h RO Reserved.
        UINT32 AE:1; // 7 0h RW Address Enable (AE): When set, the P2SB will decode the High Performance Timer memory address range selected by bits 1:0 below.
    } HPTC; //60h 60h High Performance Event Timer Configuration (HPTC)
    struct {
        UINT16 ASEL:8; // 7:0 0h RW APIC Range Select (ASEL): These bits define address bits 19:12 for the IOxAPIC range. The default value of 00h enables compatibility with prior products as an initial value. This value must not be changed unless the IOxAPIC Enable bit is cleared.
        UINT16 AE:1; // 8 0h RW Address Enable (AE): When set, the P2SB will decode the IOxAPIC memory address range selected by bits 7:0 below.
        UINT16 RES_15_9:7; // 15:9 0h RO Reserved.
    } IOAC; //64h 65h IOxAPIC Configuration (IOAC)
    UINT8 RES_6B_66[0x6];
    struct {
        UINT16 FUNC:3; // 2:0 0h RW Function Number (FUNC): IOxAPIC Function Number
        UINT16 DEV:5; // 7:3 1Fh RW Device Number (DEV): IOxAPIC Device Number
        UINT16 BUS:8; // 15:8 0h RW Bus Number (BUS): IOxAPIC Bus Number
    } IBDF; //6Ch 6Dh IOxAPIC Bus:Device:Function (IBDF)
    UINT8 RES_6F_6E[0x2];
    struct {
        UINT16 FUNC:3; // 2:0 0h RW Function Number (FUNC): HPET Function Number
        UINT16 DEV:5; // 7:3 1Fh RW Device Number (DEV): HPET Device Number
        UINT16 BUS:8; // 15:8 0h RW Bus Number (BUS): HPET Bus Number
    } HBDF; //70h 71h HPET Bus:Device:Function (HBDF)
    UINT8 RES_BF_72[0x4E];
    struct {
        UINT32 FUNC:3; // 2:0 0h RW Function Number (FUNC): HPET Function Number
        UINT32 DEV:5; // 7:3 1Fh RW Device Number (DEV): HPET Device Number
        UINT32 BUS:8; // 15:8 0h RW Bus Number (BUS): HPET Bus Number
        UINT32 DTBLK:3; // 18:16 6h RW Display Target Block (DTBLK): This register contains the Target BLK field that will be used when sending RAVDM messages to the CPU Complex North Display.
        UINT32 RES_31_19:13; // 31:19 0h RO Reserved.
    } DISPBDF; //C0h C3h Display Bus:Device:Function (DISPBDF)
    struct {
        UINT16 BUFBASE:8; // 7:0 0h RW Buffer Address Offset (BUFBASE): This specifies the upper 8b for the 16b address that will be used for sending RAVDM access that target the Buffer range of the ICC (FFE00h - FFEFFh).
        UINT16 MODBASE:8; // 15:8 0h RW Modulator Control Address Offset (MODBASE): This specifies the upper 8b for the 16b address that will be used for sending RAVDM access that target the Modulator Control range of the ICC (FFF00h - FFFFFh).
    } ICCOS; //C4h C5h ICC Register Offsets (ICCOS)
    UINT8 RES_CF_C6[0xA];
    struct {
        UINT32 OFFSET:16; // 15:0 0h RW Address Offset (OFFSET): Register address offset. The content of this register field is sent in the IOSF Sideband Message Register Access address(15:0) field
        UINT32 RES_23_16:8; // 23:16 0h RO Reserved.
        UINT32 DESTID:8; // 31:24 0h RW Destination Port ID (DESTID): The content of this register field is sent in the IOSF  Sideband Message Register Access dest field.
    } SBIADDR; //D0h D3h SBI Address (SBIADDR)
    struct {
        UINT32 DATA:32; // 31:0 0h RW/V Data (DATA): The content of this register field is sent on the IOSF sideband Message Register Access data(31:0) field
    } SBIDATA; // D4h D7h SBI Data (SBIDATA)
    struct {
        UINT16 INITRDY:1; // 0 0h RW/1S Initiate/ Ready# (INITRDY): 0: The IOSF sideband interface is ready for a new transaction
                  // 1: The IOSF sideband interface is busy with the previous transaction. A write to set this register bit to 1 will trigger an IOSF sideband message on the private IOSF sideband interface. The message will be formed based on the values programmed in the Sideband Message Interface Register Access registers. Software needs to ensure that the interface is not busy (SBISTAT.INITRDY is clear) before writing to this register.
        UINT16 RESPONSE:2; // 2:1 0h RW/V Response Status (RESPONSE): 00 - Successful
                  // 01 - Unsuccessful / Not Supported
                  // 10 - Powered Down
                  // 11 - Multi-cast Mixed 
                  // This register reflects the response status for the previously completed transaction. The value of this register is only meaningful if SBISTAT.INITRDY is zero.
        UINT16 RES_6_3:4; // 6:3 0h RO Reserved.
        UINT16 POSTED:1; // 7 0h RW Posted (POSTED): When set to 1, the message will be sent as a posted message  instead of non-posted. This should only be used if the receiver is known to support  posted operations for the specified operation.
        UINT16 OPCODE:8; // 15:8 0h RW Opcode (OPCODE): This is the Opcode sent in the IOSF sideband message.
    } SBISTAT; //D8h D9h SBI Status (SBISTAT)
    struct {
        UINT16 FID:8; // 7:0 0h RW Function ID (FID): The contents of this field are sent in the IOSF Sideband Register access FID field. This field should generally remain at zero unless specifically required by a particular application
        UINT16 BAR:3; // 10:8 0h RW Base Address Register (BAR): The contents of this field are sent in the IOSF  Sideband Register Access BAR field. This should be zero performing a Memory  Mapped operation to a PCI compliant device.
        UINT16 RES_11:1; // 11 0h RO Reserved.
        UINT16 FBE:4; // 15:12 0h RW First Byte Enable (FBE): The content of this field is sent in the IOSF Sideband Register Access FBE field.
    } SBIRID; //DAh DBh SBI Routing Identification (SBIRID)
    struct {
        UINT32 ADDR:32; // 31:0 0h RW Extended Address (ADDR): The content of this register field is sent on the IOSF sideband Message Register Access address(48:32) field. This must be set to all 0 if 16b addressing is desired.
    } SBIEXTADDR; //DCh DFh SBI Extended Address (SBIEXTADDR)
    UINT32 P2SBC; //E0h E3h P2SB Control (P2SBC)
    UINT8 PCE; //E4h E4h Power Control Enable (PCE)
    UINT8 RES_1FF_E5[0x11B]; 
    UINT32 SBREGPOSTED0; //200h 203h Sideband Register Posted 0 (SBREGPOSTED0)
    UINT32 SBREGPOSTED1; //204h 207h Sideband Register Posted 1 (SBREGPOSTED1)
    UINT32 SBREGPOSTED2; //208h 20Bh Sideband Register Posted 2 (SBREGPOSTED2)
    UINT32 SBREGPOSTED3; //20Ch 20Fh Sideband Register Posted 3 (SBREGPOSTED3)
    UINT32 SBREGPOSTED4; //210h 213h Sideband Register Posted 4 (SBREGPOSTED4)
    UINT32 SBREGPOSTED5; //214h 217h Sideband Register Posted 5 (SBREGPOSTED5)
    UINT32 SBREGPOSTED6; //218h 21Bh Sideband Register Posted 6 (SBREGPOSTED6)
    UINT32 SBREGPOSTED7; //21Ch 21Fh Sideband Register Posted 7 (SBREGPOSTED7)
    UINT32 EPMASK0; //220h 223h Endpoint Mask 0 (EPMASK0)
    UINT32 EPMASK1; //224h 227h Endpoint Mask 1 (EPMASK1)
    UINT32 EPMASK2; //228h 22Bh Endpoint Mask 2 (EPMASK2)
    UINT32 EPMASK3; //22Ch 22Fh Endpoint Mask 3 (EPMASK3)
    UINT32 EPMASK4; //230h 233h Endpoint Mask 4 (EPMASK4)
    UINT32 EPMASK5; //234h 237h Endpoint Mask 5 (EPMASK5)
    UINT32 EPMASK6; //238h 23Bh Endpoint Mask 6 (EPMASK6)
    UINT32 EPMASK7; //23Ch 23Fh Endpoint Mask 7 (EPMASK7)
} PS2_BRIDGE_CFG, *PPS2_BRIDGE_CFG;


//
//BDF 00:1f:02
typedef struct _PMC {
    PCI_CONFIG_GENERAL Header; // 00h - 3Fh
    UINT8 RES_79_40[0x40];
    UINT32 POWERCAPID; // 80h 83h Power Management Capability ID (POWERCAPID) 48000001h
    UINT32 PMECTRLSTATUS; // 84h 87h PME Control Status (PMECTRLSTATUS) 8h
    UINT8 RES_8F_88[0x8];
    //UINT8 RES_8F_40[0x50];
    UINT32 PCIDEVIDLE_CAP_RECORD; // 90h 93h PCI Device Idle Capability Record (PCIDEVIDLE_CAP_RECORD) F0140009h 
    UINT32 RES_97_94;
    UINT32 D0I3_CONTROL_SW_LTR_MMIO_REG; // 98h 9Bh SW LTR Update MMIO Location (D0I3_CONTROL_SW_LTR_MMIO_REG) 0h
    UINT32 DEVICE_IDLE_POINTER_REG; // 9Ch 9Fh Device IDLE pointer register (DEVICE_IDLE_POINTER_REG) 0h 
    UINT32 D0I3_MAX_POW_LAT_PG_CONFIG; // A0h A3h D0I3_MAX_POW_LAT_PG_CONFIG (D0I3_MAX_POW_LAT_PG_CONFIG) 800h
} PMC_CONTROLLER_CFG, *PPMC_CONTROLLER_CFG;

typedef struct _PMC_IO_BASED_REGISTERS {
    UINT32 PM1_EN_STS; // 0h 3h Power Management 1 Enables and Status (PM1_EN_STS) 0h
    UINT32 PM1_CNT; // 4h 7h Power Management 1 Control (PM1_CNT) 0h
    UINT32 PM1_TMR; // 8h Bh Power Management 1 Timer (PM1_TMR) 0h
    UINT32 RES_0F_0C;
    UINT32 SMI_EN; // 30h 33h SMI Control and Enable (SMI_EN) 2h
    UINT32 SMI_STS; // 34h 37h SMI Status Register (SMI_STS) 0h
    UINT32 RES_1B_18;
    UINT32 RES_1F_1C;
    UINT32 GPE_CTRL; // 40h 43h General Purpose Event Control (GPE_CTRL) 0h
    UINT32 DEVACT_STS; // 44h 47h Device Activity Status Register (DEVACT_STS) 0h
    UINT32 RES_4B_48;
    UINT32 RES_4F_4C;
    UINT32 PM2A_CNT_BLK; // 50h 53h PM2a Control Block (PM2A_CNT_BLK) h 0h
    UINT32 OC_WDT_CTL; // 54h 57h Over-Clocking WDT Control (OC_WDT_CTL) 2000h
    UINT32 RES_5B_58;
    UINT32 RES_5F_5C;
    UINT32 GPE0_STS_31_0; // 60h 63h General Purpose Event 0 Status [31:0] (GPE0_STS_31_0) 0h
    UINT32 GPE0_STS_63_32; // 64h 67h General Purpose Event 0 Status [63:32] (GPE0_STS_63_32) 0h
    UINT32 GPE0_STS_95_64; // 68h 6Bh General Purpose Event 0 Status [95:64] (GPE0_STS_95_64) h 0h
    UINT32 GPE0_STS_127_96; // 6Ch 6Fh General Purpose Event 0 Status [127:96] (GPE0_STS[127:96]) 0h
    UINT32 GPE0_EN_31_0; // 70h 73h General Purpose Event 0 Enable [31:0] (GPE0_EN_31_0)
    UINT32 GPE0_EN_63_32; // 74h 77h General Purpose Event 0 Enable [63:32] (GPE0_EN_63_32) 0h
    UINT32 GPE0_EN_95_64; // 78h 7Bh General Purpose Event 0 Enable [95:64] (GPE0_EN_95_64) 0h
    UINT32 GPE0_EN_127_96; // 7Ch 7Fh General Purpose Event 0 Enable [127:96] (GPE0_EN[127:96]) 0h
} PMC_IO_BASED_REGISTERS, *PPMC_IO_BASED_REGISTERS;

typedef struct _PMC_MEMORY_MAPPED_REGISTERS {
    UINT8 RES_101F_00[0x1020]; // 101F 0
    UINT32 GEN_PMCON_A; // 1020h 1023h General PM Configuration A (GEN_PMCON_A)
    UINT32 GEN_PMCON_B; // 1024h 1027h General PM Configuration B (GEN_PMCON_B)
    UINT8 RES_102F_1028[0x8]; // 102F 1028
    UINT32 CRID; // 1030h 1033h Configured Revision ID (CRID)
    UINT8 RES_1047_1034[0x14]; // 1047 1034
    UINT32 ETR3; // 1048h 104Bh Extended Test Mode Register 3 (ETR3)
    UINT32 SSML; // 104Ch 104Fh SET_STRAP_MSG_LOCK (SSML)
    UINT32 SSMC; // 1050h 1053h SET_STRAP_MSG_CONTROL (SSMC)
    UINT32 SSMD; // 1054h 1057h SET_STRAP_MSG_DATA (SSMD)
    UINT8 RES_10AF_1058[0x58]; // 
    UINT32 CRID_UIP; // 10B0h 10B3h Configured Revision ID (CRID_UIP)
    UINT32 SLP_S0_DBG_0; // 10B4h 10B7h SLP_S0# Debug 0 (SLP_S0_DBG_0)
    UINT32 SLP_S0_DBG_1; // 10B8h 10BBh SLP_S0# Debug 1 (SLP_S0_DBG_1)
    UINT32 SLP_S0_DBG_2; // 10BCh 10BFh SLP_S0# Debug 2 (SLP_S0_DBG_2)
    UINT32 MODPHY_PM_CFG1; // 10C0h 10C3h ModPhy Power Management Configuration 1 (MODPHY_PM_CFG1)
    UINT32 MODPHY_PM_CFG2; // 10C4h 10C7h MODPHY Power Management Configuration 2 (MODPHY_PM_CFG2)
    UINT32 MODPHY_PM_CFG3; // 10C8h 10CBh MODPHY Power Management Configuration 3 (MODPHY_PM_CFG3)
    UINT32 MODPHY_PM_CFG4; // 10CCh 10CFh MODPHY Power Management Configuration 4 (MODPHY_PM_CFG4)
    UINT32 MODPHY_PM_CFG5; // 10D0h 10D3h MODPHY Power Management Configuration Reg 5 (MODPHY_PM_CFG5)
    UINT32 MODPHY_PM_CFG6; // 10D4h 10D7h MODPHY Power Management Configuration Reg 6 (MODPHY_PM_CFG6)
    //UINT32 CIR3E0; // 10E0h 10E3h Chipset Initialization Register 3E0 (CIR3E0)
    //UINT32 CIR3E4; // 10E4h 10E7h Chipset Initialization Register 3E4 (CIR3E4)
    //UINT32 ARTV_31_0; // 1200h 1203h Always Running Timer Value 31:0 (ARTV_31_0)
    //UINT32 ARTV_63_32; // 1204h 1207h Always Running Timer Value 31:0 (ARTV_63_32)
    //UINT32 TGPIOCTL0; // 1210h 1213h Timed GPIO Control 0 (TGPIOCTL0)
    //UINT32 TGPIOCOMPV0_31_0; // 1220h 1223h Timed GPIO 0 Comparator Value 31:0 (TGPIOCOMPV0_31_0)
    //UINT32 TGPIOCOMPV0_63_32; // 1224h 1227h Timed GPIO Comparator Value 63:32 (TGPIOCOMPV0_63_32)
    //UINT32 TGPIOPIV0_31_0; // 1228h 122Bh Timed GPIO0 Periodic Interval Value 31_0 (TGPIOPIV0_31_0)
    //UINT32 TGPIOPIV0_63_32; // 122Ch 122Fh Timed GPIO 0 Periodic Interval Value 63_32 (TGPIOPIV0_63_32)
    //UINT32 TGPIOTCV0_31_0; // 1230h 1233h Timed GPIO Time Capture Register 31_0 (TGPIOTCV0_31_0)
    //UINT32 TGPIOTCV0_63_32; // 1234h 1237h Timed GPIO0 Time Capture Register 63_32 (TGPIOTCV0_63_32)
    //UINT32 TGPIOECCV0_31_0; // 1238h 123Bh Timed GPIO0 Event Counter Capture Register 31_0 (TGPIOECCV0_31_0)
    //UINT32 TGPIOECCV0_63_32; // 123Ch 123Fh Timed GPIO0 Event Counter Capture Register 63_32 (TGPIOECCV0_63_32)
    //UINT32 TGPIOEC0_31_0; // 1240h 1243h Timed GPIO0 Event Counter Register 31_0 (TGPIOEC0_31_0)
    //UINT32 TGPIOEC0_63_32; // 1244h 1247h Timed GPIO0 Event Counter Register 63_32 (TGPIOEC0_63_32)
    //UINT32 TGPIOCTL1; // 1310h 1313h Timed GPIO Control 1 (TGPIOCTL1)
    //UINT32 TGPIOCOMPV1_31_0; // 1320h 1323h Timed GPIO 1 Comparator Value 31:0 (TGPIOCOMPV1_31_0)
    //UINT32 TGPIOCOMPV1_63_32; // 1324h 1327h Timed GPIO Comparator Value 63:32 (TGPIOCOMPV1_63_32)
    //UINT32 TGPIOPIV1_31_0; // 1328h 132Bh Timed GPIO1 Periodic Interval Value 31_0 (TGPIOPIV1_31_0)
    //UINT32 TGPIOPIV1_63_32; // 132Ch 132Fh Timed GPIO 1 Periodic Interval Value 63_32 (TGPIOPIV1_63_32)
    //UINT32 TGPIOTCV1_31_0; // 1330h 1333h Timed GPIO Time Capture Register 31_0 (TGPIOTCV1_31_0)
    //UINT32 TGPIOTCV1_63_32; // 1334h 1337h Timed GPIO Time Capture Register 63_32 (TGPIOTCV1_63_32)
    //UINT32 TGPIOECCV1_31_0; // 1338h 133Bh Timed GPIO0 Event Counter Capture Register 31_0 (TGPIOECCV1_31_0)
    //UINT32 TGPIOECCV1_63_32; // 133Ch 133Fh Timed GPIO0 Event Counter Capture Register 63_32 (TGPIOECCV1_63_32)
    //UINT32 TGPIOEC1_31_0; // 1340h 1343h Timed GPIO1 Event Counter Register 31_0 (TGPIOEC1_31_0)
    //UINT32 TGPIOEC1_63_32; // 1344h 1347h Timed GPIO Event Counter Register 63_32 (TGPIOEC1_63_32)
    //UINT32 ART_RTC_RATIO; // 1670h 1673h ART to RTC Ratio (ART_RTC_RATIO)
    //UINT32 WADT_AC; // 1800h 1803h Wake Alarm Device Timer: AC (WADT_AC)
    //UINT32 WADT_DC; // 1804h 1807h Wake Alarm Device Timer: DC (WADT_DC)
    //UINT32 WADT_EXP_AC; // 1808h 180Bh Wake Alarm Device Expired Timer: AC (WADT_EXP_AC)
    //UINT32 WADT_EXP_DC; // 180Ch 180Fh Wake Alarm Device Expired Timer: DC (WADT_EXP_DC)
    //UINT32 PRSTS; // 1810h 1813h Power and Reset Status (PRSTS)
    //UINT32 PM_CFG; // 1818h 181Bh Power Management Configuration Reg 1 (PM_CFG)
    //UINT32 PCH_PM_STS2; // 1824h 1827h PCH Power Management Status (PCH_PM_STS2)
    //UINT32 S3_PWRGATE_POL; // 1828h 182Bh S3 Power Gating Policies (S3_PWRGATE_POL)
    //UINT32 S4_PWRGATE_POL; // 182Ch 182Fh S4 Power Gating Policies (S4_PWRGATE_POL)
    //UINT32 S5_PWRGATE_POL; // 1830h 1833h S5 Power Gating Policies (S5_PWRGATE_POL)
    //UINT32 DSX_CFG; // 1834h 1837h DeepSx Configuration (DSX_CFG)
    //UINT32 PM_CFG2; // 183Ch 183Fh Power Management Configuration Reg 2 (PM_CFG2)
    //UINT32 CIR48; // 1848h 184Bh Chipset Initialization Register 48 (CIR48)
    //UINT32 CIR4C; // 184Ch 184Fh Chipset Initialization Register 4C (CIR4C)
    //UINT32 CIR50; // 1850h 1853h Chipset Initialization Register 50 (CIR50)
    //UINT32 CIR54; // 1854h 1857h Chipset Initialization Register 54 (CIR54)
    //UINT32 CIR58; // 1858h 185Bh Chipset Initialization Register 58 (CIR58)
    //UINT32 CIR68; // 1868h 186Bh Chipset Initialziation Register 68 (CIR68)
    //UINT32 CIR80; // 1880h 1883h Chipset Initialization Register 80 (CIR80)
    //UINT32 CIR84; // 1884h 1887h Chipset Initialization Register 84 (CIR84)
    //UINT32 CIR88; // 1888h 188Bh Chipset Initialization Register 88 (CIR88)
    //UINT32 CIR8C; // 188Ch 188Fh Chipset Initialization Register 8C (CIR8C)
    //UINT32 CIR98; // 1898h 189Bh Chipset Initialization Register 98 (CIR98)
    //UINT32 CIRA8; // 18A8h 18ABh Chipset Initizaliation Register A8 (CIRA8)
    //UINT32 CIRAC; // 18ACh 18AFh Chipset Initizaliation Register AC (CIRAC)
    //UINT32 CIRB0; // 18B0h 18B3h Chipset Initialization Register B0 (CIRB0)
    //UINT32 CIRB4; // 18B4h 18B7h Chipset Initialization Register B4 (CIRB4)
    //UINT32 CIRC0; // 18C0h 18C3h Chipset Initialization Register C0 (CIRC0)
    //UINT32 PMSYNC_TPR_CFG; // 18C4h 18C7h PMSYNC Thermal Power Reporting Configuration (PMSYNC_TPR_CFG)
    //UINT32 PM_SYNC_MISC_CFG; // 18C8h 18CBh PM_SYNC Miscellaneous Configuration (PM_SYNC_MISC_CFG)
    //UINT32 CIRD0; // 18D0h 18D3h Chipset Initialization Register D0 (CIRD0)
    //UINT32 CIRD4; // 18D4h 18D7h Chipset Initialization Register D4 (CIRD4)
    //UINT32 PM_CFG3; // 18E0h 18E3h Power Management Configuration Reg 3 (PM_CFG3)
    //UINT32 CIRE4; // 18E4h 18E7h Chipset Initialization Register E4 (CIRE4)
    //UINT32 CIRE8; // 18E8h 18EBh Chipset Initializatin Register E8 (CIRE8)
    //UINT32 ACPI_TMR_CTL; // 18FCh 18FFh ACPI Timer Control (ACPI_TMR_CTL)
    //UINT32 VR_MISC_CTL; // 1900h 1903h VR Miscellaneous Control (VR_MISC_CTL)
    //UINT32 TSC_ALARM_LO; // 1910h 1913h Last TSC Alarm Value[31:0] (TSC_ALARM_LO)
    //UINT32 TSC_ALARM_HI; // 1914h 1917h Last TSC Alarm Value[63:32] (TSC_ALARM_HI)
    //UINT32 GPIO_CFG; // 1920h 1923h GPIO Configuration (GPIO_CFG)
    //UINT32 GBLRST_CAUSE0; // 1924h 1927h Global Reset Causes (GBLRST_CAUSE0)
    //UINT32 GBLRST_CAUSE1; // 1928h 192Bh Global Reset Causes Register 1 (GBLRST_CAUSE1)
    //UINT32 HPR_CAUSE0; // 192Ch 192Fh Host Partition Reset Causes (HPR_CAUSE0)
    //UINT32 LAT_LIM_RES_0; // 1930h 1933h LATENCY_LIMIT_RESIDENCY_0 (LAT_LIM_RES_0)
    //UINT32 LAT_LIM_RES_1; // 1934h 1937h LATENCY_LIMIT_RESIDENCY_1 (LAT_LIM_RES_1)
    //UINT32 LAT_LIM_RES_2; // 1938h 193Bh LATENCY_LIMIT_RESIDENCY_2 (LAT_LIM_RES_2)
    //UINT32 SLP_S0_RES; // 193Ch 193Fh SLP S0 RESIDENCY (SLP_S0_RES)
    //UINT32 LLC; // 1940h 1943h LATENCY LIMIT CONTROL (LLC)
    //UINT32 CIR324; // 1B24h 1B27h Chipset Initialization Register 324 (CIR324)
    //UINT32 CIRB28; // 1B28h 1B2Bh Chipset Initialization Register B28 (CIRB28)
    //UINT32 CIRB40; // 1B40h 1B43h Chipset Initialization Register B40 (CIRB40)
    //UINT32 CIRB44; // 1B44h 1B47h Chipset Initialization Register B44 (CIRB44)
    //UINT32 CIRBA8; // 1BA8h 1BABh Chipset Initialization Register BA8 (CIRBA8)
    //UINT32 CIRBAC; // 1BACh 1BAFh Chipset Initialization Register BAC (CIRBAC)
    //UINT32 PM_SYNC_DATA_0; // 1BB0h 1BB3h Last PM_SYNC Message [31:0] (PM_SYNC_DATA_0)
    //UINT32 PM_SYNC_DATA_1; // 1BB4h 1BB7h Last PM_SYNC Message [63:32] (PM_SYNC_DATA_1)
    //UINT32 CWBMDIDSTATUS; // 1BD4h 1BD7h CWB MDID Status Register (CWBMDIDSTATUS)
    //UINT32 ACTL; // 1BD8h 1BDBh ACPI Control (ACTL)
    //UINT32 PMC_THROT_1; // 1BE0h 1BE3h PMC Throttling 1 (PMC_THROT_1)
    //UINT32 CIR3E8; // 1BE8h 1BEBh Chipset Initialization Register 3E8 (CIR3E8)
    //UINT32 CS_SD_CTL2; // 1BECh 1BEFh Clock Source Shutdown Control Reg 2 (CS_SD_CTL2)
    //UINT32 PPAMR1; // 1D04h 1D07h PGD Priority Agent Mapping Register 1 (PPAMR1)
    //UINT32 PPAMR2; // 1D08h 1D0Bh PGD Priority Agent Mapping Register 2 (PPAMR2)
    //UINT32 PPAMR3; // 1D0Ch 1D0Fh PGD Priority Agent Mapping Register 3 (PPAMR3)
    //UINT32 PPAMR4; // 1D10h 1D13h PGD Priority Agent Mapping Register 4 (PPAMR4)
    //UINT32 PPAMR5; // 1D14h 1D17h PGD Priority Agent Mapping Register 5 (PPAMR5)
    //UINT32 PPAMR6; // 1D18h 1D1Bh PGD Priority Agent Mapping Register 6 (PPAMR6)
    //UINT32 PPAMR7; // 1D1Ch 1D1Fh PGD Priority Agent Mapping Register 7 (PPAMR7)
    //UINT32 PPAMR8; // 1D20h 1D23h PGD Priority Agent Mapping Register 8 (PPAMR8)
    //UINT32 PPAMR9; // 1D24h 1D27h PGD Priority Agent Mapping Register 9 (PPAMR9)
    //UINT32 PPAMR10; // 1D28h 1D2Bh PGD Priority Agent Mapping Register 10 (PPAMR10)
    //UINT32 PPAMR11; // 1D2Ch 1D2Fh PGD Priority Agent Mapping Register 11 (PPAMR11)
    //UINT32 PPAMR12; // 1D30h 1D33h PGD Priority Agent Mapping Register 12 (PPAMR12)
    //UINT32 PPAMR13; // 1D34h 1D37h PGD Priority Agent Mapping Register 13 (PPAMR13)
    //UINT32 PPAMR14; // 1D38h 1D3Bh PGD Priority Agent Mapping Register 14 (PPAMR14)
    //UINT32 PPAMR15; // 1D3Ch 1D3Fh PGD Priority Agent Mapping Register 15 (PPAMR15)
    //UINT32 CIR580; // 1D80h 1D83h Chipset Initialization Register 580 (CIR580)
    //UINT32 PPASR1; // 1D84h 1D87h PGD PG_ACK Status Register 1 (PPASR1)
    //UINT32 PPFEAR0; // 1D90h 1D93h PFET Enable Ack Register 0 (PPFEAR0)
    //UINT32 PPFEAR1; // 1D94h 1D97h PFET Enable Ack Register 1 (PPFEAR1)
    //UINT32 CIRDA0; // 1DA0h 1DA3h Chipset Initialization Register DA0 (CIRDA0)
    //UINT32 PMCR; // 1DB0h 1DB3h PGD Misc Control Register (PMCR)
    //UINT32 HSWPGCR1; // 1DD0h 1DD3h Host SW PG Control Register 1 (HSWPGCR1)
    //UINT32 PPRSR0; // 1DE0h 1DE3h PGD PG_REQ Status Register 0 (PPRSR0)
    //UINT32 PPRSR1; // 1DE4h 1DE7h PGD PG_REQ Status Register 1 (PPRSR1)
    //UINT32 ST_PG_FDIS1; // 1E20h 1E23h Static PG Function Disable 1 (ST_PG_FDIS1)
    //UINT32 ST_PG_FDIS2; // 1E24h 1E27h Static Function Disable Control 2 (ST_PG_FDIS2) 
    //UINT32 NST_PG_FDIS_1; // 1E28h 1E2Bh Non-Static PG Related Function Disable Register 1 (NST_PG_FDIS_1)
    //UINT32 N_STPG_FUSE_SS_DIS_RD_1; // 1E40h 1E43h Capability Disable Status 1 (N_STPG_FUSE_SS_DIS_RD_1)
    //UINT32 STPG_FUSE_SS_DIS_RD_2; // 1E44h 1E47h Capability Disable Status 2 (STPG_FUSE_SS_DIS_RD_2)
} PMC_MEMORY_MAPPED_REGISTERS, *PPMC_MEMORY_MAPPED_REGISTERS;

//
//BDF 00:1f:04
typedef struct _SMBI {
    PCI_CONFIG_GENERAL Header; // 00h - 3Fh
    struct {
        UINT32 HSTEN:1; // 0 0h RW HST_EN (HSTEN): When set, the SMB Host Controller interface is enabled to execute commands. The HST_INT_EN bit needs to be enabled in order for the SMB Host Controller to interrupt or SMI#. Additionally, the SMB Host Controller will not respond to any new requests until all interrupt requests have been cleared
        UINT32 SSEN:1; // 1 0h RW SMB_SMI_EN (SSEN): When this bit is set, any source of an SMB interrupt will instead be routed to generate an SMI#.
        UINT32 I2CEN:1; // 2 0h RW I2C_EN (I2CEN): When this bit is 1, the Intel PCH is enabled to communicate with I2C devices. This will change the formatting of some commands. When this bit is 0,  behavior is for SMBus.
        UINT32 SSRESET:1; // 3 0h RW SSRESET (SSRESET): Soft SMBUS Reset: When this bit is 1, the SMbus state machine and logic in PCH is reset. The HW will reset this bit to 0 when reset operation is completed.
        UINT32 SPDWD:1; // 4 0h RW/1L SPD Write Disable (SPDWD): When this bit is set to 1, writes to SMBus addresses 50h – 57h are disabled. Note: This bit is R/WO and will be reset on PLTRST# assertion. This bit should be set by BIOS to ‘1’. Software can only program this bit when both the START bit and Host Busy bit are ‘0’; otherwise, the write may result in undefined behavior.
        UINT32 RES_7_5:3; // 7:5 0h RO Reserved.
        UINT32 RES_31_8:24; // 31:8
    } HCFG; // 40 Host Configuration
    UINT8 RES_4F_44[0xC];
    struct {
        UINT32 IOS:1; // 0 1h RO I/O Space (IOS): Indicates an I/O Space
        UINT32 RES_4_1:4; // 4:1 0h RO Reserved.
        UINT32 TCOBA:11; // 15:5 0h RW/L TCO Base Address (TCOBA): Provides the 32 bytes of I/O space for TCO logic,  mappable anywhere in the 64k I/O space on 32-byte boundaries.
        UINT32 RES_31_16:16; // 31:16
    } TCOBASE; // 50 TCO Base Address
    struct {
        UINT32 TCO_BASE_LOCK:1; // 0 0h RW/O TCO Base Lock (TCO_BASE_LOCK): When set to 1, this bit locks down the TCO Base Address Register (TCOBASE) at offset 50h. The Base Address Field becomes read-only. This bit becomes locked when a value of 1b is written to it. Writes of 0 to this bit are always ignored. Once locked by writing 1, the only way to clear this bit is to perform a platform reset.
        UINT32 RES_7_1:7; // 7:1 0h RO Reserved.
        UINT32 TCO_BASE_EN:1; // 8 0h RW TCO Base Enable (TCO_BASE_EN): When set, decode of the I/O range pointed to by the TCO base register is enabled.
        UINT32 RES_31_9:23; // 31:9 0h RO Reserved.
    } TCOCTL; // 54 TCO Control
    UINT8 RES_63_58[0xC]; // 63 - 58
    struct {
        UINT32 RES_31_0:32; // ??
    } HTIM; // 64 Host SMBus Timing
    UINT8 RES_7F_68[0x18]; // 7F - 68
    struct {
        UINT32 RES_17_0:18; // 17:0 0h RO Reserved.
        UINT32 PGCBDCGDIS:1; // 18 1h RW SMBus Dynamic Clock Gating (PGCBDCGDIS): Setting this bit will disable the  SMBus dynamic clock gating.
        UINT32 RES_31_19:13; // 31:19 0h RO Reserved.
    } SMBSM; // 80 SMBus Power Gating
} SMBUS_INTERFACE, *PSMBUS_INTERFACE;

//
// The SMBus registers can be accessed through I/O BAR or Memory BAR registers in PCI configuration space. The offsets are the same for both I/O and Memory Mapped I/O registers
// bar0 or bar4
//typedef struct _SMBUS_IO_REGISTERS{
//0h 0h Host Status Register Address (HSTS)
// 7 0h RW/1C BYTE_DONE_STS (BDS): This bit will be set to 1 when the host controller has received a byte (for Block Read commands) or if it has completed transmission of a byte (for Block Write commands) when the 32-byte buffer is not being used. Note that this bit will be set, even on the last byte of the transfer. Software clears the bit by writing a 1 to the bit position. This bit has no meaning for block transfers when the 32- byte buffer is enabled. Note: When the last byte of a block message is received, the host controller will set this bit. However, it will not immediately set the INTR bit (bit 1 in this register). When the interrupt handler clears the BYTE_DONE_STS bit, the message is considered complete, and the host controller will then set the INTR bit (and generate another interrupt). Thus, for a block message of n bytes, the Intel PCH will generate n+1 interrupts. The interrupt handler needs to be implemented to handle these cases.
// 6 0h RW/1C In Use Status (IUS): After a full PCI reset, a read to this bit returns a 0. After the first read, subsequent reads will return a 1. A write of a 1 to this bit will reset the next read value to 0. Writing a 0 to this bit has no effect. Software can poll this bit until it reads a 0, and will then own the usage of the host controller. This bit has no other effect on the hardware, and is only used as semaphore among various independent software threads that may need to use the Intel PCHs SMBus logic.
// 5 0h RW/1C SMBALERT_STS (SMSTS): Intel PCH sets this bit to a 1 to indicates source of the interrupt or SMI# was the SMBAlert# signal. Software resets this bit by writing a 1 to this location. This bit should also be cleared by RSMRST# (but not PLTRST#).
// 4 0h RW/1C Failed (FAIL): When set, this indicates that the source of the interrupt or SMI# was  a failed bus transaction. This is set in response to the KILL bit being set to terminate the host transaction.
// 3 0h RW/1C Bus Error (BERR): When set, this indicates the source of the interrupt or SMI# was a transaction collision.
// 2 0h RW/1C Device Error (DERR): When set, this indicates that the source of the interrupt or SMI# was due one of the following: Illegal Command Field Unclaimed Cycle (host initiated) Host Device Time-out Error. CRC Error
// 1 0h RW/1C Interrupt (INTR): When set, this indicates that the source of the interrupt or SMI# was the successful completion of its last command.
// 0 0h RW/1C Host Busy (HBSY): A 1 indicates that the Intel PCH is running a command from the host interface. No SMB registers should be accessed while this bit is set. Exception: The BLOCK DATA REGISTER can be accessed when this bit is set ONLY when the SMB_CMD bits (in Host control register) are programmed for Block command or I2C Read command. This is necessary in order to check the DONE_STS bit.
//2h 2h Host Control Register (HCTL)
// 7 0h RW PEC_EN (PEC_EN): When set to 1, this bit causes the host controller to perform the SMBus transaction with the Packet Error Checking phase appended. For writes, the value of the PEC byte is transferred from the PEC Register. For reads, the PEC byte is loaded in to the PEC Register. When this bit is cleared to 0, the SMBus host controller does not perform the transaction with the PEC phase appended. This bit must be written prior to the write in which the START bit is set.
// 6 0h RW START (START): This write-only bit is used to initiate the command described in the SMB_CMD field. All registers should be setup prior to writing a 1 to this bit position. This bit always reads zero. The HOST_BUSY bit in the Host Status register (offset 00h) can be used to identify when the Intel PCH has finished the command.
// 5 0h RW LAST_BYTE (LAST_BYTE): This bit is used for I2C Read commands. Software sets this bit to indicate that the next byte will be the last byte to be received for the block. This causes the PCH to send a NACK (instead of an ACK) after receiving the last byte. Note: This bit may be set when the TCO timer causes the SECOND_TO_STS bit to be set. SW should clear the LAST_BYTE bit (if it is set) before starting any new command. Note: In addition to I2C Read Commands, the LAST_BYTE bit will also cause Block Read/Write cycles to stop prematurely (at the end of the next byte).
// 4:2 0h RW SMB_CMD (SMB_CMD): As shown by the bit encoding below, indicates which command the PCH is to perform. If enabled, the Intel PCH will generate an interrupt or SMI# when the command has completed. If the value is for a non-supported or reserved command, the PCH will set the device error(DEV_ERR) status bit and generate an interrupt when the START bit is set. The PCH will perform no command, and will not operate until DEV_ERR is cleared.Val.
//    000 - Quick: The slave address and read/write value (bit 0) are stored in the tx slave address register
//    001 - Byte: This command uses the transmit slave address and command registers. Bit 0 of the slave address register determines if this is a read or write command.
//    010 - Byte Data: This command uses the transmit slave address, command, and DATA0 registers. Bit 0 of the slave address register determines if this is a read or write command. If it is a read, the DATA0 register will contain the read data.
//    011 - Word Data: This command uses the transmit slave address, command, DATA0 and DATA1 registers. Bit 0 of the slave address register determines if this is a read or write command. If it is a read, after the command completes the DATA0 and DATA1 registers will contain the read data.
//    100 - Process Call: This command uses the transmit slave address, command, DATA0 and DATA1 registers. Bit 0 of the slave address register determines if this is a read or write command. After the command completes, the DATA0 and DATA1 registers will contain the read data.
//    101 - Block: This command uses the transmit slave address, command, and DATA0 registers, and the Block Data Byte register. For block write, the count is stored in the DATA0 register and indicates how many bytes of data will be transferred. For block reads, the count is received and stored in the DATA0 register. Bit 0 of the slave address register selects if this is a read or write command. For writes, data is retrieved from the first n (where n is equal to the specified count) addresses of the SRAM array. For reads, the data is stored in the Block Data Byte register.
//    110 - I2C Read: This command uses the transmit slave address, command, DATA0, DATA1 registers, and the Block Data Byte register. The read data is stored in the Block Data Byte register. The Intel PCH will continue reading data until the NAK is received.
//    111 - Block-Process: This command uses the transmit slave address, command, DATA0 and the Block Data Byte register. For block write, the count is stored in the DATA0 register and indicates how many bytes of data will be transferred. For block read, the count is received and stored in the DATA0 register. Bit 0 of the slave address register always indicate a write command. For writes, data is retrieved from the first m (where m is equal to the specified count) addresses of the SRAM array. For reads, the data is stored in the Block Data Byte register. Note: E32B bit in the Auxiliary Control Register must be set for this command to work.
// 1 0h RW KILL (KILL): When set, kills the current host transaction taking place, sets the FAILED status bit, and asserts the interrupt (or SMI#) selected by the SMB_INTRSEL field. This bit, once set, must be cleared to allow the SMB Host Controller to function normally.
// 0 0h RWINTREN (INTREN): Enable the generation of an interrupt or SMI# upon the completion of the command
//3h 3h Host Command Register (HCMD)
// 7:0 0h RW Host Command Register (HCMD): This eight bit field is transmitted by the host controller in the command field of the SMB protocol during the execution of any command.
//4h 4h Transmit Slave Address Register (TSA)
// 7:1 0h RW ADDRESS (ADDR): 7-bit address of the targeted slave. Note: Writes to TSA values of A0h - AEh are blocked depending on the setting of the SPD write disable bit in HCFG - HostConfiguration.
// 0 0h RW RW (RW): Direction of the host transfer. 1 = read, 0 = write
//5h 5h Data 0 Register (HD0)
// 7:0 0h RW DATA0/COUNT (DATA0_COUNT): This field contains the eight bit data sent in the DATA0 field of the SMB protocol. For block write commands, this register reflects the number of bytes to transfer. This register should be programmed to a value between 1 and 32 for block counts. A count of 0 or a count above 32 will result in unpredictable behavior. The host controller does not check or log illegal block counts
//6h 6h Data 1 Register (HD1)
// 7:0 0h RW DATA1 (DATA1): This eight bit register is transmitted in the DATA1 field of the SMB protocol during the execution of any command.
//7h 7h Host Block Data (HBD)
// 7:0 0h RW Block Data (BDTA): This is either a register, or a pointer into a 32- byte block array, depending upon whether the E32B bit is set in the Auxiliary Control register. When the E32B bit is cleared, this is a register containing a byte of data to be sent on a block write or read from on a block read, just as it behaved on the INTEL PCH. When the E32B bit is set, reads and writes to this register are used to access the 32-byte block data storage array. An internal index pointer is used to address the array, which is reset to 0 by reading the HCTL register (offset 02h). The index pointer then increments automatically upon each access to this register. The transfer of block data into (read) or out of (write) this storage array during an SMBus transaction always starts at index address 0. When the E2B bit is set, for writes, software will write up to 32-bytes to this register as part of the setup for the command. After the Host Controller has sent the Address, Command, and Byte Count fields, it will send the bytes in the SRAM pointed to by this register. When the E2B bit is cleared for writes, software will place a single byte in this register. After the host controller has sent the address, command, and byte count fields, it will send the byte in this register. If there is more data to send, software will write the next series of bytes to the SRAM pointed to by this register and clear the DONE_STS bit. The controller will then send the next byte. During the time between the last byte being transmitted to the next byte being transmitted, the controller will insert wait-states on the interface. When the E2B bit is set for reads, after receiving the byte count into the Data0 register, the first series of data bytes go into the SRAM pointed to by this register. If the byte count has been exhausted or the 32-byte SRAM has been filled, the controller will generate an SMI# or interrupt (depending on configuration) and set the DONE_STS bit. Software will then read the data. During the time between when the last byte is read from the SRAM to when the DONE_STS bit is cleared, the controller will insert waitstates on the interface.
//8h 8h Packet Error Check Data Register (PEC)
// 7:0 0h RW PEC_DATA (PEC_DATA): This 8-bit register is written with the SMBus PEC data prior to a write transaction. For read transactions, the PEC data is loaded from the SMBus into this register and is then read by software. Software must ensure that the INUSE_STS bit is properly maintained to avoid having this field over-written by a write transaction following a read transaction.
//9h 9h Receive Slave Address Register (RSA)
// 7 0h RO Reserved.
// 6:0 44h RW SLAVE_ADDR[6:0] (SA_6_0): This field is the slave address that the Intel PCH decodes for read and write cycles. The default is not 0 so that it can respond even before the CPU comes up (or if the CPU is dead). This register is reset by RSMRST#, but not by PLTRST#
//Ah Bh Slave Data Register (SD)
// 15:0 0h RO/V SLAVE_DATA[15:0] (SD_15_0): This field is the 16-bit data value written by the external SMBus master. The CPU can then read the value from this register. This register is reset by RSMRST#, but not by PLTRST#. SLAVE_DATA[7:0] corresponds to the Data Message Byte 0 at Slave Write Register 4 in the table. SLAVE_[15:8] corresponds to the Data Message Byte 1 at Slave Write Register 5 in the table.
//Ch Ch Auxiliary Status (AUXS)
// 7:1 0h RO Reserved.
// 0 0h RW/1C CRC Error (CRCE): This bit is set if a received message contained a CRC error. When this bit is set, the DERR bit of the host status register will also be set. This bit will be set by the controller if a software abort occurs in the middle of the CRC portion of the cycle or an abort happens after Intel PCH has received the final data bit transmitted by external slave.
//Dh Dh Auxiliary Control (AUXC)
// 7:2 0h RO Reserved.
// 1 0h RW Enable 32-byte Buffer (E32B): When set, the Host Block Data register is a pointer into a 32-byte buffer, as opposed to a single register. This enables the block commands to transfer or receive up to 32-bytes before the Intel PCH generates an interrupt.
// 0 0h RW Automatically Append CRC (AAC): When set, the Intel PCH will automatically append the CRC. This bit must not be changed during SM Bus transactions, or undetermined behavior will result. It should be programmed only once during the lifetime of the function
//Eh Eh SMLINK_PIN_CTL Register (SMLC)
// 7:3 0h RO Reserved.
// 2 1h RW SMLINK_CLK_CTL (SMLINK_CLK_CTL): 0 = Intel PCH will drive the SMLINK[0] pin low, independent of what the other SMLINK logic would otherwise indicate for the SMLINK(0) pin. 1 = The SMLINK[0] pin is Not overdriven low. The other SMLINK logic controls the state of the pin.
// 1 0h RO/V SMLINK[1]_CUR_STS (SMLINK1_CUR_STS): This bit has a default value that is dependent on an external signal level. This returns the value on the SMLINK[1] pin. It will be 1 to indicate high, 0 to indicate low. This allows software to read the current state of the pin.
// 0 0h RO/V SMLINK[0]_CUR_STS (SMLINK0_CUR_STS): This bit has a default value that is dependent on an external signal level. This returns the value on the SMLINK[0] pin. It will be 1 to indicate high, 0 to indicate low. This allows software to read the current state of the pin.
//Fh Fh SMBUS_PIN_CTL Register (SMBC)
// 7:3 0h RO Reserved.
// 2 1h RW SMBCLK_CTL (SMBCLK_CTL): 0 = Intel PCH will drive the SMBCLK pin low, independent of what the other SMB logic would otherwise indicate for the SMBCLK pin. 1 = The SMBCLK pin is Not overdriven low. The other SMBus logic controls the state of the pin.
// 1 0h RO/V SMBDATA_CUR_STS (SMBDATA_CUR_STS): This bit has a default value that is dependent on an external signal level. This returns the value on the SMBDATA pin. It will be 1 to indicate high, 0 to indicate low. This allows software to read the current state of the pin.
// 0 0h RO/V SMBCLK_CUR_STS (SMBCLK_CUR_STS): This bit has a default value that is dependent on an external signal level. This returns the value on the SMBCLK pin. It will be 1 to indicate high, 0 to indicate low. This allows software to read the current state of the pin.
//10h 10h Slave Status Register (SSTS)
//11h 11h Slave Command Register (SCMD)
//14h 14h Notify Device Address Register (NDA)
//16h 16h Notify Data Low Byte Register (NDLB)
//17h 17h Notify Data High Byte Register (NDHB)
//} SMBUS_IO_REGISTERS, *PSMBUS_IO_REGISTERS;


//
//BDF 00:1f:05
typedef struct _SPI_CFG {
    PCI_CONFIG_GENERAL Header;
    UINT8 Reserved1[0x90]; // 0x40
    struct {
        UINT32 URRE:1; // 0 0h RW Unsupported Request Reporting Enabled (URRE): If set to 1 by software, the flash controller generates a DoSERR sideband message when the URD bit transitions from 0 to 1.
        UINT32 URD:1; // 1 0h RW/1C/V Unsupported Request Detected (URD): Set to 1 by hardware upon detecting an  Unspported Request that is not considered an Advisory Non-Fatal error. Cleared to 0 when software writes a 1 to this register.
        UINT32 Reserved1:30; // 31:2 0h RO Reserved.
    } UR_STS_CTL; // D0h SPI Unsupported Request Status (BIOS_SPI_UR_STS_CTL)
    UINT32 Reserved2; // D4h
    struct {
        UINT32 E40:1; // 0 1b RW 40-4F Enable (E40) Enables decoding of 1MB of the following BIOS ranges:
                    //FF400000h - FF4FFFFFh
                    //FF000000h - FF0FFFFFh
        UINT32 E50:1; // 1 1b RW 50-5F Enable (E50) Enables decoding of 1MB of the following BIOS ranges:
                    //FF500000h - FF5FFFFFh
                    //FF100000h - FF1FFFFFh
        UINT32 E60:1; // 2 1b RW 60-6F Enable (E60) Enables decoding of 1MB of the following BIOS ranges:
                    //FF600000h - FF6FFFFFh
                    //FF200000h - FF2FFFFFh
        UINT32 E70:1; // 3 1b RW 70-7F Enable (E70) Enables decoding of 1MB of the following BIOS ranges:
                    //FF700000h - FF7FFFFFh
                    //FF300000h - FF3FFFFFh
        UINT32 Reserved1:2; //5:4 - - Reserved
        UINT32 LEE:1; // 6 1b RW Legacy E Segment Enable (LEE) Legacy E Segment Enable (LFE): This enables the decoding of the
                     //legacy 64KB range at E0000h - EFFFFh
        UINT32 LFE:1; // 7 1b RW Legacy F Segment Enable (LFE) Legacy F Segment Enable (LFE): This enables the decoding of the
                    //legacy 64KB range at F0000h - FFFFFh
        UINT32 EC0:1; // 8 1b RW C0-C7 Enable (EC0) Enables decoding of 512K of the following BIOS ranges:
                    //FFC00000h - FFC7FFFFh
                    //FF800000h - FF87FFFFh
        UINT32 EC8:1; // 9 1b RW C8-CF Enable (EC8) Enables decoding of 512K of the following BIOS ranges:
                    //FFC80000h - FFCFFFFFh
                    //FF880000h - FF8FFFFFh
        UINT32 ED0:1; // 10 1b RW D0-D7 Enable (ED0) Enables decoding of 512K of the following BIOS ranges:
                    //FFD00000h - FFD7FFFFh
                    //FF900000h - FF97FFFFh
        UINT32 ED8:1; // 11 1b RW D8-DF Enable (ED8) Enables decoding of 512K of the following BIOS ranges:
                    //FFD80000h - FFDFFFFFh
                    //FF980000h - FF9FFFFFh
        UINT32 EE0:1; // 12 1b RW E0-E8 Enable (EE0) Enables decoding of 512K of the following BIOS ranges:
                    //FFE00000h - FFE7FFFFh
                    //FFA00000h - FFA7FFFFh
        UINT32 EE8:1; // 13 1b RW E8-EF Enable (EE8) Enables decoding of 512K of the following BIOS ranges:
                    //FFE80000h - FFEFFFFFh
                    //FFA80000h - FFAFFFFFh
        UINT32 EF0:1; // 14 1b RW F0-F8 Enable (EF0) Enables decoding of 512K of the following BIOS ranges:
                    //FFF00000h - FFF7FFFFh
                    //FFB00000h - FFB7FFFFh
        UINT32 EF8:1; // 15 1b RO F8-FF Enable (EF8) Enables decoding of 512K of the following BIOS ranges:
                    //FFF80000h - FFFFFFFFh
                    //FFB80000h - FFBFFFFFh
        UINT32 Reserved2:16; //31:16 - - Reserved
    } BDE; // D8h BIOS Decode Enable (BIOS_SPI_BDE)
    struct {
        UINT32 WPD:1; // 0 0h RW Write Protect Disable (WPD): When set, access to the BIOS space is enabled for both read and write cycles to BIOS. When cleared, only read cycles are permitted to the FWH or SPI flash.When this bit is written from a '0' to a '1' and the LE bit is also set, an SMI# is generated. This ensures that only SMM code can update BIOS.
        UINT32 LE:1; // 1 0h RW/L Lock Enable (LE): When set, setting the WPD bit will cause SMI. When cleared, setting the WPD bit will not cause SMI. Once set, this bit can only be cleared by a PLTRST#. When this bit is set, EISS - bit [5] of this register is locked down.
        UINT32 SRC:2; // 3:2 2h RW SPI Read Configuration (SRC): These bits are located in PCI Config space to allow  them to be set early in the boot flow. This 2-bit field controls two policies related to BIOS reads on the SPI interface: Bit 3- Prefetch Enable Bit 2- Cache Disable Settings are summarized below: 00 = No prefetching, but caching enabled. Direct Memory reads load the read buffer  cache with valid data, allowing repeated reads to the same range to complete quickly 01 = No prefetching and no caching. One-to-one correspondence of host BIOS reads  to SPI cycles. This value can be used to invalidate the cache. 10 = Prefetching and Caching enabled. This mode is used for long sequences of short reads to consecutive addresses (i.e. shadowing) 11 = Illegal. Caching must be enabled when Prefetching is enabled.
        UINT32 TSS:1; // 4 0h RO/V Top Swap Status (TSS): This bit provides a read-only path to view the state of the  Top Swap bit. It is duplicated here to be consistent with the LPC version of the BC  register.
        UINT32 EISS:1; // 5 1h RW/L Enable InSMM.STS (EISS): When this bit is set, the BIOS region is not writable  until the CPU sets the InSMM.STS bit. If this bit [5] is set, then WPD must be a '1' and InSMM.STS(0xFED3_0880[0]) must  be '1' also in order to write to BIOS region of SPI Flash. If this bit [5] is clear, then the InSMM.STS is a do not care. This bit is locked by LE
        UINT32 BBS:1; // 6 0h RW/V/L Boot BIOS Strap (BBS): This field determines the destination of accesses to the  BIOS memory range. 0 = SPI 1 = LPC When SPI or LPC is selected, the range that is decoded is further qualified by BIOS  Decode Enable. The value in this field can be overwritten by software as long as the BIOS Interface Lock-Down (BILD) is not set
        UINT32 BILD:1; // 7 0h RW/L BIOS Interface Lock-Down (BILD): When set, prevents TS and BBS from being changed. This bit can only be written from 0 to 1 once.
        UINT32 SPI_SYNC_SS:1; // 8 0h RW/1C/V Synchronous SMI Status (SPI_SYNC_SS): Status indication that the SPI Flash Controller has asserted a synchronous SMI. Hardware clears the bit when it sends the De-assert Synchronous SMI message. 0 : default state 1 : SPI flash controller asserted Synchronous SMI
        UINT32 Reserved1:1; // 9 0h RW/L Reserved
        UINT32 SPI_ASYNC_SS:1; // 10 0h RO/V Asynchronous SMI Status (SPI_ASYNC_SS): Status indication that the SPI Flash Controller has asserted an asynchronous SMI. Hardware clears the bit when it sends the De-assert SMI message. 0 : default state 1 : SPI flash controller asserted asynchronous SMI
        UINT32 ASE_BWP:1; // 11 0h RW/L Async SMI Enable for BIOS Write Protection (ASE_BWP): When set to '1' the flash controller will generate an SMI when it blocks a BIOS write or erase due to WPD = 0. The value in this field can be written by software as long as the BIOS Interface Lock-Down (BILD) is not set.
        UINT32 Reserved2:20; // 31:12 0h RO Reserved.
    } BC; // DCh BIOS Control (BIOS_SPI_BC) Config
} SPI_CFG, *PSPI_CFG;


//
//BDF 00:1f:06
typedef struct _INT_GBE {
    PCI_CONFIG_GENERAL Header; // 00h - 3Fh
    UINT8 RES_9F_40[0x60];
    struct {
        UINT32 LD:1; // 0 0h RW LAN Disable (LD): Setting this bit to 1 will disable the LAN Controller functionality.
        UINT32 RES_31_1:31; // 31:1 0h RO Reserved. 
    } LANDISCTRL; // A0 LAN Disable Control
    struct {
        UINT32 LLD:1; // 0 0h RW Lock LAN Disable (LLD): When set this bit blocks writes to the LANDISCTRL register. Note: Once set this bit will only be cleared on host reset.
        UINT32 RES_31_1:31; // 31:1 0h RO Reserved. 
    } LOCKLANDIS; // A4 Lock LAN Disable
    struct {
        UINT32 MSL:10; // 9:0 0h RW Maximum Snoop Latency (MSL): Specifies the maximum snoop latency that a device is permitted to request. Software should set this to the platform’s maximum supported latency or less. This field is also an indicator of the platforms maximum latency, should an endpoint send up LTR Latency Values with the Requirement bit not set.
        UINT32 MSLS:3; // 12:10 0h RW Maximum Snoop Latency Scale (MSLS): Provides a scale for the value contained  within the Maximum Snoop Latency Value field. 000b = Value times 1 ns 001b = Value times 32 ns 010b = Value times 1,024 ns 011b = Value times 32,768 ns 100b = Value times 1,048,576 ns 101b = Value times 33,554,432 ns 110b–111b = Reserved
        UINT32 RES_15_13:3; // 15:13 0h RO Reserved
        UINT32 MNSL:10; // 25:16 0h RW Maximum Non-Snoop Latency (MNSL): Specifies the maximum non-snoop latency that a device is permitted to request. Software should set this to the platform’s maximum supported latency or less. This field is also an indicator of the platforms maximum latency, should an endpoint  send up LTR Latency Values with the Requirement bit not set.
        UINT32 MNSLS:3; // 28:26 0h RW Maximum Non-Snoop Latency Scale (MNSLS): Provides a scale for the value contained within the Maximum Non-Snoop Latency Value field. 000b = Value times 1 ns 001b = Value times 32 ns 010b = Value times 1,024 ns 011b = Value times 32,768 ns 100b = Value times 1,048,576 ns 101b = Value times 33,554,432 ns 110b–111b = Reserved
        UINT32 RES_31_29:3; // 31:29 0h RO Reserved.
    } LTRCAP; // A8 System Time Control High Register
} INTEGRATED_GBE, *PINTEGRATED_GBE;

// GbE Memory Mapped I/O Registers
typedef struct _INT_GBE_MM_IO {
    UINT32 GBECSR_00; // 0h 3h Gigabit Ethernet Capabilities and Status (GBECSR_00)—Offset 0h 0h
    UINT8 RES_17_4[0x14];
    UINT32 GBECSR_18; // 18h 1Bh Gigabit Ethernet Capabilities and Status (GBECSR_18)—Offset 18h 0h
    UINT32 RES_1F_1C[0x4];
    UINT32 GBECSR_20; // 20h 23h Gigabit Ethernet Capabilities and Status (GBECSR_20)—Offset 20h 10000000h
    UINT8 RES_EFF_24[0xEDC];
    UINT32 GBECSR_F00; // F00h F03h Gigabit Ethernet Capabilities and Status (GBECSR_F00)—Offset F00h 0h
    UINT8 RES_F0F_F04[0xC];
    UINT32 GBECSR_F10; // F10h F13h Gigabit Ethernet Capabilities and Status F10 (GBECSR_F10)—Offset F10h Ch
    UINT8 RES_53FF_14[0x53EC];
    UINT32 GBECSR_5400; // 5400h 5403h Gigabit Ethernet Capabilities and Status (GBECSR_5400)—Offset 5400h 0h
    UINT32 GBECSR_5404; // 5404h 5407h Gigabit Ethernet Capabilities and Status (GBECSR_5404)—Offset 5404h 0h
    UINT8 RES_57FF_5408[0x3FB];
    UINT32 GBECSR_5800; // 5800h 5803h Gigabit Ethernet Capabilities and Status (GBECSR_5800)—Offset 5800h 0h
    UINT8 RES_5B53_5804[0x350];
    UINT32 GBECSR_5B54; // 5B54h 5B57h Gigabit Ethernet Capabilities and Status (GBECSR_5B54)—Offset 5B54h 0h
} INT_GBE_MM_IO, *PINT_GBE_MM_IO;

//Gigabit Ethernet Capabilities and Status (GBECSR_00)
//23:0 0h RO Reserved.
//24 0h RW PHY Power Down (PHYPDN): When cleared (0b), the PHY power down setting is  controlled by the internal logic of PCH.
//31:25 0h RO Reserved.
//
//Gigabit Ethernet Capabilities and Status (GBECSR_18)
//19:0 0h RO Reserved
//20 0h RW PHY Power Down Enable (PHYPDEN): When set, this bit enables the PHY to enter a low-power state when the LAN controller is at the DMOff/ D3 or with no WOL.
//31:21 0h RO Reserved.
//
//Gigabit Ethernet Capabilities and Status (GBECSR_20)
//15:0 0h RW/V DATA: In a Write command, software places the data bits and the MAC shifts them out to the LAN Connected Device. In a Read command, the MAC reads these bits serially from the LAN Connected Device and software can read them from this location.
//20:16 0h RW/V LAN Connected Device Register Address (REGADD) 
//25:21 0h RW/V LAN Connected Device Address (PHYADD) 
//27:26 0h RW/V MDI Type: 01 = MDI Write 10 = MDI Read All other values are reserved.
//28 1h RW/V Ready Bit (RB): Set to 1 by the Gigabit Ethernet Controller at the end of the MDI  transaction. This bit should be reset to 0 by software at the same time the command  is written.
//29 0h RW/V Interrupt Enable (IE): When set to 1 by software, it will cause an Interrupt to be  asserted to indicate the end of an MDI cycle.
//30 0h RW/V Error: Set to 1 by the Gigabit Ethernet Controller when it fails to complete an MDI  read. Software should make sure this bit is clear before making an MDI read or write  command.
//31 0h RW/V Wait: Set to 1 by the Gigabit Ethernet Controller to indicate that a PCI Express* to  SMBus transition is taking place. The ME/Host should not issue new MDIC  transactions while this bit is set to 1. This bit is auto cleared by hardware after the  transition has occurred.
//
//Gigabit Ethernet Capabilities and Status (GBECSR_F00)
//4:0 0h RO Reserved.
//5 0h RW/V Software Semaphore FLAG (SWFLAG): This bit is set by the device driver to gain access permission to shared CSR registers with the firmware and hardware.
//31:6 0h RO Reserved.
//
//Gigabit Ethernet Capabilities and Status F10 (GBECSR_F10)
//0 0h RO Reserved.
//1 0h RW LPLU in D0a (LPLUD): Enables the PHY to negotiate for the slowest possible link in  all power states. This bit overrides bit 2.
//2 1h RW LPLU in non D0a (LPLUND): Enables the PHY to negotiate for the slowest possible  link in all power states except D0a.
//3 1h RW GbE Disable at non D0a—: Prevents the PHY from auto-negotiating 1000Mb/s link  in all power states except D0a. This bit must be set since GbE is not supported in Sx  states.
//5:4 0h RO Reserved.
//6 0h RW Global GbE Disable (GGD): Prevents the PHY from auto-negotiating 1000Mb/s link  in all power states.
//31:7 0h RO Reserved.
//
//Gigabit Ethernet Capabilities and Status (GBECSR_5400)
//31:0 0h RW Receive Address Low (RAL): The lower 32 bits of the 48-bit Ethernet Address.
//
//Gigabit Ethernet Capabilities and Status (GBECSR_5404)
//15:0 0h RW Receive Address High (RAH): The lower 16 bits of the 48-bit Ethernet Address.
//30:16 0h RO Reserved.
//31 0h RW Address Valid (AV) 

//Gigabit Ethernet Capabilities and Status (GBECSR_5800)
typedef struct _GBECSR_5800 {
    UINT32 APME:1; // 0 0h RW Advanced Power Management Enable (APME): 1 = APM Wakeup is enabled 0 = APM Wakeup is disabled
    UINT32 RES_31_1:31; // 31:1 0h RO Reserved.
}GBECSR_5800, *PGBECSR_5800;

// Gigabit Ethernet Capabilities and Status (GBECSR_5B54)
typedef struct _GBECSR_5B54 {
    UINT32 RES_14_0:15; // 14:0 0h RO Reserved.
    UINT32 FWVAL:1; // 15 0h RW Firmware Valid Bit (FWVAL): 1 = Firmware is ready 0 = Firmware is not ready
    UINT32 RES_31_16:16; // 31:16 0h RO Reserved.
} GBECSR_5B54, *PGBECSR_5B54;


#pragma pack()
