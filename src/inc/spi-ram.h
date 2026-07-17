#pragma once

#define FLASH_DESRIPTOR_MIN_SIZE (0x1000)
#define FLASH_DESRIPTOR_MODE_SIG (0x0FF0'A55A)

#define FDATA_COUNT (0x10)
#define FDATA_MAX_SIZE ((UINT32)(FDATA_COUNT*sizeof(UINT32)))

#define MAX_NUM_FLREG (0xA)

#define OEM_SECTION_OFFSET (0xF00)
#define OEM_SECTION_SIZE (0x100)

#define FD_INVALID_REGION_BASE (0x7FFF)
#define FD_INVALID_REGION_LIMIT (0x0)

#define SPI_RAM_NR_FLASH_REGIONS (0x6) // officially in public specs there are 6
#define SPI_RAM_MAX_NR_FLASH_REGIONS (0xC) // there is the potential for 0xC, and id 0x8 is sometimes filled

#define FREG_FD_ID (0)
#define FREG_BIOS_ID (1)
#define FREG_ME_ID (2)
#define FREG_GBE_ID (3)
#define FREG_PD_ID (4)
#define FREG_EC_ID (8)

#define FREG_0_NAME "Flash Descriptor" 
#define FREG_1_NAME "BIOS/UEFI" 
#define FREG_2_NAME "Intel ME" 
#define FREG_3_NAME "Gigabit Ethernet" 
#define FREG_4_NAME "Platform Data" 
#define FREG_5_NAME "REG[5]" 
#define FREG_6_NAME "REG[6]" 
#define FREG_7_NAME "REG[7]" 
#define FREG_8_NAME "EC" 
#define FREG_9_NAME "REG[9]" 
#define FREG_10_NAME "REG[10]" 
#define FREG_11_NAME "REG[11]" 

#define FD_REGION_NAME(__id__) \
      (__id__ == 0 ) ? FREG_0_NAME \
    : (__id__ == 1 ) ? FREG_1_NAME \
    : (__id__ == 2 ) ? FREG_2_NAME \
    : (__id__ == 3 ) ? FREG_3_NAME \
    : (__id__ == 4 ) ? FREG_4_NAME \
    : (__id__ == 5 ) ? FREG_5_NAME \
    : (__id__ == 6 ) ? FREG_6_NAME \
    : (__id__ == 7 ) ? FREG_7_NAME \
    : (__id__ == 8 ) ? FREG_8_NAME \
    : (__id__ == 9 ) ? FREG_9_NAME \
    : "Unknown" 

#define MASTER_ID_HOST_CPU (0)
#define MASTER_ID_ME (1)
#define MASTER_ID_GBE (2)
#define MASTER_ID_EC (3)

#define MASTER_NAME_HOST_CPU "CPU"
#define MASTER_NAME_ME "ME"
#define MASTER_NAME_GBE "GBe"
#define MASTER_NAME_EC "EC"

#define MASTER_REGION_NAME(__id__) \
      (__id__ == MASTER_ID_HOST_CPU ) ? MASTER_NAME_HOST_CPU \
    : (__id__ == MASTER_ID_ME ) ? MASTER_NAME_ME \
    : (__id__ == MASTER_ID_GBE ) ? MASTER_NAME_GBE \
    : (__id__ == MASTER_ID_EC ) ? MASTER_NAME_EC \
    : "Unknown" 

#define FCYCLE_READ (0x0) // 0000 Read (1 up to 64 bytes by setting FDBC)
#define FCYCLE_WRITE (0x2) //0010  Write (1 up to 64 bytes by setting FDBC) 
#define FCYCLE_BLOCK_ERASE (0x3) // 0011 4k Block Erase 
#define FCYCLE_SEC_ERASE (0x4) // 0100 64k Sector erase
#define FCYCLE_READ_SFDP (0x5) // 0101 Read SFDP Serial Flash Discoverable Parameter (SFDP) standard provides a consistent method of describing the functional and feature capabilities of Serial Flash devices in a standard set of internal parameter tables. E.g. AN114v1-SFDP Introduction.pdf
#define FCYCLE_JEDEC_ID (0x6) // 0110 Read JEDEC ID 
#define FCYCLE_WRITE_STATUS (0x7) // 0111 write status 
#define FCYCLE_READ_STATUS (0x8) // 1000 read status 
#define FCYCLE_RPMC_OP1 (0x9) // 1001 RPMC Op1 
#define FCYCLE_RPMC_OP2 (0xA) // 1010 RPMC Op2 

#define FCYCLE_TYPE_STR(__c__) \
      (__c__==FCYCLE_READ) ? "READ" \
    : (__c__==FCYCLE_WRITE) ? "WRITE" \
    : (__c__==FCYCLE_BLOCK_ERASE) ? "BLOCK_ERASE" \
    : (__c__==FCYCLE_SEC_ERASE) ? "FCYCLE_SEC_ERASE" \
    : (__c__==FCYCLE_READ_SFDP) ? "FCYCLE_READ_SFDP" \
    : (__c__==FCYCLE_JEDEC_ID) ? "FCYCLE_JEDEC_ID" \
    : (__c__==FCYCLE_WRITE_STATUS) ? "FCYCLE_WRITE_STATUS" \
    : (__c__==FCYCLE_READ_STATUS) ? "FCYCLE_READ_STATUS" \
    : (__c__==FCYCLE_RPMC_OP1) ? "FCYCLE_RPMC_OP1" \
    : (__c__==FCYCLE_RPMC_OP2) ? "FCYCLE_RPMC_OP2" \
    : "Unknown"

