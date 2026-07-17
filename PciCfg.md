# PCI CFG

Tool to convert BDFo values to port io values or memory mapped addresses and decode port io values or memory mapped addresses into bdf offset values.


## Version
1.0.3  
Last changed: 15.07.2026


## Requirements
- msbuild


## Build
```bash
$devcmd> build.bat /pcicfg [/d] [/r] [/dp <value>] [/pdb] [/h]
=>
build[\debug]\PciCfg.exe
```

## Usage
```bash
PciCfg.exe
    [/io <bus> <device> <function> <offset>]
    [/mm <pciexbar> <bus> <device> <function> <offset>]
    [/io <portIoAddr>]
    [/mm <mmAddr>]
    [/h]
``` 
Modes:
- /io : Encode `<bus>` `<device>` `<function>` `<offset>` into portio data value.
- /mm : Encode `<pciexbar>` `<bus>` `<device>` `<function>` `<offset>` into memory mapped io config address.
- /io : Decode `<portIoAddr>` into bus, device, function, offset.
- /mm : Decode `<mmAddr>` into pciexbarbus, bus, device, function, offset.

Flags:
- /v : Verbose output.
- /h : Print this.


## Remarks
All values given are interpreted as hex values.

### Examples
```
$ PciCfg.exe /io 0 1f 5 0 // encode bdf 00:1f:05 with offset 0 into portio value
=> 0x8000f800

$ PciCfg.exe /io 0x8000f800 // decode 0x8000f800 into bdf and offset 
=> bus: 0x0 (0)
   device: 0x1f (31)
   function: 0x0 (0)
   offset: 0x0 (0)

$ PciCfg.exe /mm f0000000 0 1f 0 0 // decode bdf 00:1f:05 with offset 0, pciexbar of fed19000 into memory mapped address
=> mmioAddr: 0xf00fd000

$ PciCfg.exe /mm 0xf00fd000 // decode memory mapped address into bdf, offset and pciexbar
=>  PciExBar: 0xf0000000
    Bus: 0x0
    Device: 0x1f
    Function: 0x5
    Offset: 0x0
```
