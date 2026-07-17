#pragma once

#pragma pack(1)

typedef struct _SFDP_PARAMETER_HEADER {
    UINT8 JEDEC_ID; // 00
    UINT8 MinorVersion; // 01
    UINT8 MajorVersion; // 02
    UINT8 Length; // 03
    UINT8 PTP[3]; // 04 Parameter Table Pointer
    UINT8 res8_07; // 07 set to 0xFF
} SFDP_PARAMETER_HEADER, *PSFDP_PARAMETER_HEADER; // 08

typedef struct _SFDP {
    union {
        UINT8 Signature8[4];
        UINT32 Signature32;
    }; // 00 "SFDP" Signature
    UINT8 MinorVersion; // 04
    UINT8 MajorVersion; // 05
    UINT8 NrOfParamHeaders; // 06 This number is 0-based. Therefore, 0 indicates 1 parameter header
    UINT8 res8_07; // 07
    SFDP_PARAMETER_HEADER ParamHeader[1];
} SFDP, *PSFDP; // 10++

// parameter table entries

// JESD216.pdf
//typedef struct _SFDP_JEDEC_STD {
//31:23 Unused Contains 0xFF and can never be changed.
//22
//Supports (1-1-4) Fast Read 
//Device supports single input opcode & address and quad output data Fast Read.
//0: (1-1-4) Fast Read NOT supported.
//1: (1-1-4) Fast Read supported.
//21
//Supports (1-4-4) Fast Read 
//Device supports single input opcode, quad input address, and quad output data Fast 
//Read.
//0: (1-4-4) Fast Read NOT supported.
//1: (1-4-4) Fast Read supported.
//20
//Supports (1-2-2) Fast Read 
//Device supports single input opcode, dual input address, and dual output data Fast Read.
//0: (1-2-2) Fast Read NOT supported.
//1: (1-2-2) Fast Read supported.
//19
//Supports Double Transfer Rate (DTR) Clocking 
//Indicates the device supports some type of double transfer rate clocking. 
//0: DTR NOT supported
//1: DTR Clocking supported
//18:17
//Address Bytes 
//Number of bytes used in addressing flash array read, write and erase:
//00: 3-Byte only addressing
//01: 3- or 4-Byte addressing (e.g. defaults to 3-Byte mode; enters 4-Byte mode on 
//command)
//10: 4-Byte only addressing
//11: Reserved
//NOTE All flash devices under 128 megabits in size should use 00b for this value for 24 bit 
//addressing. This field refers to the number of address bits/bytes that are clocked in for any 
//instruction requiring an address in the flash array. This field does not pertain to SFDP Header or 
//Table accesses; all SFDP accesses use 3-byte addressing. 
//Examples: Read, Fast Read, Write, 4 kilobyte Erase
//   16
//Supports (1-1-2) Fast Read 
//Device supports single input opcode & address and dual output data Fast Read with 8 
//bits of wait states.
//0: (1-1-2) Fast Read NOT supported.
//1: (1-1-2) Fast Read supported.
//15:8 
//4 Kilobyte Erase Opcode 
//Note: If 4 kilobyte erase is not supported, then enter 0xFF.
// 7:5 
//Unused 
//Contains 111b and can never be changed.
//4 
//Write Enable Opcode Select for Writing to Volatile Status Register 
//0: 0x50 is the Opcode to enable a status register write when bit 3 is set to 1. 
//1: 0x06 is the Opcode to enable a status register write when bit 3 is set to 1. 
//NOTE If target flash status register is nonvolatile, then bits 3 and 4 must be set to 00b. 
//3 
//Write Enable Instruction Required for Writing to Volatile Status Register 
//0: Target flash has nonvolatile status bit and does not require status register to be written on every power on to allow writes and erases. 
//1: Target flash requires a 0x00 to be written to the status register in order to allow writes 
//and erases. 
//NOTE If target flash register is nonvolatile, then bits 3 and 4 must be set to 00b.
//2 
//Write Granularity 
//0: 1 Byte – Use this setting for single byte programmable devices or buffer 
//programmable devices when the buffer is less than 64 bytes (32 Words). 
//1: Use this setting for buffer programmable devices when the buffer size is 64 bytes (32 Words) or larger. 
//1:0 
//Block/Sector Erase Sizes 
//Identifies the erase granularity for all Flash Components. 
//00: Reserved 
//01: 4 kilobyte Erase 
//10: Reserved 
//11: Use this setting only if the 4 kilobyte erase is unavailable.
//
//...
//} SFDP_JEDEC_STD, *PSFDP_JEDEC_STD;

#pragma pack()
