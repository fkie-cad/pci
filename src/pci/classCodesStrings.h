#pragma once

//(?<name>(0x[A-Z_0-9]{1,}))( - )(?<desc>([a-zA-Z0-9_/ -\(\\,.)]){1,})((\t)(--)){0,}
//        case ${name}: return "${desc}";


#include "../inc/pci/classCodes.h"

PCHAR classCodeStr(UINT32 classCode)
{
    switch ( classCode )
    {
        case PCI_CLASS_PRE_20: return "PRE_20"; break;
        case PCI_CLASS_MASS_STORAGE_CTLR: return "Mass Storage Controller"; break;
        case PCI_CLASS_NETWORK_CTLR: return "Network Controller"; break;
        case PCI_CLASS_DISPLAY_CTLR: return "Display Controller"; break;
        case PCI_CLASS_MULTIMEDIA_DEV: return "Multimedia Device"; break;
        case PCI_CLASS_MEMORY_CTLR: return "Memory Controller"; break;
        case PCI_CLASS_BRIDGE_DEV: return "Bridge Device"; break;
        case PCI_CLASS_SIMPLE_COMMS_CTLR: return "Simple Communications Controllers"; break;
        case PCI_CLASS_BASE_SYSTEM_DEV: return "Base System Device"; break;
        case PCI_CLASS_INPUT_DEV: return "Input Device"; break;
        case PCI_CLASS_DOCKING_STATION: return "Docking Station"; break;
        case PCI_CLASS_PROCESSOR: return "Processor"; break;
        case PCI_CLASS_SERIAL_BUS_CTLR: return "Serial Bus Controller"; break;
        case PCI_CLASS_WIRELESS_CTLR: return "Wireless Controller"; break;
        case PCI_CLASS_INTELLIGENT_IO_CTLR: return "Intelligent IO Controller"; break;
        case PCI_CLASS_SATELLITE_COMMS_CTLR: return "Satellite Comms Controller"; break;
        case PCI_CLASS_ENCRYPTION_DECRYPTION: return "Encryption Decryption"; break;
        case PCI_CLASS_DATA_ACQ_SIGNAL_PROC: return "Data ACQ Signal Proc"; break;
        case PCI_CLASS_PROCESSING_ACCELERATOR: return "Pci Class Processing Accelerator"; break;
        case PCI_CLASS_NON_ESSENTIAL_INSTRUMENTATION: return "ICI Class Non Essential Instrumentation"; break;
        case PCI_CLASS_CO_PROCESSOR: return "PCI Class Co Pocessor"; break;
        case PCI_CLASS_NOT_DEFINED: return "PCI Class not Defined"; break;
        default: return "Unknown"; break;
    }
}

PCHAR subClassStr_PRE_20(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_PRE_20_NON_VGA: return "Non VGA";
        case PCI_SUBCLASS_PRE_20_VGA: return "VGA";
        default: return "Unknown";
    }
}

PCHAR subClassStr_MASS_STORAGE_CTLR(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_MSC_SCSI_BUS_CTLR: return "SCSI Bus Controller"; break;
        case PCI_SUBCLASS_MSC_IDE_CTLR: return "IDE Controller"; break;
        case PCI_SUBCLASS_MSC_FLOPPY_CTLR: return "Floppy Controller"; break;
        case PCI_SUBCLASS_MSC_IPI_CTLR: return "IPI Controller"; break;
        case PCI_SUBCLASS_MSC_RAID_CTLR: return "RAID Controller"; break;
        case PCI_SUBCLASS_MSC_AHCI_CTLR: return "AHCI Controller"; break;
        case PCI_SUBCLASS_MSC_NVM_CTLR: return "NVM Controller"; break;
        case PCI_SUBCLASS_MSC_OTHER: return ""; break;
        default: return "Unknown";
    }
}

