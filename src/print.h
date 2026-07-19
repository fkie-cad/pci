#pragma once

#include <stdint.h>

//
// DbgPrint and DbgPrintEx can be called at IRQL<=DIRQL (Dispatch level). 
// However, Unicode format codes (%C, %S, %lc, %ls, %wc, %ws, and %wZ) can be used only at IRQL=PASSIVE_LEVEL. 
// Also, because the debugger uses interprocess interrupts (IPIs) to communicate with other processors, 
// calling DbgPrint at IRQL>DIRQL can cause deadlocks.
//

//#define COMPONTENT_ID DPFLTR_DEFAULT_ID
#define COMPONTENT_ID DPFLTR_IHVDRIVER_ID

#ifdef __GNUC__
#define FORCEINLINE __attribute__((always_inline)) inline
#endif

#ifdef RING3
#include <stdio.h>
#include <stdint.h>

#define DbgPrint printf
#define DbgPrintEx(_ci_, _l_, _va_) printf(_va_)
#define DPRINT_PREFIX ""
#define EPRINT_PREFIX ""
#else
#define DbgPrint(...) DbgPrintEx ( COMPONTENT_ID, DPFLTR_INFO_LEVEL, __VA_ARGS__ )

#define DPRINT_PREFIX DRIVER_NAME ": "
#define EPRINT_PREFIX DRIVER_NAME

#ifdef _WIN32
#define uint8_t UINT8
#define uint16_t UINT16
#define uint32_t UINT32
#define uint64_t UINT64
#define size_t SIZE_T
#endif
#endif

#ifndef DRIVER_NAME
#define DRIVER_NAME "driver"
#endif


#define HEX_CHAR_WIDTH(__hcw_v__, __hcw_w__) \
{ \
    uint8_t _hcw_w_ = 0x10; \
    for ( uint8_t _i_ = 0x38; _i_ > 0; _i_-=8 ) \
    { \
        if ( ! ((uint8_t)(__hcw_v__ >> _i_)) ) \
            _hcw_w_ -= 2; \
        else \
            break; \
    } \
    __hcw_w__ = _hcw_w_; \
}


#ifdef DEBUG_PRINT
#define DPrint(...) \
                {DbgPrint(DPRINT_PREFIX);\
                 DbgPrint(__VA_ARGS__);}
#define FEnter() \
                {DbgPrint(DPRINT_PREFIX);\
                 DbgPrint("[>] %s()\n", __FUNCTION__);}
#define FLeave() \
                {DbgPrint(DPRINT_PREFIX);\
                 DbgPrint("[<] %s()\n", __FUNCTION__);}
