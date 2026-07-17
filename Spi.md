# SPI
Tool to read/write SPI flash memory.

Currently Intel only.

It's more like a research project which might have some errors.

The [chipsec](https://github.com/chipsec/chipsec.git) tool should offer more reliable results and offer a broader target range.
Sometimes the latter fails to recognize a "read protected" pci config, whereas this tool still works in this case.

## Version
1.1.2  
Last changed: 15.07.2026


## Requirements
- msbuild
- [WDK]


## Build
```bash
$devcmd> build.bat /mio /spi [/d] [/r] [/dp <value>] [/pdb] [/h]
=>
build[\debug]\MemIo.sys
build[\debug]\SpiUi.exe
```

## Usage
```bash
Spi.exe
    [/dspi]
    [/bdfb <bus> <device> <function> <bar>]
    [/rcbo <offset>]
    [/cspi <command> <address> [<size|bytes>]]
    [/mmio]
    [/reg <mask>] | [/ft <from> <to>]
    [/obs <size>]
    [/lp <path>]
    [/wtf]
    [/ptc]
    [/v]
    [/h]
```

Modes:
- /dspi : Try to dump spi flash memory via SPIBar reading cycle. 
          Automatically finds the interface if no `/bdfb` or `/rcba` option is provided. 
          Dumps all regions into separate blobs by default, if /ft is not used.
- /cspi : Issue an SPI cycle `<command>` at `<address>` with `<size>` or `<bytes>`.
-   FCYCLE_READ (0x0) : Read `<size>` bytes.
-   FCYCLE_WRITE (0x2) : Write `<bytes>` from a given byte string (AABBCCDD) or a file (f:filename).
-   FCYCLE_BLOCK_ERASE (0x3) : 4k Block Erase at `<address>`.
-   FCYCLE_SEC_ERASE (0x4) : 64k Sector erase at `<address>`.
-   FCYCLE_READ_SFDP (0x5) : Read SFDP.
-   FCYCLE_JEDEC_ID (0x6) : Read JEDEC ID.
-   FCYCLE_WRITE_STATUS (0x7) : write status.
-   FCYCLE_READ_STATUS (0x8) : read status.
-   FCYCLE_RPMC_OP1 (0x9) : RPMC Op1.
-   FCYCLE_RPMC_OP2 (0xA) : RPMC Op2.

Mode Options
- /bdfb : Manually set the `<bus>` `<device>` `<function>` `<barId>` where the address of the spi interface ought to be found.
- /rcba : Manually set the `<offset>` to RCBA to get the spi interface. 
    Commonly it's `0x3800`. 
    RCBA itself is retrieved automatically from device `00:1f:00` at offset `0xf0`.
- /mmio: Dump bios region with mmio mapped physical read, instead of spi reading.
- /reg: Region mask for desired dumps: 
        `1`=fd, `2`=bios, `4`=me, `8`=gb, `0x10`=pd, `0x20`=ec, `0x40`=reg\[6\], ..., `0x200`=reg\[9\]. 
        Default=`0x37`.
- /ft: Dump spi memory `<from>` address `<to>` address. 
       Takes precedence over the `/reg` parameter.
       Use this to dump the whole bios into one blob.
       Resulting size should be multiple of 0x40 bytes.

Other Options:
- /obs : Output buffer size.
- /lp : Path to the parent directory of the created log files. Default: C:\bin\logs

Flags:
- /ptc : Print to console.
- /wtf : Write to file.
- /pp : Print plain bytes output buffer.
- /v : Verbose output.
- /h : Print this.



## Remarks

All values given for bdfs, ports, values and stuff are interpreted as hex values, except if noted otherwise.

Region `[0 : 4]` are known regions, but there are possibly 10 (or even 12/16) regions.
Sometimes region[8] (mask: 0x100) is filled.
It ought to be the EC region.
Currently seen on some Dell and Lenovo notebooks.
Usually it's not readable.

Dumping SPI (`/dspi` ) can be done automatically or by giving a device (bdf+bar) or rcba offset.
On Intel machines the device so far has always been `00:31:05` with bar\[0\] giving the address.
Sometimes (on newer machines?) it's done via RCBA offset `0x3800`.

The automatic dump (`/dspi` without modifiers) uses some known PCH/LPC device ids to identify where the SPI register is located.
If this fails, because the device id is not known yet,
  it checks the (supposed) RCBA field in the `00:1f:00` config.
If `RCAB.enable` is set, 
  it uses the RCBA with a hardcoded offset of `0x3800` to locate the SPI register.
Otherwise it uses the first BAR (bar\[0\]) of device `00:1f:05` to locate the SPI register.

If the automatic dump does not succeed, 
  try to dump with `/dspi /bdf <b> <d> <f> <b>` or `/dspi /rcba <offset>` with some Intel spec info provided values of your own choice.
With `/dspi /bdf <bus> <device> <function> <barId>` the tool tries to read the location of the SPI register via the provided `<bus> <device> <function> <barId>` values.
With `/dspi /rcba <offset>` the tool tries to read the location of the SPI register via the provided `<offset>` to the `RCBA`. 
The `RCBA` value itself is dynamically read from device `00:1f:00`, like explained above.

So far the BIOS has always been found in RAM too and the hashes of the dumps have been equal.
That's why there is the option to dump the bios via physical read.
This option is only available for dumps of the bios region since the others are not (readable) located in ram. 

It's possible to only dump selected regions.
The `/reg` parameter is a (zero based) bit mask.
Bit `0` maps to reg\[0\] (Flash Descriptor) and so on.
So by setting bit 1 and bit 4, i.e. `2 + 8 = 0xA`, the Bios and Gigabit Ethernet regions would be dumped.

The "Gigabit Ethernet" region is found on most systems and mostly is readable and writable.
"Platform Data" has just been seen once so far.
"Embedded Controller" is seen sometimes on reg[8].

EC on intel gen7 i.e. is linked via PECI SMBUS.

Vendor and Device Ids are interpreted based on [the PCI ID Repository](https://pci-ids.ucw.cz/).
[ParsePciIds.py](scripts/parsePciIds.py) is used to parse it into an usable header file.
The location of the used generated header file is [pciids.h](pci/pciids.h)

Writing to the SPI chip can be dangerous and brick the system!
Most places are write protected todays, though.

The app just needs an RW primitive driver like the [MemIo](src\sys\memio.sys) driver provided here or some other of your choice.
There should be at least physical reading and at best also port IO provided.
Even topping it, the ability to map a physical address to user space is nice for performance reasons.
If port IO is not provided it may not work on some systems and the references to port IO in code have to be changed.
See [RW Primitive](RwPrimitive.md) for some more info how to use your own RW primitive.

## Examples
```bash
$ [drivstaller /i MemIo.sys]

$ SpiUi.exe /dspi // dump alle regions with spi reading method into separate files
$ SpiUi.exe /dspi /mmio /reg 0x2 // dump only the bios region with physical memory reading 
$ SpiUi.exe /dspi /reg 0x9 // dump the flash descriptor and the GBe region into separate files
$ SpiUi.exe /dspi /ft 0 3000 // dump the spi memory from address 0 to 3000 (e.g. the flash descriptor and GBe) into one file.

$ SpiUi.exe /cspi 0 0x1000 0x40 // read 0x40 bytes (of the supposedly gbe region) at offset 0x1000
$ SpiUi.exe /bdfb 0 1f 5 0 /cspi 0 0x1000 0x40 // read 0x40 bytes at offset 0x1000 using the specified /bdfb location to find the spi interface

$ SpiUi.exe /cspi 2 0x1000 00000000 // Write 4 bytes of 0 (to the supposedly gbe region) at offset 0x1000
$ SpiUi.exe /cspi 3 0x1000 // 4k Block Erase (of the supposedly gbe region) at offset 0x1000
$ SpiUi.exe /cspi 2 0x1000 f:gbe.bin // write bytes from the gbe.bin file to offset 0x1000
```

## COPYRIGHT, CREDITS & CONTACT
Published under [GNU GENERAL PUBLIC LICENSE](LICENSE).
