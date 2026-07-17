# CFG

Tool to read and write PCI configs.

Currently Intel only.

## Version
1.1.2  
Last changed: 15.07.2026


## Requirements
- msbuild
- [WDK]


## Build
```bash
$devcmd> build.bat /mio /cfg [/d] [/r] [/dp <value>] [/pdb] [/h]
=>
build[\debug]\MemIo.sys
build[\debug]\CfgUi.exe
```

## Usage
```bash
CfgIo.exe
    [/dcfg <bus> <device> <function>]
    [/rcfg <bus> <device> <function> <offset>]
    [/wcfg <bus> <device> <function> <offset> <value>]
    [/lpci <type>]
    [/dpci]
    [/bars]
    [/caps]
    [/ex]
    [/mm]
    [/port]
    [/obs <size>]
    [/lp <path>]
    [/wtf]
    [/ptc]
    [/v]
    [/h]
```
Modes:
- /lpci : List all registered pci devices. (Using Registry.
    - Type 1: Using Registry. (No driver needed for this.)
    - Type 2: Using port IO device iteration.
- /dpci : Dump the configs of all registered pci devices (using registry listing).
- /dcfg : Dump a single pci config from `<bus>` `<device>` `<function>` `<offset>`.
- /rcfg : Read a uint32 of a single pci config at `<bus>` `<device>` `<function>` `<offset>`.
- /wcfg: Write a uint32 `<value>` to a single pci config at `<bus>` `<device>` `<function>` `<offset>`.

Modifier Flags:
- /mm: Read memory mapped BDF configs. Faster and PCIe (size of 0x1000) compatible. (Default)
- /port: Read BDF configs via port io. Slow legacy mode and just PCI (size of 0x100) compatible.
- /bars: Read out the bars of a device and write them into files.
- /caps: Read out the capabilities of a device and write them into the log.
- /ex: Set the read size for the configs to 0x1000 bytes (PCIe). Default: 0x100 (PCI).

Options:
- /obs : Output buffer size.
- /lp : Parent path for writing config file, caps and bar dumps. Default: "c:\logs\".

Flags:
- /ptc : Print result to console.
- /wtf : Write result to file.
- /pp : Print plain bytes output buffer.
- /v : Verbose output.
- /h : Print this.


### Remarks
All values given for bdfs, ports, values and stuff are interpreted as hex values, 
  except if noted otherwise.

Read the config space of an PCI device by giving its BDF (Bus, Device, Function) values.
The additional data of some known BDFs will be printed too.
This additional interpretation is not very reliable because there is no version check.
Currently known:
- SPI device (00:31:05).
- ... some other

There is some registry based listing of pci devices (`lpci 1`), 
    which does work completely in user mode.
This may fail on VMs (like HyperV), though.
There also is some pci device iteration enumeration (`lpci 2`), starting form device `00:00:00`.
This one needs a portio supporting driver to function properly.
It is based on the method explained in [Enumerating_PCI_Buses](https://wiki.osdev.org/PCI#Enumerating_PCI_Buses).
A memory mapped reading switch will be added soon.

The configs are printed out interpreted (nicely).
The extend will be further improved by time.

Vendor and Device Ids are interpreted based on [the PCI ID Repository](https://pci-ids.ucw.cz/).
[ParsePciIds.py](scripts/parsePciIds.py) is used to parse it into an usable header file.
The location of the used generated header file is [pciids.h](pci/pciids.h)

Bars and capabilities are only written to file, not printed to the console.
This may change for the capabilities, though.
Does not make much sense to do so for (kilo/mega) bytes of bar values.

Writing to the configs can be dangerous and at least freeze or BSOD the system.
Not all places are writable.
So if a write has no effect, 
    (i.e. nothing has changed after a confirmation read), 
    it may be due to read only config registers.
Sometimes they are also immediately changed (back) by some other actor.
So in this case there won't be a visible change either.

The app just needs an RW primitive driver like the [MemIo](src\sys\memio.sys) driver provided here or some other of your choice.
There should be at least Port IO provided.
On the other hand, a memory mapped (physical address) reading approach is the first choice for the app.
Port IO is just a fallback.
Considering reading memory mapped bars, it can only be done via physical address reading.
See [RW Primitive](RwPrimitive.md) for some more info how to use your own RW primitive.

## Examples
```
Install the rw provider driver
$ [drivstaller /i MemIo.sys]


List pci devices registry based (no driver needed)
$ CfgIo.exe /lpci 1

List pci devices by device iteration
$ CfgIo.exe /lpci 2



Dump all pci configs by registry based list
$ CfgIo.exe /dpci

Dump all pci configs with bars and capabilities by registry based list
save them to files located in c:\logs\
$ CfgIo.exe /dpci /bars /caps /lp c:\logs\



Read 0x100 pci config bytes of bdf 00:1f:00
$ CfgIo.exe /dcfg 0 1f 5

Read 0x100 pci config bytes of bdf 00:1f:00 and read out bars and capabilities as well
Force memory mapped reading
Write them to file
$ CfgIo.exe /rcfg 0 1f 5 /bars /caps /mm /wtf

Read 0x4 pci config bytes of bdf 00:1f:05 at offset 0x10
$ CfgIo.exe /rcfg 0 1f 5 10
```

### Remarks

Some devices *protect* themselves from reading by `FF`-ing their vendor and device id.
They are not listed by the `lpci` run.

If a specific device is expected to be present, it can manually be read by the `dcfg` option.
This does not care about `FF`-ed vendor and device ids.
If it's not all `FF`, but just vendor and device id, it may still provide some useful information.


## COPYRIGHT, CREDITS & CONTACT
Published under [GNU GENERAL PUBLIC LICENSE](LICENSE).