#define SPrint(_c_) \
                DbgPrint(DRIVER_NAME "[s] "); \
                switch ( _c_ ) { \
                    case STATUS_SUCCESS: DbgPrint("STATUS_SUCCESS"); break; \
                    case STATUS_UNSUCCESSFUL: DbgPrint("STATUS_UNSUCCESSFUL"); break; \
                    case STATUS_NOT_IMPLEMENTED: DbgPrint("STATUS_NOT_IMPLEMENTED"); break; \
                    case STATUS_ACCESS_DENIED: DbgPrint("STATUS_ACCESS_DENIED"); break; \
                    case STATUS_OBJECT_TYPE_MISMATCH: DbgPrint("STATUS_OBJECT_TYPE_MISMATCH"); break; \
                    case STATUS_NO_SUCH_DEVICE: DbgPrint("STATUS_NO_SUCH_DEVICE"); break; \
                    case STATUS_NOT_SUPPORTED: DbgPrint("STATUS_NOT_SUPPORTED"); break; \
                    case STATUS_ACCESS_VIOLATION: DbgPrint("STATUS_ACCESS_VIOLATION"); break; \
                    case STATUS_CONFLICTING_ADDRESSES: DbgPrint("STATUS_CONFLICTING_ADDRESSES"); break; \
                    case STATUS_OBJECT_NAME_INVALID: DbgPrint("STATUS_OBJECT_NAME_INVALID"); break; \
                    case STATUS_OBJECT_NAME_NOT_FOUND: DbgPrint("STATUS_OBJECT_NAME_NOT_FOUND"); break; \
                    case STATUS_OBJECT_PATH_INVALID: DbgPrint("STATUS_OBJECT_PATH_INVALID"); break; \
                    case STATUS_OBJECT_PATH_NOT_FOUND: DbgPrint("STATUS_OBJECT_PATH_NOT_FOUND"); break; \
                    case STATUS_OBJECT_PATH_SYNTAX_BAD: DbgPrint("STATUS_OBJECT_PATH_SYNTAX_BAD"); break; \
                    case STATUS_SECTION_TOO_BIG: DbgPrint("STATUS_SECTION_TOO_BIG"); break; \
                    case STATUS_SECTION_PROTECTION: DbgPrint("STATUS_SECTION_PROTECTION"); break; \
                    case STATUS_INVALID_PARAMETER: DbgPrint("STATUS_INVALID_PARAMETER"); break; \
                    case STATUS_INVALID_PARAMETER_1: DbgPrint("STATUS_INVALID_PARAMETER_1"); break; \
                    case STATUS_INVALID_PARAMETER_2: DbgPrint("STATUS_INVALID_PARAMETER_2"); break; \
                    case STATUS_INVALID_PARAMETER_3: DbgPrint("STATUS_INVALID_PARAMETER_3"); break; \
                    case STATUS_INVALID_PARAMETER_4: DbgPrint("STATUS_INVALID_PARAMETER_4"); break; \
                    case STATUS_INVALID_PARAMETER_5: DbgPrint("STATUS_INVALID_PARAMETER_5"); break; \
                    case STATUS_INVALID_PARAMETER_6: DbgPrint("STATUS_INVALID_PARAMETER_6"); break; \
                    case STATUS_NO_SUCH_FILE: DbgPrint("STATUS_NO_SUCH_FILE"); break; \
                    case STATUS_INVALID_DEVICE_REQUEST: DbgPrint("STATUS_INVALID_DEVICE_REQUEST"); break; \
                    case STATUS_ILLEGAL_FUNCTION: DbgPrint("STATUS_ILLEGAL_FUNCTION"); break; \
                    case STATUS_INVALID_HANDLE: DbgPrint("STATUS_INVALID_HANDLE"); break; \
                    case STATUS_DATATYPE_MISALIGNMENT_ERROR: DbgPrint("STATUS_DATATYPE_MISALIGNMENT_ERROR"); break; \
                    case STATUS_OBJECT_NAME_COLLISION: DbgPrint("STATUS_OBJECT_NAME_COLLISION"); break; \
                    \
                    case STATUS_ACPI_INVALID_INDEX: DbgPrint("STATUS_ACPI_INVALID_INDEX"); break; \
                    case STATUS_ACPI_INVALID_DATA: DbgPrint("STATUS_ACPI_INVALID_DATA"); break; \
                    default: DbgPrint("unknown"); break; \
                }; \
                DbgPrint(" (0x%x)\n", _c_)
