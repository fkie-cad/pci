#pragma once

#define READ_CFG_MODE_NONE (0x0)
#define READ_CFG_MODE_PORT (0x1)
#define READ_CFG_MODE_MM   (0x2)

#pragma pack(1)

typedef struct _PCI_READ_IN_FLAGS {
    UINT32 ReadMode:2;
    UINT32 ReadBars:1;
    UINT32 ReadCapabilities:1;
    UINT32 Print:1;
    UINT32 WriteToFile:1;
    UINT32 PCIe:1;
    UINT32 Reserved:26;
} PCI_READ_IN_FLAGS, *PPCI_READ_IN_FLAGS;

typedef struct _LIST_PCI_OUT {
    UINT32 Count;
    UINT32 Size;
    UINT32 BDFOffset;
    UINT32 NamesOffset;
    UINT8 Buffer[1];
} LIST_PCI_OUT, *PLIST_PCI_OUT,
  DUMP_PCI_OUT, *PDUMP_PCI_OUT;
#define LIST_PCI_OUT_DEF_SIZE ((UINT32)sizeof(LIST_PCI_OUT)-1)
#define DUMP_PCI_OUT_DEF_SIZE ((UINT32)sizeof(LIST_PCI_OUT)-1)

#pragma pack()