// from actual 300-series-chipset-on-package-pch-datasheet-vol-2
typedef struct _BIOS_HSFSTS_CTL {
    UINT32 FDONE:1; // 0 0h RW/1C/VFlash Cycle Done (FDONE): 
                    // The PCH sets this bit to 1 when the SPI Cycle completes after software previously set the FGO bit. 
                    // This bit remains asserted until cleared by software writing a 1 or hardware reset. When this bit is set and the SPI SMI# Enable bit is set, an internal signal is asserted to the SMI# generation block. Software must make sure this bit is cleared prior to enabling the SPI SMI# assertion for a new programmed access.
    UINT32 FCERR:1; // 1 0h RW/1C/V Flash Cycle Error (FCERR): Hardware sets this bit to 1 when a program register access is blocked to the FLASH due to one of the protection policies or when any of the programmed cycle registers is written while a programmed access is already in progress. 
                    // This bit remains asserted until cleared by software writing a 1 or until hardware reset occurs. 
                    // Software must clear this bit before setting the FLASH Cycle GO bit in this register.
    UINT32 H_AEL:1; // 2 0h RW/1C/V Access Error Log (H_AEL): Hardware sets this bit to a 1 when an attempt was made to access the BIOS region using the direct access method or an access to the BIOS Program Registers that violated the security restrictions. This bit is simply a log of an access security violation. This bit is cleared by software writing a '1'. 
    UINT32 Reserved1:2; // 4:3 0h RO Reserved.
    UINT32 H_SCIP:1; // 5 0h RO/V SPI Cycle In Progress (H_SCIP): 
                    // Hardware sets this bit when software sets the Flash Cycle Go (FGO) bit in the Hardware Sequencing Flash Control register. 
                    // This bit remains set until the cycle completes on the SPI interface. 
                    // Hardware automatically sets and clears this bit so that software can determine when read data is valid and/or when it is safe to begin programming the next command. 
                    // Software must only program the next command when this bit is 0.
    UINT32 Reserved2:5; // 10:6 0h RO Reserved.
    UINT32 WRSDIS:1; // 11 0h RW/L Write Status Disable (WRSDIS): 
                // 0 = Write status operation may be issued using Hardware Sequencing. 
                // 1 = Write status is not allowed as a Hardware Sequencing operation. The flash controller will block the operation and set the FCERR bit when software sets the 'go' bit. This bit is locked when FLOCKDN is set.
    UINT32 PRR34_LOCKDN:1; // 12 0h RW/L PRR3 PRR4 Lock-Down (PRR34_LOCKDN): When set to 1, the BIOS PRR3 and PRR4 registers cannot be written. Once set to 1, this bit can only be cleared by a hardware reset.
    UINT32 FDOPSS:1; // 13 1h RO/V Flash Descriptor Override Pin-Strap Status (FDOPSS): This register reflects the value the Flash Descriptor Override Pin-Strap.
                // '1': No override 
                // '0': The Flash Descriptor Override strap is set
    UINT32 FDV:1; // 14 0h RO/V Flash Descriptor Valid (FDV): This bit is set to a 1 if the Flash Controller read the correct Flash Descriptor Signature. If the Flash Descriptor Valid bit is not 1 , software cannot use the Hardware Sequencing registers, but must use the software sequencing registers. Any attempt to use the Hardware Sequencing registers will result in the FCERR bit being set
    UINT32 FLOCKDN:1; // 15 0h RW/L Flash Configuration Lock-Down (FLOCKDN): When set to 1, those Flash Program Registers that are locked down by this FLOCKDN bit cannot be written. Once set to 1, this bit can only be cleared by a hardware reset.

    UINT32 FGO:1; // 16 0h RW/1S/V Flash Cycle Go (FGO): A write to this register with a 1 in this bit initiates a request to the Flash SPI Arbiter to start a cycle.
    UINT32 FCYCLE:4; // 20:17 0h RW Flash Cycle (FCYCLE): This field defines the Flash SPI cycle type generated to the FLASH when the FGO bit is set as defined below:
    UINT32 WET:1; // 21 0h RW Write Enable Type (WET): 
                // 0: Use 06h as the write enable instruction
                // 1: Use 50h as the write enable instruction. Note that this setting is not supported as no supported flash devices require the 50h opcode to enable a non-volatile status register write.
    UINT32 Reserved3:2; // 23:22 0h RO Reserved.
    UINT32 FDBC:6; // 29:24 0h RW Flash Data Byte Count (FDBC): 
                    // This field specifies the number of bytes to shift in or out during the data portion of the SPI cycle. 
                    // The contents of this register are 0s based with 0b representing 1 byte and 3Fh representing 64 bytes. 
                    // The number of bytes transferred is the value of this field plus 1. 
                    // This field is ignored for the Block Erase command.
    UINT32 Reserved4:1; // 30 0h RO Reserved.
    UINT32 FSMIE:1; // 31 0h RW Flash SPI SMI# Enable (FSMIE): When set to 1, the SPI asserts an SMI# request whenever the Flash Cycle Done bit is 1.
} BIOS_HSFSTS_CTL, *PBIOS_HSFSTS_CTL; // 04 Hardware Sequencing Flash Status and Control (BIOS_HSFSTS_CTL)
 
//// from SentinelLabs article intel 300 series screen shot
//// FCYCLE is shorter, but WET is missing and Reserved1 is bigger
//typedef struct _HSF_CTL {
//    UINT16 FGO:1; // 0 Setting this bit to 1 instructs the SPI controller to perform an operation on SPI flash memory, as determined by the FDBC and FCYCLE fields
//    UINT16 FCYCLE:2; // 1-2 encodes the operation type we’d like to carry out.
//    UINT16 Reserved1:5; // 3-7
//    UINT16 FDBC:6; // 8-13 We use this field to specify the number of bytes we’d like to read/write. Since this field is only 6 bits in length, the maximum number of bytes that can be processed in a single cycle is limited to 64.
//    UINT16 Reserved2:1; // 14
//    UINT16 FSMIE:1; // 15
//} HSF_CTL, PHSF_CTL; // 06 Hardware Sequencing Flash Control This register is used to issue commands tothe SPI controller, and is composed of several fields. The ones which are of particular interest to us are

typedef struct _BIOS_FADDR {
    UINT32 FLA:27; // 26:0 0h RW Flash Linear Address (FLA): The FLA is the starting byte linear address of a SPI  Read or Write cycle or an address within a Block for the Block Erase command. 
    UINT32 Reserved:5; // 31:27 0h RO Reserved.
} BIOS_FADDR, *PBIOS_FADDR; // 08 Flash Address 32-bit offset from the beginning of the SPI flash memory


#define REGION_BASE_ADDRESS(__reg__) \
    ((__reg__.RB!=FD_INVALID_REGION_BASE) ? ((UINT32)__reg__.RB << PAGE_SHIFT) : 0)
#define REGION_LIMIT(__reg__) \
    ((__reg__.RB!=FD_INVALID_REGION_BASE) ? (((UINT32)__reg__.RL << PAGE_SHIFT) | 0xFFF) : 0)
#define REGION_SIZE(__reg__) \
    ((__reg__.RB!=FD_INVALID_REGION_BASE) ? ((REGION_LIMIT(__reg__) - REGION_BASE_ADDRESS(__reg__)) + 1) : 0)

typedef struct _BIOS_REGION {
    UINT32 RB:15; // 14:0 0h RO/V Region Base (RB): This specifies address bits 26:12 for the Region 0 Base. The value in this register is loaded from the contents in the Flash Descriptor.FLREG0.Region Base.
    UINT32 Reserved2:1; // 15 0h RO Reserved.
    UINT32 RL:15; // 30:16 0h RO/V Region Limit (RL): This specifies address bits 26:12 for the Region x Limit. The value in this register is loaded from the contents in the Flash Descriptor.FLREG0.Region Limit.
    UINT32 Reserved1:1; // 31 0h RO Reserved.
} BIOS_REGION, *PBIOS_REGION;