FORCEINLINE 
void DPrintMemCol8(void* _b_, size_t _s_, size_t _a_)
{
    uint64_t _hw_v_ = _a_ + (_s_);
    uint8_t _hw_w_ = 0x10;
    HEX_CHAR_WIDTH(_hw_v_, _hw_w_);
   
    for ( size_t _i_ = 0; _i_ < (size_t)(_s_); _i_+=0x10 )
    {
        size_t _end_ = (_i_+0x10<(_s_))?(_i_+0x10):((size_t)(_s_));
        uint32_t _gap_ = (_i_+0x10<=(_s_)) ? 0 : (uint32_t)((0x10+_i_-(size_t)(_s_))*3);
        DbgPrint("%.*zx  ", _hw_w_, (((size_t)_a_)+_i_));
        
        for ( size_t _j_ = _i_, _k_=0; _j_ < _end_; _j_++, _k_++ )
        {
            DbgPrint("%02x", ((uint8_t*)_b_)[_j_]);
            DbgPrint("%c", (_k_==7?'-':' '));
        }
        for ( uint32_t _j_ = 0; _j_ < _gap_; _j_++ )
        {
            DbgPrint(" ");
        }
        DbgPrint("  ");
        for ( size_t _j_ = _i_; _j_ < _end_; _j_++ )
        {
            if ( ((uint8_t*)_b_)[_j_] < 0x20 || ((uint8_t*)_b_)[_j_] > 0x7E || ((uint8_t*)_b_)[_j_] == 0x25 )
            {
                DbgPrint(".");
            } 
            else
            {
                DbgPrint("%c", ((uint8_t*)_b_)[_j_]);
            }
        }
        DbgPrint("\n");
    }
}
//#define DPrintMemCol8(_b_, _s_, _a_) \
//{ \
//    uint64_t _hw_v_ = _a_ + (_s_); \
//    uint8_t _hw_w_ = 0x10; \
//    HEX_CHAR_WIDTH(_hw_v_, _hw_w_); \
//    \
//    for ( size_t _i_ = 0; _i_ < (size_t)(_s_); _i_+=0x10 ) \
//    { \
//        size_t _end_ = (_i_+0x10<(_s_))?(_i_+0x10):((size_t)(_s_)); \
//        uint32_t _gap_ = (_i_+0x10<=(_s_)) ? 0 : (uint32_t)((0x10+_i_-(size_t)(_s_))*3); \
//        DbgPrint("%.*zx  ", _hw_w_, (((size_t)_a_)+_i_)); \
//         \
//        for ( size_t _j_ = _i_, _k_=0; _j_ < _end_; _j_++, _k_++ ) \
//        { \
//            DbgPrint("%02x", ((uint8_t*)_b_)[_j_]); \
//            DbgPrint("%c", (_k_==7?'-':' ')); \
//        } \
//        for ( uint32_t _j_ = 0; _j_ < _gap_; _j_++ ) \
//        { \
//            DbgPrint(" "); \
//        } \
//        DbgPrint("  "); \
//        for ( size_t _j_ = _i_; _j_ < _end_; _j_++ ) \
//        { \
//            if ( ((uint8_t*)_b_)[_j_] < 0x20 || ((uint8_t*)_b_)[_j_] > 0x7E || ((uint8_t*)_b_)[_j_] == 0x25 ) \
//            { \
//                DbgPrint("."); \
//            }  \
//            else \
//            { \
//                DbgPrint("%c", ((uint8_t*)_b_)[_j_]); \
//            } \
//        } \
//        DbgPrint("\n"); \
//    } \
//}
#define DPrintMemCol16(_b_, __s__) \
    uint64_t _s_ = (__s__); \
    if ( _s_ % 2 != 0 ) _s_ = _s_ - 1; \
    \
    for ( size_t _i_ = 0; _i_ < (size_t)_s_; _i_+=0x10 ) \
    { \
        size_t _end_ = (_i_+0x10<_s_)?(_i_+0x10):((size_t)_s_); \
        uint32_t _gap_ = (_i_+0x10<=_s_) ? 0 : ((0x10+_i_-(size_t)_s_)/2*5); \
        DbgPrint("%p  ", (((uint8_t*)_b_)+_i_)); \
         \
        for ( size_t _j_ = _i_; _j_ < _end_; _j_+=2 ) \
        { \
            DbgPrint("%04x ", *(uint16*)&(((uint8_t*)_b_)[_j_])); \
        } \
        for ( uint32_t _j_ = 0; _j_ < _gap_; _j_++ ) \
        { \
            DbgPrint(" "); \
        } \
        DbgPrint("  "); \
        for ( size_t _j_ = _i_; _j_ < _end_; _j_+=2 ) \
        { \
            DbgPrint("%wc", *(uint16*)&(((uint8_t*)_b_)[_j_])); \
        } \
        DbgPrint("\n"); \
    }
#define DPrintMemCol32(_b_, __s__) \
    uint64_t _s_ = (__s__); \
    if ( _s_ % 4 != 0 ) _s_ = _s_ - (_s_ % 4); \
    \
    for ( size_t _i_ = 0; _i_ < (size_t)_s_; _i_+=0x10 ) \
    { \
        size_t _end_ = (_i_+0x10<_s_)?(_i_+0x10):((size_t)_s_); \
        DbgPrint("%p  ", (((uint8_t*)_b_)+_i_)); \
         \
        for ( size_t _j_ = _i_; _j_ < _end_; _j_+=4 ) \
        { \
            DbgPrint("%08x ", *(uint32*)&(((uint8_t*)_b_)[_j_])); \
        } \
        DbgPrint("\n"); \
    }
