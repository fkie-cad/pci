#pragma once

//
// Maximum lengths in bytes
//

#define ACPI_MAX_SIGNATURE       4
#define ACPI_MAX_OEM_ID          6
#define ACPI_MAX_TABLE_ID        8
#define ACPI_MAX_CREATOR_ID      4

#pragma pack(1)

typedef struct _DESCRIPTION_HEADER {        // Header structure appears at the beginning of each ACPI table
    ULONG   Signature;                      //      Signature used to identify the type of table
    ULONG   Length;                         //      Length of entire table including the DESCRIPTION_HEADER
    UCHAR   Revision;                       //      Minor version of ACPI spec to which this table conforms
    UCHAR   Checksum;                       //      sum of all bytes in the entire TABLE should = 0
    CHAR    OEMID[ACPI_MAX_OEM_ID];         //      String that uniquely ID's the OEM
    CHAR    OEMTableID[ACPI_MAX_TABLE_ID];  //      String that uniquely ID's this table (used for table patching and replacement).
    ULONG   OEMRevision;                    //      OEM supplied table revision number.  Bigger number = newer table.
    CHAR    CreatorID[ACPI_MAX_CREATOR_ID]; //      Vendor ID of utility which created this table.
    ULONG   CreatorRev;                     //      Revision of utility that created the table.
} DESCRIPTION_HEADER, *PDESCRIPTION_HEADER;

// Configuration space base address allocation structures. 
typedef struct _CSBAA { 
    PVOID BaseAddress; // 0 8 Base address of enhanced configuration mechanism
    UINT16 SegmentGroupNr; // 8 2 PCI Segment Group Number
    UINT8 StartBusNr; // 10 1 Start PCI bus number decoded by this host bridge
    UINT8 EndBusNr; // 11 1 End PCI bus number decoded by this host bridge
    UINT32 Reserved; // 12 4 Reserved
} CSBAA, *PCSBAA;


// Memory-mapped Configuration
typedef struct _MCFG {
    DESCRIPTION_HEADER Header;
    UINT64 Reserved;
    CSBAA csbaa[1];
} MCFG, *PMCFG;

#pragma pack()