typedef struct _BIOS_RANGE_PROTECTION {
    UINT32 PRB:15; // 14:0 0h RW/L Protected Range Base (PRB): This field corresponds to FLA address bits 26:12 and specifies the lower base of the protected range. Address bits 11:0 are assumed to be 000h for the base comparison. Any address less than the value programmed in this field is unaffected by this protected range.
    UINT32 RPE:1; // 15 0h RW/L Read Protection Enable (RPE): When set, this bit indicates that the Base and Limit fields in this register are valid and that reads directed to addresses between them (inclusive) must be blocked by hardware. The base and limit fields are ignored when this bit is cleared.
    UINT32 PRL:15; // 30:16 0h RW/LProtected Range Limit (PRL): This field corresponds to FLA address bits 26:12 and specifies the upper limit of the protected range. Address bits 11:0 are assumed to be FFFh for the limit comparison. Any address greater than the value programmed in this field is unaffected by this protected range.
    UINT32 WPE:1; // 31 0h RW/L Write Protection Enable (WPE)
} BIOS_RANGE_PROTECTION, *PBIOS_RANGE_PROTECTION,
  FPR, *PFPR,
  GPR, *PGPR;

#define FDSS_STR(__fdss__) \
      (__fdss__==0b000)?"Flash Signature and Descriptor Map" \
    : (__fdss__==0b001)?"Component" \
    : (__fdss__==0b010)?"Region" \
    : (__fdss__==0b011)?"Master" \
    : "Reserved"