FORCEINLINE
void DPrintMemCols64(void* _b_, size_t __s__)
{
    uint64_t _s_ = __s__;
    if ( _s_ % 8 != 0 ) _s_ = _s_ - (_s_ % 8);
   
    for ( size_t _i_ = 0; _i_ < (size_t)_s_; _i_+=0x10 )
    {
        size_t _end_ = (_i_+0x10<_s_)?(_i_+0x10):((size_t)_s_);
        DbgPrint("%p  ", (((uint8_t*)_b_)+_i_));
        
        for ( size_t _j_ = _i_; _j_ < _end_; _j_+=8 )
        {
            DbgPrint("%016llx ", *(uint64_t*)&(((uint8_t*)_b_)[_j_]));
        }
        DbgPrint("\n");
    }
}
//#define DPrintMemCol64(_b_, __s__) \
//    uint64_t _s_ = (__s__); \
//    if ( _s_ % 8 != 0 ) _s_ = _s_ - (_s_ % 8); \
//    \
//    for ( size_t _i_ = 0; _i_ < (size_t)_s_; _i_+=0x10 ) \
//    { \
//        size_t _end_ = (_i_+0x10<_s_)?(_i_+0x10):((size_t)_s_); \
//        DbgPrint("%p  ", (((uint8_t*)_b_)+_i_)); \
//         \
//        for ( size_t _j_ = _i_; _j_ < _end_; _j_+=8 ) \
//        { \
//            DbgPrint("%016llx ", *(uint64_t*)&(((uint8_t*)_b_)[_j_])); \
//        } \
//        DbgPrint("\n"); \
//    }
#define DPrintBytes(_b_, _s_) \
{ \
    for ( size_t _i_ = 0; _i_ < (size_t)(_s_); _i_+=0x10 ) \
    { \
        size_t _end_ = (_i_+0x10<(_s_))?(_i_+0x10):((size_t)(_s_)); \
        DbgPrint("%p  ", (((uint8_t*)_b_)+_i_)); \
         \
        for ( size_t _j_ = _i_; _j_ < _end_; _j_++ ) \
        { \
            DbgPrint("%02x ", ((uint8_t*)_b_)[_j_]); \
        } \
        DbgPrint("\n"); \
    } \
}
#define DPrintUUID(__uuid__) DPrint("%08x-%04x-%04x-%02x%02-x%02x%02x%02x%02x%02x%02x\n", __uuid__.Data1, __uuid__.Data2, __uuid__.Data3, __uuid__.Data4[0], __uuid__.Data4[1], __uuid__.Data4[2], __uuid__.Data4[3], __uuid__.Data4[4], __uuid__.Data4[5], __uuid__.Data4[6], __uuid__.Data4[7]);

