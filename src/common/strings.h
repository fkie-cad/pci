#pragma once

//#include <stdint.h> // not compatible with some kernel headers ??
#ifndef uint8_t 
#define uint8_t UINT8
#define uint16_t UINT16
#define uint32_t UINT32
#define uint64_t UINT64
#endif

#define BIN_8_STR_BUFFER_SIZE (0xA) // 2 4 char blocks with 1 space and 0 termination 0000 0000\0
#define BIN_16_STR_BUFFER_SIZE (0x14) // 4 4 char blocks with 3 spaces and 0 termination 0000 0000 0000 0000\0
#define BIN_32_STR_BUFFER_SIZE (0x28) // 8 4 char blocks with 7 spaces and 0 termination
#define BIN_64_STR_BUFFER_SIZE (0x50) // 16 4 char blocks with 15 spaces and 0 termination

#define uintXToBin(__i__, __t__) \
void __i__(__t__ n, char* output) \
{ \
    char* o = output; \
    int end = (sizeof(__t__)*8) - 1; \
    for ( int i = end; i >= 0; i-- ) \
    { \
        if ( ( (n >> i) & 1 ) ) \
            *o = '1'; \
        else \
            *o = '0'; \
        o++; \
        if ( i && i % 4 == 0 ) \
        { \
            *o = ' '; \
            o++; \
        } \
    } \
    *o = 0; \
} 

//
// output size >= 10
//
uintXToBin(uint8ToBin, uint8_t);

//
// output size >= 20
//
uintXToBin(uint16ToBin, uint16_t);

//
// output size >= 40
//
uintXToBin(uint32ToBin, uint32_t);

//
// output size >= 80
//
uintXToBin(uint64ToBin, uint64_t);