typedef struct _INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP {
    BIOS_REGION BFPREG; // 00 Bios Flash primary region
    BIOS_HSFSTS_CTL HSFSTS_CTL; // 04 Hardware Sequencing Flash Status and Control (BIOS_HSFSTS_CTL)
    BIOS_FADDR FADDR; // 08 Flash Address 32-bit offset from the beginning of the SPI flash memory
    struct {
        UINT32 BMWAGLOCKDN:1; // 0 0h RW/L BMWAG Lock-Down (BMWAGLOCKDN): BIOS FRACC.BMWAG register bits are locked when the logical OR of this bit and FLOCKDN is true. Once set to 1 this register is only cleared by host partition reset.
        UINT32 BMRAGLOCKDN:1; // 1 0h RW/L BMRAG Lock-Down (BMRAGLOCKDN): BIOS FRACC.BMRAG register bits are locked when the logical OR of this bit and FLOCKDN is true. Once set to 1 this register is only cleared by host partition reset.
        UINT32 SBMWAGLOCKDN:1; // 2 0h RW/L SBMWAG Lock-Down (SBMWAGLOCKDN): BIOS SFRACC.BMWAG register bits are locked when the logical OR of this bit and FLOCKDN is true. Once set to 1 this register is only cleared by host partition reset.
        UINT32 SBMRAGLOCKDN:1; // 3 0h RW/L SBMRAG Lock-Down (SBMRAGLOCKDN): BIOS SFRACC.BMRAG register bits are locked when the logical OR of this bit and FLOCKDN is true. Once set to 1 this register is only cleared by host partition reset.
        UINT32 SPARE7:1; // 4 0h RW/L Spare7 (SPARE7): Once set to 1 this register is only cleared by host partition reset.
        UINT32 SPARE6:1; // 5 0h RW/L Spare6 (SPARE6): Once set to 1 this register is only cleared by host partition reset.
        UINT32 SPARE5:1; // 6 0h RW/L Spare5 (SPARE5): Once set to 1 this register is only cleared by host partition reset.
        UINT32 SPARE4:1; // 7 0h RW/L Spare4 (SPARE4): Once set to 1 this register is only cleared by host partition reset.
        UINT32 PR0LOCKDN:1; // 8 0h RW/L PR0 Lock-Down (PR0LOCKDN): BIOS PR0 register is locked when the logical OR of this bit and FLOCKDN is true. Once set to 1 this register is only cleared by BIOS partition reset.
        UINT32 PR1LOCKDN:1; // 9 0h RW/L PR1 Lock-Down (PR1LOCKDN): BIOS PR1 register is locked when the logical OR of this bit and FLOCKDN is true. Once set to 1 this register is only cleared by BIOS partition reset.
        UINT32 PR2LOCKDN:1; // 10 0h RW/L PR2 Lock-Down (PR2LOCKDN): BIOS PR2 register is locked when the logical OR of this bit and FLOCKDN is true. Once set to 1 this register is only cleared by BIOS partition reset.
        UINT32 PR3LOCKDN:1; // 11 0h RW/L PR3 Lock-Down (PR3LOCKDN): BIOS PR3 register is locked when the logical OR of this bit and FLOCKDN is true. Once set to 1 this register is only cleared by BIOS partition reset.
        UINT32 PR4LOCKDN:1; // 12 0h RW/L PR4 Lock-Down (PR4LOCKDN): BIOS PR4 register is locked when the logical OR of this bit and FLOCKDN is true. Once set to 1 this register is only cleared by BIOS partition reset.
        UINT32 SPARE3:1; // 13 0h RW/L Spare3 (SPARE3): Once set to 1 this register is only cleared by host partition reset.
        UINT32 SPARE2:1; // 14 0h RW/L Spare2 (SPARE2): Once set to 1 this register is only cleared by host partition reset.
        UINT32 SPARE1:1; // 15 0h RW/L Spare1 (SPARE1): Once set to 1 this register is only cleared by host partition reset.
        UINT32 SSEQLOCKDN:1; // 16 0h RW/L SSEQ Lock-Down (SSEQLOCKDN): BIOS Software Sequencing registers are locked when the logical OR of this bit and FLOCKDN is true. The affected registers are SSFSTS_CTL.SCF, PREOP_OPTYPE, OPMENU0, and OPMENU1. Once set to 1 this register is only cleared by host partition reset.
        UINT32 Reserved:15; //31:17 0h RO Reserved.
    } DLOCK; // 0C Discrete Lock Bits
    UINT32 FDATA[FDATA_COUNT]; // 10 - 4F Flash Data 0-0xF Once the read cycle is complete, these registers will be filled with the raw bytes as read from the flash memory.
    struct {
        UINT32 BRRA:8; // 7:0 C2h RO/V BIOS Region Read Access (BRRA): BIOS Region Read Access (BRRA): Each bit [7:0] corresponds to Regions [7:0]. 
        UINT32 BRWA:8; // 15:8 42h RO/V BIOS Region Write Access (BRWA): BIOS Region Write Access (BRWA): Each bit [15:8] corresponds to Regions [7:0]. 
        UINT32 BMRAG:8; // 23:16 0h RW/L BIOS Master Read Access Grant (BMRAG): BIOS Master Read Access Grant (BMRAG): Each bit [23:16] corresponds to Master[7:0]. BIOS can grant one or more masters read access to the BIOS region 1 overriding the read permissions in the Flash Descriptor. The contents of this register are locked by the FLOCKDN bit
        UINT32 BMWAG:8; // 31:24 0h RW/L BIOS Master Write Access Grant (BMWAG): BIOS Master Write Access Grant (BMWAG): Each bit [31:24] corresponds to Master[7:0]. 
    } FRACC; // 50 Flash Region Access Permission
    BIOS_REGION FREG[SPI_RAM_NR_FLASH_REGIONS]; // 54,58,5c,60,64,68 Flash Region 0-5
    UINT8 Reserved1[0x18]; // 6c possibly more FREGs. 0x18/4 = 6, i.e. Flash Region 6-B
    FPR FPR0; // 84 Flash Protected Range
    FPR FPR1; // 88 Flash Protected Range
    FPR FPR2; // 8c Flash Protected Range
    FPR FPR3; // 90 Flash Protected Range
    FPR FPR4; // 94 Flash Protected Range
    GPR GPR0; // 98 Global Protected Range
    UINT8 Reserved2[0x14]; // 9c
    struct {
        UINT32 Reserved:16; // 15:0 0h RO Reserved.
        UINT32 SECONDARYBIOS_MRAG:8; // 23:16 0h RW/L Secondary BIOS Master Read Access Grant (SECONDARYBIOS_MRAG): Each bit 28:16 corresponds to Master7:0. BIOS can grant one or more masters read access to the Secondary BIOS region 6 overriding the read permissions in the Flash Descriptor. Bits for unassigned masters are reserved. The contents of this register are locked by the FLOCKDN bit.
        UINT32 SECONDARYBIOS_MWAG:8; // 31:24 0h RW/L Secondary BIOS Master Write Access Grant (SECONDARYBIOS_MWAG): Each bit 31:29 corresponds to Master7:0. BIOS can grant one or more masters write access to the Secondary BIOS region 6 overriding the permissions in the Flash Descriptor. Bits for unassigned masters are reserved. The contents of this register are locked by the FLOCKDN bit.
    } SFRACC; // B0 Secondary Flash Region Access Permissions
    struct {
        UINT32 Reserved1:2; // 1:0 0h RO Reserved
        UINT32 FDSI:10; // 11:2 0h RW Flash Descriptor Section Index (FDSI): Selects the DW offset within the Flash  Descriptor Section to observe. 
        UINT32 FDSS:3; // 14:12 0h RW Flash Descriptor Section Select (FDSS): Selects which section within the loaded Flash Descriptor to observe.
                    //000: Flash Signature and Descriptor Map
                    //001: Component
                    //010: Region
                    //011: Master 
                    //Others: Reserved 
        UINT32 Reserved2:17; // 31:15 0h RO Reserved.
    } FDOC; // B4 Flash Descriptor Observability Control
    struct {
        UINT32 FDSD:32; // : Returns the DW of data to observe as selected in the Flash Descriptor Observability Control.
    } FDOD; // B8 Flash Descriptor Observability Data
    UINT32 Reserved3; // BC 
    struct {
        UINT32 SPFP:1; // 0 0h RW/V/P Stop Prefetch on Flush Pending (SPFP): When set to 1, the in progress of a prefetch will be ended if subsequence access from the master of the same interface is detected to be a cache-miss and read cache will be flushed.When set to 0, the prefetch will be allowed to complete prior to flushing
        UINT32 Reserved1:31; // 31:1 0h RO Reserved.
    } AFC; // C0 Additional Flash Control
    struct {
        UINT32 Reserved1:2; // 1:0 0h RO Reserved.
        UINT32 WG:1; // 2 0h RW/V/L Write Granularity (WG): 0 : Reserved 1 : 64 Byte This register is locked by the Vendor Component Lock (VCL) bit or the CPPTV bit. Note: Hardware ignores the state of this bit.
        UINT32 WSR:1; // 3 0h RW/V/L Write Status Required (WSR): 0 = No requirement to write to the Write Status Register prior to a write 1 = A write is required to the Write Status Register prior to write and erase to remove any protection. This is required for SST components. This register is locked by the Vendor Component Lock (VCL) bit or the CPPTV bit. Note: Hardware ignores the state of this bit.
        UINT32 WEWS:1; // 4 0h RW/V/L Write Enable on Write Status (WEWS): 0 = 50h is the opcode to enable a status register write 1 : 06h is the opcode to enable a status register write This register is locked by the Vendor Component Lock (VCL) bit or the CPPTV bit. Note: Hardware ignores the state of this bit.
        UINT32 QER:3; // 7:5 0h RW/V/L Quad Enable Requirements (QER): 000 = Part does not require a Quad Enable bit to be set, either because Quad is not supported or because the manufacturer somehow permanently enables Quad capability.
                //001 = Part requires bit 9 in status register 2 to be set to enable quad IO. Writing one byte to status register clears all bits in register 2, therefore status register writes MUST be two bytes.
                //010 = Part requires bit 6 of status register 1 to be set to enable quad IO.
                //011 = Part requires bit 7 of the configuration register to be set to enable Quad.
                //100 = Part requires bit 9 in status register 2 to be set to enable quad IO.
                //Writing one byte to the status register does not clear the second byte.This register is locked by the Vendor Component Lock (VCL) bit. If the SFDP table contains this information, the flash controller loads these bits from the table. The flash controller uses this information to prevent clearing the QE bit in the flash device's status register when WSR=1.
        UINT32 EO_4k:8; // 15:8 20h RW/V/L 4k Erase Opcode (EO_4k): This register is programmed with the Flash 64k sector  erase instruction opcode for component 0. This register is locked by the Vendor Component Lock (VCL) bit or the CPPTV bit.
        UINT32 EO_64k:8; // 23:16 0h RW/V/L 64k Erase Opcode (EO_64k): This register is programmed with the Flash 64k sector erase instruction opcode for component 0. This register is locked by the Vendor Component Lock (VCL) bit or the CPPTV bit.
        UINT32 SOFT_RST_SUPPORTED:1; // 24 0h RW/V/L Soft Reset Supported (SOFT_RST_SUPPORTED): 0: The device does not support Soft Reset. 1: The device supports Soft Reset.
        UINT32 SUSPEND_RESUME_SUPPORTED:1; // 25 0h RW/V/L Suspend/Resume Supported (SUSPEND_RESUME_SUPPORTED): 0: The device does not support Suspend/Resume. 1: The device supports Suspend/Resume.
        UINT32 DEEP_PWRDN_SUPPORTED:1; // 26 0h RW/V/L Deep Powerdown Supported (DEEP_PWRDN_SUPPORTED): 0: The device does not support Deep Powerdown. 1: The device supports Deep Powerdown.
        UINT32 RPMC_SUPPORTED:1; // 27 0h RW/L RPMC Supported (RPMC_SUPPORTED): 0: The device does not support RPMC. 1: The device supports RPMC.
        UINT32 EO_4k_VALID:1; // 28 0h RW/V/L 4k Erase Valid (EO_4k_VALID): 0: The EO_4k opcode is not valid. 1: The EO_4k opcode is valid.
        UINT32 EO_64k_VALID:1; // 29 0h RW/V/L 64k Erase Valid (EO_64k_VALID): 0: The EO_64k opcode is not valid. 1: The EO_64k opcode is valid.
        UINT32 VCL:1; // 30 0h RW/L Vendor Component Lock (VCL): 0: The lock bit is not set 1: The Vendor Component Lock bit is set. This register locks itself when set.
        UINT32 CPPTV:1; // 31 0h RO/V Component Property Parameter Table Valid (CPPTV): This bit is set to a 1 if the Flash Controller detects a valid SFDP Component Property Parameter Table in  Component 0. Note: If this bit is set software must not overwrite bits that were initialized by  hardware via SFDP discovery.
    } SFDP0_VSCC0; // C4 Vendor Specific Component Capabilities for Component 0
    struct {
        UINT32 Reserved1:2; // 1:0 0h RO Reserved.
        UINT32 WG:1; // 2 0h RW/V/L Write Granularity (WG): 0 : Reserved 1 : 64 Byte This register is locked by the Vendor Component Lock (VCL) bit or the CPPTV bit. Note: Hardware ignores the state of this bit.
        UINT32 WSR:1; // 3 0h RW/V/L Write Status Required (WSR): 0 = No requirement to write to the Write Status Register prior to a write 1 = A write is required to the Write Status Register prior to write and erase to remove any protection. This is required for SST components. This register is locked by the Vendor Component Lock (VCL) bit or the CPPTV bit. Note: Hardware ignores the state of this bit.
        UINT32 WEWS:1; // 4 0h RW/V/L Write Enable on Write Status (WEWS): 0 = 50h is the opcode to enable a status register write 1 : 06h is the opcode to enable a status register write This register is locked by the Vendor Component Lock (VCL) bit or the CPPTV bit. Note: Hardware ignores the state of this bit.
        UINT32 QER:3; // 7:5 0h RW/V/L Quad Enable Requirements (QER): 000 = Part does not require a Quad Enable bit to be set, either because Quad is not supported or because the manufacturer somehow permanently enables Quad capability.
                //001 = Part requires bit 9 in status register 2 to be set to enable quad IO. Writing one byte to status register clears all bits in register 2, therefore status register writes MUST be two bytes.
                //010 = Part requires bit 6 of status register 1 to be set to enable quad IO.
                //011 = Part requires bit 7 of the configuration register to be set to enable Quad.
                //100 = Part requires bit 9 in status register 2 to be set to enable quad IO.
                //Writing one byte to the status register does not clear the second byte.This register is locked by the Vendor Component Lock (VCL) bit. If the SFDP table contains this information, the flash controller loads these bits from the table. The flash controller uses this information to prevent clearing the QE bit in the flash device's status register when WSR=1.
        UINT32 EO_4k:8; // 15:8 20h RW/V/L 4k Erase Opcode (EO_4k): This register is programmed with the Flash 64k sector  erase instruction opcode for component 0. This register is locked by the Vendor Component Lock (VCL) bit or the CPPTV bit.
        UINT32 EO_64k:8; // 23:16 0h RW/V/L 64k Erase Opcode (EO_64k): This register is programmed with the Flash 64k sector erase instruction opcode for component 0. This register is locked by the Vendor Component Lock (VCL) bit or the CPPTV bit.
        UINT32 SOFT_RST_SUPPORTED:1; // 24 0h RW/V/L Soft Reset Supported (SOFT_RST_SUPPORTED): 0: The device does not support Soft Reset. 1: The device supports Soft Reset.
        UINT32 SUSPEND_RESUME_SUPPORTED:1; // 25 0h RW/V/L Suspend/Resume Supported (SUSPEND_RESUME_SUPPORTED): 0: The device does not support Suspend/Resume. 1: The device supports Suspend/Resume.
        UINT32 DEEP_PWRDN_SUPPORTED:1; // 26 0h RW/V/L Deep Powerdown Supported (DEEP_PWRDN_SUPPORTED): 0: The device does not support Deep Powerdown. 1: The device supports Deep Powerdown.
        UINT32 RPMC_SUPPORTED:1; // 27 0h RW/L RPMC Supported (RPMC_SUPPORTED): 0: The device does not support RPMC. 1: The device supports RPMC.
        UINT32 EO_4k_VALID:1; // 28 0h RW/V/L 4k Erase Valid (EO_4k_VALID): 0: The EO_4k opcode is not valid. 1: The EO_4k opcode is valid.
        UINT32 EO_64k_VALID:1; // 29 0h RW/V/L 64k Erase Valid (EO_64k_VALID): 0: The EO_64k opcode is not valid. 1: The EO_64k opcode is valid.
        UINT32 VCL:1; // 30 0h RW/L Vendor Component Lock (VCL): 0: The lock bit is not set 1: The Vendor Component Lock bit is set. This register locks itself when set.
        UINT32 CPPTV:1; // 31 0h RO/V Component Property Parameter Table Valid (CPPTV): This bit is set to a 1 if the Flash Controller detects a valid SFDP Component Property Parameter Table in  Component 0. Note: If this bit is set software must not overwrite bits that were initialized by  hardware via SFDP discovery.
    } SFDP1_VSCC1; // C8 Vendor Specific Component Capabilities for Component 1
    UINT32 PTINX; // CC Parameter Table Index (BIOS_PTINX) 
    UINT32 PTDATA; // D0 Parameter Table Data (BIOS_PTDATA)
    struct {
        UINT32 TPM_ACC_ONG:1; // 31 0h RO/V TPM Access Ongoing (TPM_ACC_ONG)  
        UINT32 ESPI_ACC_ONG:1; // 30 0h RO/V eSPI Access Ongoing (ESPI_ACC_ONG): This bit is only defined if eSPI and SPI  are sharing the SPI bus.
        UINT32 Reserved:12; // 29:18 0h RO Reserved.
        UINT32 M5STATUS:3; // 17:15 0h RO/V Master 5 Status (M5STATUS): See description under M1STATUS.
        UINT32 M6STATUS:3; // 14:12 0h RO/V Master 6 Status (M6STATUS): See description under M1STATUS.
        UINT32 M4STATUS:3; // 11:9 0h RO/V Master 4 Status (M4STATUS): See description under M1STATUS.
        UINT32 M3STATUS:3; // 8:6 0h RO/V Master 3 Status (M3STATUS): See description under M1STATUS.
        UINT32 M2STATUS:3; // 5:3 0h RO/V Master 2 Status (M2STATUS): See description under M1STATUS.
        UINT32 M1STATUS:3; // 2:0 0h RO/V Master 1 Status (M1STATUS): Indicates whether this master has an outstanding transaction enqueued or in flight and the transaction type.
                        // 0xx : no transaction
                        // 100 : flash read transaction
                        // 101 : flash write transaction
                        // 110 : flash erase transaction
                        // 111 : flash RPMC transaction

    } SBRS; // D4 SPI Bus Requester Status (BIOS_SBRS)
} INTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP, *PINTEL_SERIES_300_SPI_REGISTER_ADDRESS_MAP;