#define DPRINT_INT_D(__value__, __prefix__) \
    {DbgPrint(DPRINT_PREFIX);\
     DbgPrint("%s%s: %llu\n", __prefix__, #__value__, (size_t)__value__);}

#define DPRINT_INT_H(__value__, __prefix__) \
    {DbgPrint(DPRINT_PREFIX);\
     DbgPrint("%s%s: 0x%llx\n", __prefix__, #__value__, (size_t)__value__);}

#define DPRINT_INT_HD(__value__, __prefix__) \
    {DbgPrint(DPRINT_PREFIX);\
     DbgPrint("%s%s: 0x%llx (%llu)\n", __prefix__, #__value__, (size_t)__value__, (size_t)__value__);}

#define DPRINT_PTR(__value__, __prefix__) \
    {DbgPrint(DPRINT_PREFIX);\
     DbgPrint("%s%s: %p\n", __prefix__, #__value__, (void*)__value__);}

#define DPRINT_A(__value__, __prefix__) \
    {DbgPrint(DPRINT_PREFIX);\
     DbgPrint("%s%s: %s\n", __prefix__, #__value__, (PCHAR)__value__);}

#define DPRINT_Ax(__value__, __size__, __prefix__) \
    {DbgPrint(DPRINT_PREFIX);\
     DbgPrint("%s%s: %.*s\n", __prefix__, #__value__, __size__, (PCHAR)__value__);}

#define DpVar(__type__, __var__, __value__) \
    __type__ __var__ = __value__
#else
#define DPrint(...)
#define DPRINT_INT_D(...)
#define DPRINT_INT_H(...)
#define DPRINT_INT_HD(...)
#define DPRINT_PTR(...)
#define DPRINT_A(...)
#define DPRINT_Ax(...)
#define FEnter()
#define FLeave()
#define SPrint(_c_)
#define DPrintMemCol8(_b_, _s_, _o_)
#define DPrintMemCol16(_b_, _s_)
#define DPrintMemCol32(_b_, _s_)
#define DPrintMemCol64(_b_, _s_)
#define DPrintBytes(_b_, _s_)
#define DPrintUUID(__uuid__)

#define DpVar(...)
#endif



#ifdef INFO_PRINT
#define IPrint(...) \
                {DbgPrint(DPRINT_PREFIX "[i]");\
                 DbgPrint(__VA_ARGS__);}
#else
#define IPrint(...)
#endif


#if defined(DEBUG_PRINT) && ((DEBUG_PRINT&4)>0)
#define LOG_PRINT
#endif

#ifdef LOG_PRINT
#define LogPrint(...) \
                {DbgPrint(DPRINT_PREFIX);\
                DbgPrint(__VA_ARGS__);}
#else
#define LogPrint(...)
#endif  



#ifdef ERROR_PRINT
#define EPrint(...) \
    {DbgPrintEx ( COMPONTENT_ID, DPFLTR_ERROR_LEVEL, EPRINT_PREFIX "[e] " );\
     DbgPrintEx ( COMPONTENT_ID, DPFLTR_ERROR_LEVEL, __VA_ARGS__ );}
#else
#define EPrint(...)
#endif



// 0xC0000002 STATUS_NOT_IMPLEMENTED
// 0xC000000E STATUS_NO_SUCH_DEVICE
// 0xC0000010 STATUS_INVALID_DEVICE_REQUEST
// 0xC0000022 STATUS_ACCESS_DENIED
// 0xC0000034 STATUS_OBJECT_NAME_NOT_FOUND
// 0xC0000043 0xC0000043


FORCEINLINE 
void PrintMemCol8(void* _b_, size_t _s_, size_t _a_)
{
    uint64_t _hw_v_ = _a_ + (_s_);
    uint8_t _hw_w_ = 0x10;
    HEX_CHAR_WIDTH(_hw_v_, _hw_w_);
   
    for ( size_t _i_ = 0; _i_ < (size_t)(_s_); _i_+=0x10 )
    {
        size_t _end_ = (_i_+0x10<(_s_))?(_i_+0x10):((size_t)(_s_));
        uint32_t _gap_ = (_i_+0x10<=(_s_)) ? 0 : (uint32_t)((0x10+_i_-(size_t)(_s_))*3);
        DbgPrint("%.*zx  ", _hw_w_, (((size_t)_a_)+_i_));
        
        for ( size_t _j_ = _i_, _k_=0; _j_ < _end_; _j_++, _k_++ )
        {
            DbgPrint("%02x", ((uint8_t*)_b_)[_j_]);
            DbgPrint("%c", (_k_==7?'-':' '));
        }
        for ( uint32_t _j_ = 0; _j_ < _gap_; _j_++ )
        {
            DbgPrint(" ");
        }
        DbgPrint("  ");
        for ( size_t _j_ = _i_; _j_ < _end_; _j_++ )
        {
            if ( ((uint8_t*)_b_)[_j_] < 0x20 || ((uint8_t*)_b_)[_j_] > 0x7E || ((uint8_t*)_b_)[_j_] == 0x25 )
            {
                DbgPrint(".");
            } 
            else
            {
                DbgPrint("%c", ((uint8_t*)_b_)[_j_]);
            }
        }
        DbgPrint("\n");
    }
}
//#define PrintMemCol8(_b_, _s_, _a_) \
//{ \
//    uint64_t _hw_v_ = _a_ + _s_; \
//    uint8_t _hw_w_ = 0x10; \
//    HEX_CHAR_WIDTH(_hw_v_, _hw_w_); \
//    \
//    for ( size_t _i_ = 0; _i_ < (size_t)_s_; _i_+=0x10 ) \
//    { \
//        size_t _end_ = (_i_+0x10<_s_) ? (_i_+0x10) : ((size_t)_s_); \
//        uint32_t _gap_ = (_i_+0x10<=_s_) ? 0 : (uint32_t)((0x10+_i_-(size_t)_s_)*3); \
//        DbgPrint("%.*zx  ", _hw_w_, (((size_t)_a_)+_i_)); \
//         \
//        for ( size_t _j_ = _i_, _k_=0; _j_ < _end_; _j_++, _k_++ ) \
//        { \
//            DbgPrint("%02x", ((uint8_t*)_b_)[_j_]); \
//            DbgPrint("%c", (_k_==7?'-':' ')); \
//        } \
//        for ( uint32_t _j_ = 0; _j_ < _gap_; _j_++ ) \
//        { \
//            DbgPrint(" "); \
//        } \
//        DbgPrint("  "); \
//        for ( size_t _k_ = _i_; _k_ < _end_; _k_++ ) \
//        { \
//            if ( ((uint8_t*)_b_)[_k_] < 0x20 || ((uint8_t*)_b_)[_k_] > 0x7E || ((uint8_t*)_b_)[_k_] == 0x25 ) \
//            { \
//                DbgPrint("."); \
//            }  \
//            else \
//            { \
//                DbgPrint("%c", ((uint8_t*)_b_)[_k_]); \
//            } \
//        } \
//        DbgPrint("\n"); \
//    } \
//}

#define PrintMemCols16(_b_, __s__) \
    uint64_t _s_ = __s__; \
    if ( _s_ % 2 != 0 ) _s_ = _s_ - 1; \
    \
    for ( size_t _i_ = 0; _i_ < (size_t)_s_; _i_+=0x10 ) \
    { \
        size_t _end_ = (_i_+0x10<_s_)?(_i_+0x10):((size_t)_s_); \
        uint32_t _gap_ = (_i_+0x10<=_s_) ? 0 : ((0x10+_i_-(size_t)_s_)/2*5); \
        DbgPrint("%p  ", (((uint8_t*)_b_)+_i_)); \
         \
        for ( size_t _j_ = _i_; _j_ < _end_; _j_+=2 ) \
        { \
            DbgPrint("%04x ", *(uint16*)&(((uint8_t*)_b_)[_j_])); \
        } \
        for ( uint32_t _j_ = 0; _j_ < _gap_; _j_++ ) \
        { \
            DbgPrint(" "); \
        } \
        DbgPrint("  "); \
        for ( size_t _j_ = _i_; _j_ < _end_; _j_+=2 ) \
        { \
            DbgPrint("%wc", *(uint16*)&(((uint8_t*)_b_)[_j_])); \
        } \
        DbgPrint("\n"); \
    }

#define PrintMemCols32(_b_, __s__) \
    uint64_t _s_ = __s__; \
    if ( _s_ % 4 != 0 ) _s_ = _s_ - (_s_ % 4); \
    \
    for ( size_t _i_ = 0; _i_ < (size_t)_s_; _i_+=0x10 ) \
    { \
        size_t _end_ = (_i_+0x10<_s_)?(_i_+0x10):((size_t)_s_); \
        DbgPrint("%p  ", (((uint8_t*)_b_)+_i_)); \
         \
        for ( size_t _j_ = _i_; _j_ < _end_; _j_+=4 ) \
        { \
            DbgPrint("%08x ", *(uint32*)&(((uint8_t*)_b_)[_j_])); \
        } \
        DbgPrint("\n"); \
    }

FORCEINLINE
void PrintMemCols64(void* _b_, size_t __s__)
{
    uint64_t _s_ = __s__;
    if ( _s_ % 8 != 0 ) _s_ = _s_ - (_s_ % 8);
   
    for ( size_t _i_ = 0; _i_ < (size_t)_s_; _i_+=0x10 )
    {
        size_t _end_ = (_i_+0x10<_s_)?(_i_+0x10):((size_t)_s_);
        DbgPrint("%p  ", (((uint8_t*)_b_)+_i_));
        
        for ( size_t _j_ = _i_; _j_ < _end_; _j_+=8 )
        {
            DbgPrint("%016llx ", *(uint64_t*)&(((uint8_t*)_b_)[_j_]));
        }
        DbgPrint("\n");
    }
}


#define PRINT_INT_D(__value__, __prefix__) \
    DbgPrint("%s%s: %u\n", __prefix__, #__value__, __value__);

#define PRINT_INT_H(__value__, __prefix__) \
    DbgPrint("%s%s: 0x%x\n", __prefix__, #__value__, __value__);

#define PRINT_INT_HD(__value__, __prefix__) \
    DbgPrint("%s%s: 0x%x (%u)\n", __prefix__, #__value__, __value__, __value__);
#define PRINT_INT_HD_LL(__value__, __prefix__) \
    DbgPrint("%s%s: 0x%llx (%llu)\n", __prefix__, #__value__, __value__, __value__);
