# RW Primitive

The rw primitive driver provided in this project is not signed nor compiled at all.
If it is compiled and self signed, test signing has to be enabled on the target machine to successfully load it.

It is possible to use another rw primitive driver of your choice.
In this case, the functions in [memoryIo.h](/src/exe/memoryIo.h) have to be modified to match the driver's needs.
If not all functions can be covered by the used rw primitive, 
    they may be exchanged or wrapped by other functions.
Depending on how this is done, 
    some places in the code calling those functions may have to be changed too.

Pci configs and bars should be read and written in 4 byte chunks, 4 byte aligned.
Otherwise there may be invalid unexpected data.
Bars can provide some more or other restrictions as well.


## memoryIo.h

Provides basic functions to read and write kernel memory and ports.
The currently used rw driver is memio.sys.
If another rw driver is used this is the main file that has to be adjusted to the new driver needs.
There may be some other parts located in other files, but actually should not and if found or reported are to be removed.

Mandatory functions to implement are
- readPA
- readPAWtf
- writePA
- in(B)(8|16|32)
- out(8|16|32)

Mandatory but not necessarily subject to change
- outIn(8|16|32)
- outOut32

In here `readPa`, `readPAWtf` and `writePa` are wrapper for more specific functions.
It's not necessary to do so and the whole physical reading/writing logic can be placed directly in those three functions.
The modes have to stay defined but just would be meaningless in that case.

Especially for pci config (and also bar) reading, 
it makes sense to forcefully implement it in a 4 byte chunked way and not let it being optimized by memcpy or however the driver may read.

The `outInXX` and `outOutXX` are just convenience wrapper for a command-port status-port flow like done in port io based pci config readings.
Since they call the `inXX` and `outXX` functions, they are not really subject to change if another RW primitive would be used.

There is a `RW_PRIM_MEM_MAPPING_SUPPORTED` switch.
Turn this on (uncomment it), if the RW primitive driver supports pa mapping into user space.
If so, the `mapMemory` and `unmapMemory` functions have to adjusted as well.
If PA user space mapping is not supported, turn it of (comment it out).
Currently it is turned on, because MemIo actually supports it.

There are also some internal memio specific functions (starting with `__XXX`) called by readPA, readPAWtf. 
They have to be deleted (or at least not called) if another driver is used.
They may be changed to the other driver's needs as well.