#define ICH_FLASH_DESCRIPTOR_SIG_OFFSET (0x0)
#define PCH_FLASH_DESCRIPTOR_SIG_OFFSET (0x10)

// from: Intel® I/O Controller Hub 10 (ICH10) Family
//       io-controller-hub-10-family-datasheet.pdf
// ich style at offset 0x00
// pch style at offset 0x10
typedef struct _FLASH_DESCRIPTOR {
    UINT32 Signature; // 10 0x0ff0A55A
    struct {
        UINT32 FCBA:8; // 7:0 Flash Component Base Address (FCBA). This identifies address bits [11:4] for the  Component portion of the Flash Descriptor. Bits [24:12] and bits [3:0] are 0. A recommended FCBA is 01h.
        UINT32 NC:2; // 9:8 Number Of Components (NC). This field identifies the total number of Flash Components. Each supported Flash Component requires a separate chip select. 
                // 00 = 1 Component.
                // 01 = 2 Components.
                // All other settings = Reserved
        UINT32 Reserved1:6; // 15:10 Reserved
        UINT32 FRBA:8; // 23:16 Flash Region Base Address (FRBA). This identifies address bits [11:4] for the Region portion of the Flash Descriptor. Bits [24:12] and bits [3:0] are 0. A recommended FRBA is 04h.
        UINT32 NR:3; // 26:24 Number Of Regions (NR). This field identifies the total number of Flash Regions. This number is 0's based, so a setting of all 0's indicates that the only Flash region is region 0, the Flash Descriptor region.
        UINT32 Reserved2:5; // 31:27 Reserved
    } FLMAP0; // 14
    struct {
        UINT32 FMBA:8; // 7:0 Flash Master Base Address (FMBA). This identifies address bits [11:4] for the Master portion of the Flash Descriptor. Bits [24:12] and bits [3:0] are 0. A recommended FMBA is 06h.
        UINT32 NM:3; // 10:08 Number Of Masters (NM). This field identifies the total number of Flash Regions. This number is 0's based. 
        UINT32 Reserved1:5; // 15:11 Reserved 
        UINT32 FISBA:8; // 23:16 Flash ICH Strap Base Address (FISBA). This identifies address bits [11:4] for the ICH Strap portion of the Flash Descriptor. Bits [24:12] and bits [3:0] are 0. For validation purposes, the recommended FISBA is 10h
        UINT32 ISL:8; // 31:24 ICH Strap Length (ISL). Identifies the 1s based number of Dwords of ICH Straps to be read, up to 255 DWs (1KB) max. A setting of all 0's indicates there are no ICH DW straps.
    } FLMAP1; // 18
    struct {
        UINT32 FMSBA:8; // 7:0 Flash (G)MCH Strap Base Address (FMSBA). This identifies address bits [11:4] for the (G)MCH Strap portion of the Flash Descriptor. Bits [24:12] and bits [3:0] are 0. A recommended FMSBA is 20h
        UINT32 MSL:8; // 15:08 MCH Strap Length (MSL). Identifies the 1's based number of Dwords of (G)MCH Straps to be read, up to 255 DWs (1KB) max. A setting of all 0's indicates there are no (G)MCH DW straps.
        UINT32 Reserved1:16; // 31:16 Reserved
    } FLMAP2; // 1c
} FLASH_DESCRIPTOR, *PFLASH_DESCRIPTOR;

