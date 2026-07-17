#pragma once

#ifdef RING3
#define ExAllocatePoolWithTag(_pt_, _n_, _t_) malloc(_n_)
#ifdef ExFreePool
#undef ExFreePool
#endif
#define ExFreePool(_p_) free(_p_)
#endif



#define REG_KEY_PART_INFO_DWORD_SIZE (sizeof(KEY_VALUE_PARTIAL_INFORMATION)+3)
#define REG_KEY_PART_INFO_QWORD_SIZE (sizeof(KEY_VALUE_PARTIAL_INFORMATION)+7)

typedef NTSTATUS (*REG_KEY_CALLBACK)(_In_ HANDLE KeyHandle, _In_ PWCHAR KeyName);
typedef NTSTATUS (*REG_VALUE_CALLBACK)(_In_ HANDLE KeyHandle, _In_ PWCHAR KeyName, _In_ PKEY_VALUE_FULL_INFORMATION Info);



NTSTATUS CreateRegistryKey(
    _In_ CONST PWCHAR Path, 
    _Out_ HANDLE* Key,
    _In_ ULONG DesiredAccess, 
    _In_ ULONG CreateOptions
);

NTSTATUS OpenRegistryKey(
    _In_ CONST PWCHAR Path, 
    _Out_ HANDLE* Key, 
    _In_ ULONG DesiredAccess
);

NTSTATUS WriteRegDWORD(
    _In_ HANDLE Key, 
    _In_ PWCHAR valueName, 
    _In_ ULONG data
);

NTSTATUS ReadRegDWORD(
    _In_ HANDLE Key, 
    _In_ PWCHAR valueName, 
    _Out_ ULONG * Data
);

NTSTATUS WriteRegQWORD(
    _In_ HANDLE Key, 
    _In_ PWCHAR valueName, 
    _In_ UINT64 data
);

NTSTATUS ReadRegQWORD(
    _In_ HANDLE Key, 
    _In_ PWCHAR valueName, 
    _Out_ UINT64 * Data
);

NTSTATUS WriteRegSZ(
    _In_ HANDLE Key, 
    _In_ PWCHAR valueName, 
    _In_ PWCHAR Sz, 
    _In_ ULONG SzCb
);

///**
// * Read REG_SZ value.
// * Allocates *readData, if NULL, be sure to free it in this case.
// * If *readData is already allocated, pass a valid readDataSize
// * 
// * @param hKey HKEY the opened key
// * @param valueName PCHAR the name of the SZ value
// * @param readData PCHAR* pointer to an allocated or not allocated buffer.
// * @param readDataSize PULONG if *readData is allocated it should be its size. Will be filled with the actual data size.
// */
//NTSTATUS ReadRegSZ(
//    _In_ HANDLE Key, 
//    _In_ PWCHAR valueName, 
//    _Out_ PCHAR *data,
//    _Inout_ PULONG size
//);

NTSTATUS WriteRegBinary(
    _In_ HANDLE Key, 
    _In_ PWCHAR valueName, 
    _In_ PVOID Data, 
    _In_ ULONG Size
);

///**
// * Read REG_Binary value.
// * Allocates *readData, if NULL, be sure to free it in this case.
// * If *readData is already allocated, pass a valid readDataSize
// * 
// * @param hKey HKEY the opened key
// * @param valueName PCHAR the name of the SZ value
// * @param readData PCHAR* pointer to an allocated or not allocated buffer.
// * @param readDataSize PULONG if *readData is allocated it should be its size. Will be filled with the actual data size.
// */
//NTSTATUS ReadRegBinary(
//    _In_ HANDLE Key, 
//    _In_ PWCHAR valueName, 
//    _Out_ PVOID *data,
//    _Inout_ PULONG size
//);

/**
 * Read REG_XXX value.
 * Allocates *readData, if NULL, be sure to free it in this case.
 * If *readData is already allocated, pass a valid readDataSize
 * 
 * @param hKey HKEY the opened key
 * @param valueName PCHAR the name of the SZ value
 * @param readData PKEY_VALUE_PARTIAL_INFORMATION * to an allocated or not allocated buffer.
 * @param readDataSize PULONG if *readData is allocated it should be its size. Will be filled with the actual data size.
 */
NTSTATUS ReadRegData(
    _In_ HANDLE Key, 
    _In_ PWCHAR ValueName, 
    _Inout_ PKEY_VALUE_PARTIAL_INFORMATION * Data, 
    _Inout_ PULONG Size
);

NTSTATUS ReadRegDataAlign64(
    _In_ HANDLE Key, 
    _In_ PWCHAR ValueName, 
    _Inout_ PKEY_VALUE_PARTIAL_INFORMATION_ALIGN64 * Data, 
    _Inout_ PULONG Size
);


//
// query functions
//

// some arbitrary size to prevent infinite loops
#define MAX_KEY_ITERATION_COUNT (0x100)
#define MAX_VALUE_ITERATION_COUNT (0x100)

#define KEY_NAME_MAX (0x200)
#pragma pack(1)
#pragma warning( disable : 4201 )

typedef struct _KEY_OBJ
 { 
    LIST_ENTRY Link;
    union {
        ULONG Visited:1;
        ULONG Reserved:31;
    } Flags;
    ULONG NameSize; // in bytes
    WCHAR Name[1];
 } KEY_OBJ, *PKEY_OBJ; 

typedef union _QUERY_KEY_FLAGS {
    UINT32 Raw;
    struct {
        UINT32 PrintQueryValuesParent:1;
        UINT32 Reserved:31;
    };
} QUERY_KEY_FLAGS, *PQUERY_KEY_FLAGS;

#pragma warning( default : 4201 )
#pragma pack()

/**
 * Query all values of a key and call a callback on it
 *
 */
NTSTATUS
QueryKeys(
    _In_ PWCHAR KeyPath,
    _In_ ULONG DesiredAccess,
    _In_opt_ REG_KEY_CALLBACK RegKeyCallback,
    _In_opt_ REG_VALUE_CALLBACK RegValueCallback,
    _In_ QUERY_KEY_FLAGS Flags
);

NTSTATUS
QuerySubKeys(
    _In_ PWCHAR KeyPath,
    _In_ HANDLE Key,
    _Inout_ PUINT8 *Buffer,
    _Inout_ PULONG BufferSize,
    _In_ PLIST_ENTRY ListHead,
    _In_ QUERY_KEY_FLAGS Flags
);

NTSTATUS
QueryValues(
    _In_ HANDLE Key,
    _In_ PWCHAR KeyPath,
    _Inout_ PVOID *Buffer,
    _Inout_ PULONG BufferSize,
    _In_ REG_VALUE_CALLBACK ValueCallback,
    _In_ QUERY_KEY_FLAGS Flags
);

NTSTATUS deleteKeyCb(
    _In_ HANDLE Key,
    _In_ PWCHAR Name
);

NTSTATUS deleteValueCb(
    _In_ HANDLE Key,
    _In_ PWCHAR Name, 
    _In_ PKEY_VALUE_FULL_INFORMATION Info
);