PCHAR subClassStr_NETWORK_CTLR(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_NET_ETHERNET_CTLR: return "Ethernet Controller";
        case PCI_SUBCLASS_NET_TOKEN_RING_CTLR: return "Token Ring Controller";
        case PCI_SUBCLASS_NET_FDDI_CTLR: return "FDDI Controller";
        case PCI_SUBCLASS_NET_ATM_CTLR: return "ATM Controller";
        case PCI_SUBCLASS_NET_ISDN_CTLR: return "ISDN Controller";
        case 0x5: return "WorldFip Controller";
        case 0x6: return "PICMG 2.14 Multi Computing Controller";
        case 0x7: return "Infiniband Controller";
        case 0x8: return "Fabric Controller";
        case PCI_SUBCLASS_NET_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_MULTIMEDIA_DEV(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_MM_VIDEO_DEV: return "Video Device";
        case PCI_SUBCLASS_MM_AUDIO_DEV: return "Audio Device";
        case PCI_SUBCLASS_MM_TELEPHONY_DEV: return "Telephony Device";
        case PCI_SUBCLASS_MM_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_DISPLAY_CTLR(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_VID_VGA_CTLR: return "VGA Compatible Controller";
        case PCI_SUBCLASS_VID_XGA_CTLR: return "XGA Controller";
        case PCI_SUBLCASS_VID_3D_CTLR: return "3D Controller (Not VGA-Compatible)";
        case PCI_SUBCLASS_VID_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_MEMORY_CTLR(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_MEM_RAM: return "RAM Controller";
        case PCI_SUBCLASS_MEM_FLASH: return "Flash Controller";
        case PCI_SUBCLASS_MEM_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_BRIDGE_DEV(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_BR_HOST: return "Host Bridge";
        case PCI_SUBCLASS_BR_ISA: return "ISA Bridge";
        case PCI_SUBCLASS_BR_EISA: return "EISA Bridge";
        case PCI_SUBCLASS_BR_MCA: return "MCA Bridge";
        case PCI_SUBCLASS_BR_PCI_TO_PCI: return "PCI-to-PCI Bridge";
        case PCI_SUBCLASS_BR_PCMCIA: return "PCMCIA Bridge";
        case PCI_SUBCLASS_BR_NUBUS: return "NuBus Bridge";
        case PCI_SUBCLASS_BR_CARDBUS: return "CardBus Bridge";
        case PCI_SUBCLASS_BR_RACEWAY: return "RACEway Bridge";
        case 0x9: return "PCI-to-PCI Bridge";
        case 0x0A: return "InfiniBand-to-PCI Host Bridge";
        case PCI_SUBCLASS_BR_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_BASE_SYSTEM_DEV(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_SYS_INTERRUPT_CTLR: return "Interrupt Controller";
        case PCI_SUBCLASS_SYS_DMA_CTLR: return "DMA Controller";
        case PCI_SUBCLASS_SYS_SYSTEM_TIMER: return "System Timer";
        case PCI_SUBCLASS_SYS_REAL_TIME_CLOCK: return "Time Clock";
        case PCI_SUBCLASS_SYS_GEN_HOTPLUG_CTLR: return "Hotplug Controller";
        case PCI_SUBCLASS_SYS_SDIO_CTRL: return "SDIO Controller";
        case PCI_SUBCLASS_SYS_RCEC: return "RCEC";
        case PCI_SUBCLASS_SYS_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_INPUT_DEV(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_INP_KEYBOARD: return "Keyboard Controller";
        case PCI_SUBCLASS_INP_DIGITIZER: return "Digitizer Pen";
        case PCI_SUBCLASS_INP_MOUSE: return "Mouse Controller";
        case PCI_SUBCLASS_INP_SCANNER: return "Scanner Controller";
        case PCI_SUBCLASS_INP_GAMEPORT: return "Gameport Controller";
        case PCI_SUBCLASS_INP_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_DOCKING_STATION(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_DOC_GENERIC: return "Generic";
        case PCI_SUBCLASS_DOC_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_PROCESSOR(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_PROC_386: return "386";
        case PCI_SUBCLASS_PROC_486: return "486";
        case PCI_SUBCLASS_PROC_PENTIUM: return "Pentium";
        case 0x3: return "Pentium Pro";
        case PCI_SUBCLASS_PROC_ALPHA: return "Alpha";
        case PCI_SUBCLASS_PROC_POWERPC: return "PowerPC";
        case 0x30: return "MIPS";
        case PCI_SUBCLASS_PROC_COPROCESSOR: return "Co-Processor";
        case 0x80: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_SERIAL_BUS_CTLR(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_SB_IEEE1394: return "FireWire (IEEE 1394) Controller";
        case PCI_SUBCLASS_SB_ACCESS: return "ACCESS Bus Controller";
        case PCI_SUBCLASS_SB_SSA: return "SSA";
        case PCI_SUBCLASS_SB_USB: return "USB Controller";
        case PCI_SUBCLASS_SB_FIBRE_CHANNEL: return "Fibre Channel";
        case PCI_SUBCLASS_SB_SMBUS: return "SMBus Controller";
        case 0x6: return "InfiniBand Controller";
        case 0x7: return "IPMI Interface";
        case 0x8: return "SERCOS Interface (IEC 61491)";
        case 0x9: return "CANbus Controller";
        case PCI_SUBCLASS_SB_THUNDERBOLT: return "Thunderbolt Controller";
        case 0x80: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_WIRELESS_CTLR(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_WIRELESS_IRDA: return "iRDA Compatible Controller";
        case PCI_SUBCLASS_WIRELESS_CON_IR: return "Consumer IR Controller";
        case PCI_SUBCLASS_WIRELESS_RF: return "RF Controller";
        case 0x11: return "Bluetooth Controller";
        case 0x12: return "Broadband Controller";
        case 0x20: return "Ethernet Controller (802.1a)";
        case 0x21: return "Ethernet Controller (802.1b)";
        case PCI_SUBCLASS_WIRELESS_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_INTELLIGENT_IO_CTLR(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_INTIO_I2O: return "IntIO I20";
        default: return "Unknown";
    }
}

PCHAR subClassStr_SATELLITE_COMMS_CTLR(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_SAT_TV: return "TV";
        case PCI_SUBCLASS_SAT_AUDIO: return "Audio";
        case PCI_SUBCLASS_SAT_VOICE: return "Voice";
        case PCI_SUBCLASS_SAT_DATA: return "Data";
        default: return "Unknown";
    }
}

PCHAR subClassStr_ENCRYPTION_DECRYPTION(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_CRYPTO_NET_COMP: return "Network and Computing Encrpytion/Decryption";
        case PCI_SUBCLASS_CRYPTO_ENTERTAINMENT: return "Entertainment Encryption/Decryption";
        case PCI_SUBCLASS_CRYPTO_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr_DATA_ACQ_SIGNAL_PROC(UINT32 subClass)
{
    switch ( subClass )
    {
        case PCI_SUBCLASS_DASP_DPIO: return "DPIO";
        case PCI_SUBCLASS_DASP_OTHER: return "Other";
        default: return "Unknown";
    }
}

PCHAR subClassStr(UINT32 classCode, UINT32 subClass)
{
    switch ( classCode )
    {
        case PCI_CLASS_PRE_20: return subClassStr_PRE_20(subClass); break;
        case PCI_CLASS_MASS_STORAGE_CTLR: return subClassStr_MASS_STORAGE_CTLR(subClass); break;
        case PCI_CLASS_NETWORK_CTLR: return subClassStr_NETWORK_CTLR(subClass); break;
        case PCI_CLASS_DISPLAY_CTLR: return subClassStr_DISPLAY_CTLR(subClass); break;
        case PCI_CLASS_MULTIMEDIA_DEV: return subClassStr_MULTIMEDIA_DEV(subClass); break;
        case PCI_CLASS_MEMORY_CTLR: return subClassStr_MEMORY_CTLR(subClass); break;
        case PCI_CLASS_BRIDGE_DEV: return subClassStr_BRIDGE_DEV(subClass); break;
        //case PCI_CLASS_SIMPLE_COMMS_CTLR: return subClassStr_SIMPLE_COMMS_CTLR(subClass); break;
        case PCI_CLASS_BASE_SYSTEM_DEV: return subClassStr_BASE_SYSTEM_DEV(subClass); break;
        case PCI_CLASS_INPUT_DEV: return subClassStr_INPUT_DEV(subClass); break;
        case PCI_CLASS_DOCKING_STATION: return subClassStr_DOCKING_STATION(subClass); break;
        case PCI_CLASS_PROCESSOR: return subClassStr_PROCESSOR(subClass); break;
        case PCI_CLASS_SERIAL_BUS_CTLR: return subClassStr_SERIAL_BUS_CTLR(subClass); break;
        case PCI_CLASS_WIRELESS_CTLR: return subClassStr_WIRELESS_CTLR(subClass); break;
        case PCI_CLASS_INTELLIGENT_IO_CTLR: return subClassStr_INTELLIGENT_IO_CTLR(subClass); break;
        case PCI_CLASS_SATELLITE_COMMS_CTLR: return subClassStr_SATELLITE_COMMS_CTLR(subClass); break;
        case PCI_CLASS_ENCRYPTION_DECRYPTION: return subClassStr_ENCRYPTION_DECRYPTION(subClass); break;
        case PCI_CLASS_DATA_ACQ_SIGNAL_PROC: return subClassStr_DATA_ACQ_SIGNAL_PROC(subClass); break;
        //case PCI_CLASS_PROCESSING_ACCELERATOR: return "PCI_CLASS_PROCESSING_ACCELERATOR"; break;
        //case PCI_CLASS_NON_ESSENTIAL_INSTRUMENTATION: return "PCI_CLASS_NON_ESSENTIAL_INSTRUMENTATION"; break;
        //case PCI_CLASS_CO_PROCESSOR: return "PCI_CLASS_CO_PROCESSOR"; break;
        //case PCI_CLASS_NOT_DEFINED: return "PCI_CLASS_NOT_DEFINED"; break;
        default: return "Unknown"; break;
    }
}

//Class Code 0: Pre 2.0
//Sub-Class	Prog. I/F	Description
//0x00	0x00	All devices other than VGA
//0x01	0x01	VGA device

//Class Code 1: Mass Storage Controllers
//Sub-Class	Prog. I/F	Description
//0x00	0x00	SCSI Controller
//0x01	0xXX	IDE controller. The Prog I/F is defined as follows:
//bits	Desc
//0	Operating mode (primary)
//1	Programmable indicator (primary)
//2	Operating mode (secondary)
//3	Programmable indicator (secondary)
//6..4	Reserved (zero)
//7	Master IDE device
//0x02	0x00	Floppy disk controller
//0x03	0x00	IPI controller
//0x04	0x00	RAID controller
//0x80	0x00	Other mass storage controller

//Class Code 2: Network Controllers
//Sub-Class	Prog. I/F	Description
//0x00	0x00	Ethernet controller
//0x01	0x00	Token ring
//0x02	0x00	FDDI controller
//0x03	0x00	ATM controller
//0x80	0x00	Other network controller

//Class Code 3: Display Controllers
//Sub-Class	Prog. I/F	Description
//#define VGA_COMPATIBLE_CONTROLLER (0x0000) // 0x00 0x00 VGA compatible controller. Has mapping for 0xA0000..0xBFFFF and io addresses 0x3B0..0x3BB.
//#define _8514_VGA_COMPATIBLE_CONTROLLER (0x0001) // 0x00 0x01 8514 compatable
//#define XGA_CONTROLLER (0x0100) //0x01 0x00 XGA controller
//#define OTHER_DISPLAY_CONTROLLER (0x8080) //0x80 0x80 Other display controller


//Class Code 4: Multimedia Devices
//Sub-Class	Prog. I/F	Description
//0x00	0x00	Video device
//0x01	0x00	Audio device
//0x80	0x00	Other multimedia device

//Class Code 5: Memory Controllers
//Sub-Class	Prog. I/F	Description
//0x00	0x00	RAM controller
//0x01	0x00	Flash memory controller
//0x80	0x00	Other memory controller

//Class Code 6: Bridge Devices
//Sub-Class	Prog. I/F	Description
//#define HOST_PCI_BRIDGE (0x00) //0x00	0x00	Host/PCI bridge
//#define PCI_ISA_BRIDGE (0x) //0x01	0x00	PCI/ISA bridge
//#define PCI_EISA_BRIDGE (0x02) //0x02	0x00	PCI/EISA bridge
//#define PCI_MICRO_CHANNEL_BRIDGE (0x03) //0x03	0x00	PCI/Micro Channel bridge
//#define PCI_PCI_BRIDGE (0x04) //0x04	0x00	PCI/PCI bridge
//#define PCI_PCMCIA_BRIDGE (0x05) //0x05	0x00	PCI/PCMCIA bridge
//#define PCI_NUBUS_BRIDGE (0x06) //0x06	0x00	PCI/NuBus bridge
//#define PCI_CARDBUS_BRIDGE (0x07) //0x07	0x00	PCI/CardBus bridge
//#define OTHER_BRIDGE_TYPE (0x80) //0x80	0x00	Other bridge type

//Class Code 7: Simple Communications Controllers
//Sub-Class	Prog. I/F	Description
//0x00	0x00	Generic XT compatable serial controller
//0x01	16450 compatable serial controller
//0x02	16550 compatable serial controller
//0x01	0x00	Parallel port
//0x01	Bi-directional parallel port
//0x02	ECP 1.X parallel port
//0x80	0x00	Other commonications device
//Class Code 8: Base System Peripherals
//Sub-Class	Prog. I/F	Description
//0x00	0x00	Generic 8259 programmable interrupt controller (PIC)
//0x01	ISA PIC
//0x02	EISA PIC
//0x01	0x00	Generic 8237 DMA controller
//0x01	ISA DMA controller
//0x02	EISA DMA controller
//0x02	0x00	Generic 8254 timer
//0x01	ISA system timer
//0x02	EISA system timer
//0x03	0x00	Generic RTC controller
//0x01	ISA RTC controller
//0x80	0x00	Other system peripheral
//Class Code 9: Input Devices
//Sub-Class	Prog. I/F	Description
//0x00	0x00	Keyboard controller
//0x01	0x00	Digitizer (pen)
//0x02	0x00	Mouse controller
//0x80	0x00	Other input controller
//Class Code A: Docking Stations
//Sub-Class	Prog. I/F	Description
//0x00	0x00	Generic docking station
//0x80	0x00	Other type of docking station
//Class Code B: Processors
//Sub-Class	Prog. I/F	Description
//0x00	0x00	386
//0x01	0x00	486
//0x02	0x00	Pentium
//0x10	0x00	Alpha
//0x20	0x00	PowerPC
//0x40	0x00	Co-Processor
//Class Code C: Serial Bus Controllers
//Sub-Class	Prog. I/F	Description
//0x00	0x00	Firewire (IEEE 1394)
//0x01	0x00	ACCESS bus
//0x02	0x00	SSA (Serial Storage Architecture)
//0x03	0x00	USB (Universal Serial Bus)