// pch style
// seems to be the same, just at offset 0x10
// address offsets in data are relativ to 0, to signature 0x10
typedef struct _FLASH_DESCRIPTOR_PCH {
    UINT32 Reserved1[0x4]; // 00 FFs
    FLASH_DESCRIPTOR FD; // 10
} FLASH_DESCRIPTOR_PCH, *PFLASH_DESCRIPTOR_PCH;


// check signature of flash descriptor
FORCEINLINE
BOOLEAN isValidFlashDescriptor(PVOID Buffer)
{
    PFLASH_DESCRIPTOR fd = (PFLASH_DESCRIPTOR)((PUINT8)Buffer + ICH_FLASH_DESCRIPTOR_SIG_OFFSET);
    if ( fd->Signature != FLASH_DESRIPTOR_MODE_SIG )
    {
        fd = (PFLASH_DESCRIPTOR)((PUINT8)Buffer + PCH_FLASH_DESCRIPTOR_SIG_OFFSET);
        if ( fd->Signature != FLASH_DESRIPTOR_MODE_SIG )
        {
            return FALSE;
        }
    }
    return TRUE;
}

// from 200 series data sheet
// 300 has 6 (b110), if mapped correctly
#define COMPONENT_DENSITY_STR(__d__) \
      (__d__==0b000) ? "512 KB" \
    : (__d__==0b001) ? "1 MB" \
    : (__d__==0b010) ? "2 MB" \
    : (__d__==0b011) ? "4 MB" \
    : (__d__==0b100) ? "8 MB" \
    : (__d__==0b101) ? "16 MB" \
    : (__d__==0b110) ? "32 MB" \
    : "Reserved" /* 110 guessed, 111 */

// from 200 series data sheet
// 300 has 6 (b110), if mapped correctly
#define CLOCK_FREQ_STR(__f__) \
      (__f__==0b000) ? "20 MHz" \
    : (__f__==0b001) ? "33 MHz" \
    : (__f__==0b010) ? "50 MHz" \
    : (__f__==0b011) ? "66 MHz" \
    : "Reserved" /* 10, 11 guessed,..., 111 */

