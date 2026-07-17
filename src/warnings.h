#pragma once

// __pragam is MSC specific
// _Pragma is similar to the Microsoft-specific __pragma keyword. 
// It was introduced into the C standard in C99, and the C++ standard in C++11. 
// It's available in C only when you specify the /std:c11 or /std:c17 option.
// Unlike #pragma, _Pragma allows you to put pragma directives into a macro definition. 
#define DISABLE_WARNING(_id_) \
    __pragma( warning( disable : _id_) )

#define DISABLE_WARNINGS(...) \
    __pragma( warning( disable : __VA_ARGS__ ) )

#define DEFAULT_WARNING(_id_) \
    __pragma( warning( default : _id_) )

#define DEFAULT_WARNINGS(...) \
    __pragma( warning( default : __VA_ARGS__ ) )

#define SUPPRESS_WARNING(_id_) \
    __pragma( warning( suppress : _id_) )

#define SUPPRESS_WARNINGS(...) \
    __pragma( warning( suppress : __VA_ARGS__ ) )

// disable warnings in release and debug build
// 
//  4201: nonstandard extension used: nameless struct/union
//  4996: 'ExAllocatePoolWithTag': ExAllocatePoolWithTag is deprecated, use ExAllocatePool2
//  6320: Exception-filter expression is the constant EXCEPTION_EXECUTE_HANDLER
// 28175: member of _DRIVER_OBJECT should not be accessed
// 30029: A call was made to MmMapIoSpace(). This allocates executable memory.
// 30030: Warning: Allocating executable memory via specifying a MM_PAGE_PRIORITY type without a bitwise OR with MdlMappingNoExecute
// 28118: The current function is permitted to run at an IRQ level above the maximum permitted for '__PREfastPagedCode' (1).
// 28278: Function openLogFile appears with no prototype in scope. Only limited analysis can be performed.
//#pragma warning( disable : 4201 4996 6320 28175 30029 30030 28118 28278 )
DISABLE_WARNINGS(4201 4996 6320 28175 30029 30030 28118 28278)

// disable warnings just in debug build
// 
// 4100: unreferenced formal parameter
// 4101: 
// 4102: unreferenced label
// 4189: local variable is initialized but not referenced
// 4702: unreachable code
#ifdef DBG
DISABLE_WARNINGS(4100 4101 4102 4189 4702)
#endif
