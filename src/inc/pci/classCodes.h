#pragma once

// defined in wdm.h
//#define PCI_CLASS_PRE_20                    0x00
//#define PCI_CLASS_MASS_STORAGE_CTLR         0x01
//#define PCI_CLASS_NETWORK_CTLR              0x02
//#define PCI_CLASS_DISPLAY_CTLR              0x03
//#define PCI_CLASS_MULTIMEDIA_DEV            0x04
//#define PCI_CLASS_MEMORY_CTLR               0x05
//#define PCI_CLASS_BRIDGE_DEV                0x06
//#define PCI_CLASS_SIMPLE_COMMS_CTLR         0x07
//#define PCI_CLASS_BASE_SYSTEM_DEV           0x08
//#define PCI_CLASS_INPUT_DEV                 0x09
//#define PCI_CLASS_DOCKING_STATION           0x0a
//#define PCI_CLASS_PROCESSOR                 0x0b
//#define PCI_CLASS_SERIAL_BUS_CTLR           0x0c
//#define PCI_CLASS_WIRELESS_CTLR             0x0d
//#define PCI_CLASS_INTELLIGENT_IO_CTLR       0x0e
//#define PCI_CLASS_SATELLITE_COMMS_CTLR      0x0f
//#define PCI_CLASS_ENCRYPTION_DECRYPTION     0x10
//#define PCI_CLASS_DATA_ACQ_SIGNAL_PROC      0x11
#define PCI_CLASS_PROCESSING_ACCELERATOR (0x12)
#define PCI_CLASS_NON_ESSENTIAL_INSTRUMENTATION (0x13)
#define PCI_CLASS_CO_PROCESSOR (0x40)
// 0d thru fe reserved

#define PCI_CLASS_NOT_DEFINED               0xff

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