// from 200 series data sheet
typedef struct _FLASH_DESCRIPTOR_COMPONENT_SECTION {
    struct {
        UINT32 Component1Density:3; // 2:0 Component 1 Density. This field identifies the size of the 1st or only Flash component. 
        UINT32 Component2Density:3; // 5:3 Component 2 Density. This field identifies the size of the 2nd Flash component. If there is not 2nd Flash component, the contents of this field are undefined.
                                    // 000 = 512 KB
                                    // 001 = 1 MB
                                    // 010 = 2 MB
                                    // 011 = 4 MB
                                    // 100 = 8 MB
                                    // 101 = 16 MB
                                    // 111 = Reserved
        UINT32 Reserved1:11; // 16:6 Reserved
        UINT32 ReadClockFrequency:3; // 19:17 Read Clock Frequency.
                                    // 000 = 20 MHz
                                    // All other Settings = Reserved 
        UINT32 FastReadSupport:1; // 20 Fast Read Support.
        UINT32 FastReadClockFrequency:3; // 23:21 Fast Read Clock Frequency. This field identifies the frequency that can be used with the Fast Read instruction. This field is undefined if the Fast Read Support field is '0'.
        UINT32 WriteClockFrequency:3; // 26:24 Write and Erase Clock Frequency.
        UINT32 ReadIDAndStatusClockFrequency:3; // 29:27 Read ID and Read Status Clock Frequency.
                                    // 000 = 20 MHz
                                    // 001 = 33 MHz 
                                    // All other Settings = Reserved
        UINT32 Reserved:2; // 31:30 Reserved
    } FLCOMP; // 00 Flash Components Register
    struct {
        UINT32 InvalidInstruction0:8; // 7:0 Invalid Instruction 0. Op-code for an invalid instruction in the that the Flash Controller should protect against such as Chip Erase. This byte should be set to 0 if 
        UINT32 InvalidInstruction1:8; // 15:8 Invalid Instruction 1. See definition of Invalid Instruction 0
        UINT32 InvalidInstruction2:8; // 23:16 Invalid Instruction 2. See definition of Invalid Instruction 0
        UINT32 InvalidInstruction3:8; // 31:24 Invalid Instruction 3. See definition of Invalid Instruction 0
    } FLILL; // 04 Flash Invalid Instructions Register
    struct {
        UINT32 FPBA:13; // 12:0 Flash Partition Boundary Address (FPBA). This register specifies Flash Boundary 
                        //Address bits[24:12] that logically divides the flash space into two partitions, a lower 
                        //and an upper partition. The lower and upper partitions can support SPI flash parts with 
                        //different attributes between partitions that are defined in the LVSCC and UVSCC.
                        //NOTE: If this register is set to all 0s, then there is only one partition.
        UINT32 Reserved:19; // 31:13 Reserved
    } FLPB; // 08 Flash Partition Boundary Register
} FLASH_DESCRIPTOR_COMPONENT_SECTION, *PFLASH_DESCRIPTOR_COMPONENT_SECTION;


typedef struct _FLASH_DESCRIPTOR_REGION_SECTION {
    BIOS_REGION FLREG[1]; // Concrete number given in BIOS/UEFI FLASH_DESCRIPTOR.FLMAP0.NR
                    // At least there are the Flash Descritpor and BIOS
                    // + usually IntelMe and Gibabit Ethernet
                    // also known Platform Data and EC
} FLASH_DESCRIPTOR_REGION_SECTION, *PFLASH_DESCRIPTOR_REGION_SECTION;

#define FLMSTR_REQUESTER_ID_HOST_PROCESSOR (0)
#define FLMSTR_REQUESTER_ID_IME (0)
#define FLMSTR_REQUESTER_ID_GBE (0x218)
#define FLMSTR_REQUESTER_UNK_0 (0x900)
#define FLMSTR_REQUESTER_UNK_1 (0xB00)
#define FLMSTR_REQUESTER_UNK_2 (0xD00)

typedef struct _FLMSTR {
    UINT32 RequesterID:16; // 15:0 Requester ID. This is the Requester ID of the Host processor. This must be set to 0000h.
    UINT32 FlashDescriptorReadAccess:1; // 16 Flash Descriptor Region Read Access. If the bit is set, this master can read that particular region through register accesses.
    UINT32 HostCPUBIOSRegionReadAccess:1; // 17 Host CPU/BIOS Master Region Read Access. If the bit is set, this master can read that particular region through register accesses. Bit 17 is a don’t care as the primary master always has read/write permissions to it’s primary region
    UINT32 MERegionReadAccess:1; // 18 ME Region Read Access. If the bit is set, this master can read that particular region through register accesses.
    UINT32 GbERegionReadAccess:1; // 19 GbE Region Read Access. If the bit is set, this master can read that particular region through register accesses.
    UINT32 PlatformDataRegionReadAccess:1; // 20 Platform Data Region Read Access. If the bit is set, this master can read that particular region through register accesses.
    //UINT32 ECRegionReadAccess:1; // 21 EC Region Read Access. If the bit is set, this master can read that particular region through register accesses.
    UINT32 RES_23_21:3; // 23:21 Reserved, must be zero, one bit could be EC now
    UINT32 FlashDescriptorRegionWriteAccess:1; // 24 Flash Descriptor Region Write Access. If the bit is set, this master can erase and write that particular region through register accesses.
    UINT32 HostCPUBIOSMasterRegionWriteAccess:1; // 25 Host CPU/BIOS Master Region Write Access. If the bit is set, this master can erase and write that particular region through register accesses.Bit 25 is a don’t care as the primary master always has read/write permissions to it’s primary region
    UINT32 MERegionWriteAccess:1; // 26 ME Region Write Access. If the bit is set, this master can erase and write that particular region through register accesses.
    UINT32 GbERegionWriteAccess:1; // 27 GbE Region Write Access. If the bit is set, this master can erase and write that particular region through register accesses.
    UINT32 PlatformDataRegionWriteAccess:1; // 28 Platform Data Region Write Access. If the bit is set, this master can erase and write that particular region through register accesses.
    //UINT32 ECWriteAccess:1; // 29 EC Region Write Access. If the bit is set, this master can erase and write that particular region through register accesses.
    UINT32 RES_31_29:3; // 31:29 Reserved, must be zero, one bit could be EC now
} FLMSTR, *PFLMSTR;

typedef struct _FLASH_DESCRIPTOR_MASTER_SECTION {
    FLMSTR FlmStr[1]; // Concrete number given in FLASH_DESCRIPTOR.FLMAP1.NM
                      // BIOS
                      // ME
                      // GbE
                      // ..
} FLASH_DESCRIPTOR_MASTER_SECTION, *PFLASH_DESCRIPTOR_MASTER_SECTION;



//ich10   
//22.2.5 Descriptor Upper Map Section
//22.2.5.1 FLUMAP1—Flash Upper Map 1 (Flash Descriptor Registers)
//Memory Address: FDBAR + EFCh Default Value: 0000FFFFh
//Size: 32 bits
// Each VSCC table entry is composed of two 32 bit fields: JEDEC ID and the corresponding VSCC value
typedef struct _FLUMAP1 {
    UINT32 VTBA:8; // 7:0 ME_VSCC_Table_Base_Address (VTBA). This identifies address bits [11:4] for the VSCC Table portion of the Flash Descriptor. Bits [24:12] and bits [3:0] are 0. NOTE: VTBA should be above the offset for MCHSTRP0 and below FLUMAP1. It is recommended that this address is set based on the anticipated maximum number of different flash parts entries.
    UINT32 VTL:8; // 15:8 ME VSCC Table Length (VTL). Identifies the 1s based number of DWORDS contained in the VSCC Table. Each SPI component entry in the table is 2 DWORDS long.
    UINT32 RES_31_16:16; // 31:16 0 Reserved
} FLUMAP1, *PFLUMAP1;

//22.2.6 Intel ME Vendor Specific Component Capabilities Table
typedef struct _JEDEC_ID {
    UINT32 VENDOR_ID:8; // 7:0 SPI Component Vendor ID. This field identifies the one byte Vendor ID of the SPI Flash Component. This is the first byte returned by the Read JEDEC-ID command (opcode 9Fh).
    UINT32 DEVICE_ID_0:8; // 15:8 SPI Component Device ID 0. This field identifies the first byte of the Device ID of the SPI Flash Component. This is the second byte returned by the Read JEDEC-ID command (opcode 9Fh).
    UINT32 DEVICE_ID_1:8; // 23:16 SPI Component Device ID 1. This field identifies the second byte of the Device ID of the SPI Flash Component. This is the third byte returned by the Read JEDEC-ID command (opcode 9Fh).
    UINT32 RES_31_24:8; // 31:24 Reserved
} JEDEC_ID, *PJEDEC_ID;

FORCEINLINE
PCHAR UBES_STR(UINT32 Ubes)
{
    switch ( Ubes )
    {
        case 0: return "256 Bytes";
        case 1: return "4 Bytes";
        case 2: return "8 Bytes";
        case 3: return "64 Bytes";
        default: return "Unknown";
    }
}
FORCEINLINE
PCHAR UWG_STR(UINT32 UWG)
{
    switch ( UWG )
    {
        case 0: return "1 Byte";
        case 1: return "64 Bytes";
        default: return "Unknown";
    }
}
FORCEINLINE
PCHAR LBES_STR(UINT32 LBES)
{
    switch ( LBES )
    {
        case 0: return "256 Bytes";
        case 1: return "4 Bytes";
        case 2: return "8 Bytes";
        case 3: return "64 Bytes";
        default: return "Unknown";
    }
}
FORCEINLINE
PCHAR LWG_STR(UINT32 LWG)
{
    switch ( LWG )
    {
        case 0: return "1 Byte";
        case 1: return "64 Bytes";
        default: return "Unknown";
    }
}

typedef struct _VSCC {
    UINT32 UBES:2; // 1:0 Upper Block/Sector Erase Size (UBES). This field identifies the erasable sector size for all Flash components. 00 = 256 Bytes, 01 = 4 KB, 10 = 8 KB, 11 = 64 KB
    UINT32 UWG:1; // 2 Upper Write Granularity (UWG). 0 = 1 Byte, 1 = 64 Bytes
    UINT32 UWSR:1; // 3 Upper Write Status Required (UWSR). 0 = No requirement to write to the Status Register prior to a write 1 = A write of 00h to the SPI flash’s status register is required prior to write and erase to unlock the flash component. 50h is the opcode used to unlock the Status  register. NOTES: 1. Bit 4 and bit 3 should NOT be both set to 1. 2. Bit 3 should not be set if the flash part does not support the opcode 50h to unlock the status register.
    UINT32 UWEWS:1; // 4 Upper Write Enable on Write Status (UWEWS). 0 = No write to the SPI flash’s status register required prior to a write 1 = A write of 00h to the SPI flash’s status register is required prior to write and erase to unlock the flash component. 06h is the opcode used to unlock the Status register. NOTES: 1. This bit should not be set to 1 if the SPI flash status register is non-volatile. This may lead to premature flash wear out. 2. Bit 4 and bit 3 should NOT be both set to 1.
    UINT32 RES_7_5:3; // 7:5 Reserved
    UINT32 UEO:8; // 15:8 Upper Erase Opcode (UEO). This register must be programmed with the Flash erase instruction opcode that corresponds to the erase size that is in LBES.
    UINT32 LBES:2; // 17:16 Lower Block/Sector Erase Size (LBES). This field identifies the erasable sector size for all Flash space below the flash partition boundary address. 00 = 256 Byte01 = 4 KB, 10 = 8 KB, 11 = 64 KB
    UINT32 LWG:1; // 18 Lower Write Granularity (LWG). 0 = 1 Byte, 1 = 64 Byte
    UINT32 LWSR:1; // 19 Lower Write Status Required (LWSR). 0 = No requirement to write to the Status Register prior to a write, 1 = A write of 00h to the SPI flash’s status register is required prior to write and erase to unlock the flash component. 50h is the opcode used to unlock the Status register. NOTES: 1. Bit 20 and bit 19 should NOT be both set to 1. 2. Bit 19 should not be set if the flash part does not support the opcode 50h to unlock the status register.
    UINT32 LWEWS:1; // 20 Lower Write Enable on Write Status (LWEWS). 0 = No write to the SPI flash’s status register required prior to a write, 1 = A write of 00h to the SPI flash’s status register is required prior to write and erase to unlock the flash component. 06h is the opcode used to unlock the Status register. NOTES: 1. This bit should not be set to 1 if the SPI flash status register is non-volatile. This may lead to premature flash wear out. 2. Bit 20 and bit 19 should NOT be both set to 1.
    UINT32 RES_23_21:3; // 23:21 Reserved
    UINT32 LEO:8; // 31:24 Lower Erase Opcode (LEO). This register must be programmed with the Flash erase instruction opcode that corresponds to the erase size that is in LBES.
} VSCC, *PVSCC;


//
//
//22.3 OEM Section
//Memory Address: F00h Default Value:
//Size: 256 Bytes
