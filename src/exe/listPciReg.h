#include "ntstrsafe.h"

#include "../warnings.h"

#include "../common/kAlloc.h"
#include "../common/kRegistry.h"
#include "helper/Converter.h"


#ifndef POOL_TAG
#define POOL_TAG 0x0
#endif

#define BDF_FLAG_SKIP_DOUBLE (0x1)

typedef struct _LPCI_ENTRY {
    BDF Bdf;
    PCHAR Name;
} LPCI_ENTRY, *PLPCI_ENTRY;



NTSTATUS listPciReg(
    _In_ ULONG Flags,
    _In_ PVOID OutputBuffer,
    _In_ ULONG OutputBufferSize,
    _In_ PULONG BytesWritten
);

NTSTATUS enumPci(
    _Out_ PLPCI_ENTRY *BDFList,
    _Out_ PUINT32 BDFListCount,
    _Out_opt_ PCHAR* Names,
    _Out_opt_ PUINT32 NamesSize,
    _In_ UINT32 Flags
);

NTSTATUS locationInfoToBDF(
    _In_ HANDLE Key,
    _Inout_ PKEY_VALUE_PARTIAL_INFORMATION *Value,
    _Inout_ PULONG ValueSize,
    _Out_ PBDF Bdf
);

NTSTATUS deviceDescToName(
    _In_ HANDLE Key,
    _Inout_ PKEY_VALUE_PARTIAL_INFORMATION *Value,
    _Inout_ PULONG ValueSize,
    _Out_ PWCHAR *Name,
    _Out_ PUINT32 NameCch
);



NTSTATUS listPciReg(
    _In_ ULONG Flags,
    _In_ PVOID OutputBuffer,
    _In_ ULONG OutputBufferSize,
    _In_ PULONG BytesWritten
)
{
    FEnter();
    NTSTATUS status = 0;
    
    PLPCI_ENTRY pciEntryList = NULL;
    UINT32 pciEntryListCount = 0;
    PCHAR names = NULL;
    UINT32 namesSize = 0;

    *BytesWritten = 0;
    
    FEnter();

    if ( OutputBufferSize < LIST_PCI_OUT_DEF_SIZE )
    {
        status = STATUS_INVALID_PARAMETER;
        goto clean;
    }

    //
    // get all pci bdf

    status = enumPci(&pciEntryList, &pciEntryListCount, &names, &namesSize, Flags);
    if ( status != 0 || !pciEntryList || !names )
    {
        EPrint("enumPci failed! (0x%x)\n", status);
        goto clean;
    }
    if ( pciEntryListCount == 0 )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("No PCI devices found! (0x%x)\n", status);
        goto clean;
    }

    UINT32 pciEntryListSize = pciEntryListCount * sizeof(LPCI_ENTRY);
    UINT32 reqSize = pciEntryListSize + namesSize + LIST_PCI_OUT_DEF_SIZE;
    if ( reqSize > OutputBufferSize )
    {
        RtlZeroMemory(OutputBuffer, LIST_PCI_OUT_DEF_SIZE);
        PLIST_PCI_OUT out = (PLIST_PCI_OUT)OutputBuffer;
        out->Size = reqSize;
        *BytesWritten = LIST_PCI_OUT_DEF_SIZE;
        // no status set, to write the required size into output
        //EPrint("OutputBuffer too small! (0x%x)\n", status);
        goto clean;
    }
    
    // repoint entry->Name pointer to new location
    //UINT64 oldBase = names;
    //UINT64 newBase = (UINT64)OutputBuffer + offsetof(LIST_PCI_OUT, Buffer) + pciEntryListSize;
    //for ( UINT32 i = 0; i < pciEntryListCount; i++ )
    //{
    //    PLPCI_ENTRY entry = &pciEntryList[i];
    //    entry->Name = (PCHAR)(entry->Name - oldBase + newBase);
    //}

    RtlZeroMemory(OutputBuffer, reqSize);
    PLIST_PCI_OUT out = (PLIST_PCI_OUT)OutputBuffer;
    out->Count = pciEntryListCount;
    out->Size = reqSize;
    out->BDFOffset = 0;
    out->NamesOffset = pciEntryListSize;
    RtlCopyMemory(&out->Buffer[out->BDFOffset], pciEntryList, pciEntryListSize);
    RtlCopyMemory(&out->Buffer[out->NamesOffset], names, namesSize);

    
    // point entry->Name pointer to name buffer location
    PLPCI_ENTRY outEntryList = (PLPCI_ENTRY)&out->Buffer[out->BDFOffset];
    PCHAR name = (PCHAR)&out->Buffer[out->NamesOffset];
    for ( UINT32 i = 0; i < out->Count; i++ )
    {
        PLPCI_ENTRY e = &outEntryList[i];
        e->Name = name;
        name += strlen(name)+1;
    }

    *BytesWritten = reqSize;

clean:
    if ( pciEntryList )
        ExFreePool(pciEntryList);
    if ( names )
        ExFreePool(names);

    FLeave();
    return status;
}

FORCEINLINE
BOOLEAN bdfExists(PLPCI_ENTRY PciEntryList, UINT32 PciEntryListSize, PBDF BDF)
{
    FEnter();
    for ( UINT32 i = 0; i < PciEntryListSize; i++ )
    {
        PBDF bdf = &PciEntryList[i].Bdf;
        if ( bdf->Bus == BDF->Bus 
          && bdf->Device == BDF->Device 
          && bdf->Function == BDF->Function )
        {
            FLeave();
            return TRUE;
        }
    }
    FLeave();
    return FALSE;
}

//
// enum pci devices to bdf
//
//  @param PciEntryList PLPCI_ENTRY* Filled with internally allocated buffer containing the entries. To be freed by caller!
//  @param PciEntryListCount PUINT32 Filled with the number of entries in PciEntryList
//  @param Names PCHAR* Filled with internally allocated buffer containing the names of the entries. To be freed by caller!
//  @param NamesSize PUINT32 Filled with the number of entries in Names
//  @param Flags UINT32 Flags: BDF_FLAG_SKIP_DOUBLE
//
NTSTATUS enumPci(
    _Out_ PLPCI_ENTRY *PciEntryList,
    _Out_ PUINT32 PciEntryListCount,
    _Out_opt_ PCHAR* Names,
    _Out_opt_ PUINT32 NamesSize,
    _In_ UINT32 Flags
)
{
    FEnter();
    NTSTATUS status = STATUS_SUCCESS;

    HANDLE key = NULL;
    PUINT8 szBuffer = NULL;
    ULONG szBufferSize = PAGE_SIZE;

    QUERY_KEY_FLAGS qkFlags = { 0 };
    LIST_ENTRY subKeys0NamesHead = {0};
    LIST_ENTRY subKeys1NamesHead = {0};
    PKEY_OBJ firstKeyObj = NULL;
    SIZE_T firstKeyNameCb = 0;
    PKEY_OBJ actKeyObj = NULL;
    PLIST_ENTRY actKeyLink = NULL;

    PKEY_VALUE_PARTIAL_INFORMATION value = 0;
    ULONG valueSize = 0;

    PWCHAR keyName = L"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\PCI";

    *PciEntryList = NULL;
    *PciEntryListCount = 0;
    if ( Names )
        *Names = NULL;
    if ( NamesSize )
        *NamesSize = 0;

    if ( (Names && !NamesSize) || (!Names && NamesSize) )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Either provide Names and NamesSize or none of them! (0x%x)", status);
        goto clean;
    }
    
    InitializeListHead(&subKeys0NamesHead);
    InitializeListHead(&subKeys1NamesHead);

    PLPCI_ENTRY actEntry = NULL;

    DPrint("  KeyName: %ws\n", keyName);

    //
    // init list
    status = RtlStringCbLengthW(keyName, KEY_NAME_MAX, &firstKeyNameCb);
    if ( !NT_SUCCESS(status) )
        goto clean;
    firstKeyObj = ExAllocatePoolWithTag(PagedPool, firstKeyNameCb+sizeof(KEY_OBJ), POOL_TAG);
    if ( !firstKeyObj )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }
    RtlStringCbPrintfW(firstKeyObj->Name, firstKeyNameCb+2, L"%s", keyName);
    firstKeyObj->NameSize = (ULONG)firstKeyNameCb+2;
    firstKeyObj->Link.Flink = NULL;
    firstKeyObj->Link.Blink = NULL;
    *(PUINT32)&firstKeyObj->Flags = 0;
    

    //
    // preallocate buffer
    SUPPRESS_WARNING( 6014 );
    szBuffer = ExAllocatePoolWithTag(PagedPool, szBufferSize, POOL_TAG);
    if ( !szBuffer )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }
    
    status = OpenRegistryKey(firstKeyObj->Name, &key, KEY_READ);
    if ( !NT_SUCCESS(status) )
    {
        EPrint("OpenRegistryKey \"%ws\" failed! (0x%x)\n", firstKeyObj->Name, status);
        goto clean;
    }

    // get first level sub keys
    //DPrint("get first level sub keys:\n");
    //DPrint("subKeys0NamesHead: %p\n", &subKeys0NamesHead);
    //DPrint("  FLink: %p\n", subKeys0NamesHead.Flink);
    //DPrint("  BLink: %p\n", subKeys0NamesHead.Blink);
    status = QuerySubKeys(firstKeyObj->Name, key, &szBuffer, &szBufferSize, &subKeys0NamesHead, qkFlags);
    if ( !NT_SUCCESS(status) || szBuffer == NULL )
        goto clean;

    //
    // get second level sub keys
    DPrint("get second level sub keys:\n");

    while ( !IsListEmpty(&subKeys0NamesHead) )
    {
        //actKeyLink = PopEntryList(&subKeys0NamesHead);
        //actKeyLink = RemoveTailList(&subKeys0NamesHead);
        actKeyLink = PeekTailList(&subKeys0NamesHead);
        if ( actKeyLink == NULL )
            break;
        actKeyObj = CONTAINING_RECORD(actKeyLink, KEY_OBJ, Link);
        //DPrint("actKeyObj: %p\n", actKeyObj);
        //DPrint("  FLink: %p\n", actKeyObj->Link.Flink);
        //DPrint("  BLink: %p\n", actKeyObj->Link.Blink);
        //DPrint("  NameSize: 0x%x\n", actKeyObj->NameSize);
        //DPrint("  Name: %.*ws\n", actKeyObj->NameSize/2, actKeyObj->Name);
        // open key
        if ( key != NULL )
        {
            ZwClose(key);
            key = NULL;
        }
        status = OpenRegistryKey(actKeyObj->Name, &key, KEY_READ);
        if ( !NT_SUCCESS(status) )
        {
            EPrint("OpenRegistryKey \"%ws\" failed! (0x%x)\n", actKeyObj->Name, status);
            goto skip1;
        }
    
        // QuerySubKeys
        status = QuerySubKeys(actKeyObj->Name, key, &szBuffer, &szBufferSize, &subKeys1NamesHead, qkFlags);
        if ( !NT_SUCCESS(status) || szBuffer == NULL )
            goto clean;
    
    skip1:
        actKeyLink = RemoveTailList(&subKeys0NamesHead);
        ExFreePool(actKeyObj);
        actKeyObj = NULL;
    }


    //
    // init result array

    UINT32 maxCount = 20;
    UINT32 count = 0;
    UINT32 pciEntryListSize = maxCount * sizeof(LPCI_ENTRY);
    *PciEntryList = ExAllocatePoolWithTag(PagedPool, pciEntryListSize, POOL_TAG);
    if ( !(*PciEntryList) )
    {
        status = STATUS_NO_MEMORY;
        goto clean;
    }
    *PciEntryListCount = 0;

    UINT32 namesSize = 0;
    SIZE_T namesRestSize = 0;
    PCHAR namesPtr = NULL;
    if ( Names )
    {
        namesSize = maxCount * 0x20;
        *Names = ExAllocatePoolWithTag(PagedPool, namesSize, POOL_TAG);
        if ( !(*Names) )
        {
            status = STATUS_NO_MEMORY;
            goto clean;
        }
        namesRestSize = namesSize;
        namesPtr = *Names;
    }

    //
    // iterate second level sub keys
    // and read location info
    DPrint("iterate second level sub keys and read location info:\n");
    while ( !IsListEmpty(&subKeys1NamesHead) )
    {
        //actKeyLink = PopEntryList(&subKeys1NamesHead);
        //actKeyLink = RemoveTailList(&subKeys1NamesHead);
        actKeyLink = PeekTailList(&subKeys1NamesHead);
        if ( actKeyLink == NULL )
            break;
        actKeyObj = CONTAINING_RECORD(actKeyLink, KEY_OBJ, Link);
        //DPrint("actKeyObj: %p\n", actKeyObj);
        //DPrint("  FLink: %p\n", actKeyObj->Link.Flink);
        //DPrint("  BLink: %p\n", actKeyObj->Link.Blink);
        //DPrint("  NameSize: 0x%x\n", actKeyObj->NameSize);
        //DPrint("  Name: %.*ws\n", actKeyObj->NameSize/2, actKeyObj->Name);
        
        // open key
        if ( key != NULL )
        {
            ZwClose(key);
            key = NULL;
        }
        status = OpenRegistryKey(actKeyObj->Name, &key, KEY_READ);
        if ( !NT_SUCCESS(status) )
        {
            EPrint("OpenRegistryKey \"%ws\" failed! (0x%x)\n", actKeyObj->Name, status);
            goto skip2;
        }
    
        BDF bdf = { 0 };
        status = locationInfoToBDF(key, &value, &valueSize, &bdf);
        if ( status != 0 )
            goto skip2;

        if ( Flags & BDF_FLAG_SKIP_DOUBLE )
        {
            // check if bdf is already added
            // sometimes they occur multiple times
            if ( bdfExists(*PciEntryList, count, &bdf) )
            {
                DPrint("[i] Skipping existing bdf %02x:%02x:%02x\n", bdf.Bus, bdf.Device, bdf.Function);
                goto skip2;
            }
        }

        if ( count >= maxCount )
        {
            status = ExReAllocPool(PciEntryList, (PULONG)&pciEntryListSize, maxCount * 2 * sizeof(LPCI_ENTRY), POOL_TAG);
            if ( status != 0 )
                goto clean;
            maxCount = maxCount<<1;
        }

        if ( count < maxCount )
        {
            actEntry = &(*PciEntryList)[count];
            actEntry->Bdf = bdf;
            count++;
        }

        if ( Names )
        {
            PWCHAR name = NULL; // points into value
            UINT32 nameCch = 0;
            
            status = deviceDescToName(key, &value, &valueSize, &name, &nameCch);
            
            if ( status != 0 || name == NULL )
            {
                name = L"Unknown";
                nameCch = 7;
            }
            DPrint("name: %ws\n", name);

            if ( namesRestSize < nameCch + 1 )
            {
                SIZE_T namesUsedSize = namesSize - namesRestSize;
                status = ExReAllocPool(Names, (PULONG)&namesSize, namesSize*2, POOL_TAG);
                if ( status != 0 )
                    goto clean;
                namesRestSize = namesSize - namesUsedSize;
                namesPtr = &(*Names)[namesUsedSize];
            }
            
            // added for debug printing, has to be relocated after copying to out buffer
            // because of possible reallocs
#ifdef DEBUG_PRINT
            actEntry->Name = namesPtr;
#endif
            RtlStringCchPrintfExA(namesPtr, namesRestSize, &namesPtr, &namesRestSize, 0,
                                  "%ws",
                                  name);
            // add 0 termination
            namesPtr++;
            namesRestSize--;
        }
        
        DPrint("PciEntryList[%u]: %02x:%02x:%02x %s\n", 
            count, 
            actEntry->Bdf.Bus, actEntry->Bdf.Device, actEntry->Bdf.Function, actEntry->Name);

    skip2:
        actKeyLink = RemoveTailList(&subKeys1NamesHead);
        ExFreePool(actKeyObj);
        actKeyObj = NULL;
    }

    *PciEntryListCount = count;
    if ( NamesSize )
        *NamesSize = (UINT32)(namesSize - namesRestSize);

clean:

    if ( key != NULL )
        ZwClose(key);
    if ( firstKeyObj != NULL )
        ExFreePool(firstKeyObj);
    if ( szBuffer != NULL )
        ExFreePool(szBuffer);
    if ( value != NULL )
        ExFreePool(value);
    
    FLeave();
    return status;
}

NTSTATUS locationInfoToBDF(
    _In_ HANDLE Key,
    _Inout_ PKEY_VALUE_PARTIAL_INFORMATION *Value,
    _Inout_ PULONG ValueSize,
    _Out_ PBDF Bdf
)
{
    FEnter();
    NTSTATUS status = STATUS_BUFFER_TOO_SMALL;

    RtlZeroMemory(Bdf, sizeof(BDF));

    while ( status == STATUS_BUFFER_TOO_SMALL )
    {
        status = ReadRegData(Key, L"LocationInformation", Value, ValueSize);
        if ( status == STATUS_BUFFER_TOO_SMALL )
        {
            ExFreePool(*Value);
            *Value = NULL;
            *ValueSize = 0;
        }
    }
    if ( status != 0 )
    {
        EPrint("Reading location info failed! (0x%x)\n", status);
        goto clean;
    }

    if ( (*Value)->Type != REG_SZ
        || (*Value)->DataLength < 2 )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("Wrong type! (0x%x)\n", status);
        goto clean;
    }

    DPrint("LocationInformation: %.*ws\n", (*Value)->DataLength/2, (PWCHAR)&(*Value)->Data[0]);
        
    // LocationInformation: @System32\drivers\pci.sys,#65536;PCI bus %1, device %2, function %3;(1,0,0)
    PWCHAR locationInfo = (PWCHAR)&(*Value)->Data[0];
    ULONG locationInfoCch = (*Value)->DataLength/2;
    if ( locationInfo[locationInfoCch-1] == L'\0' )
        locationInfoCch--;
    if ( locationInfoCch == 0 )
    {
        status = STATUS_UNSUCCESSFUL;
        goto clean;
    }
    // search )
    PWCHAR ptr1 = &locationInfo[locationInfoCch-1];
    PWCHAR ptr2 = NULL;
    PWCHAR ptr3 = NULL;
    PWCHAR ptr4 = NULL;

    while ( *ptr1 != L'(' && *ptr1 != locationInfo[0] )
    {
        if ( *ptr1 == L')' )
            ptr4 = ptr1;
        else if ( *ptr1 == L',' && ptr4 && *ptr4 == L')' && ptr3 == NULL )
            ptr3 = ptr1;
        else if ( *ptr1 == L',' && ptr4 && *ptr4 == L')' && ptr3 && *ptr3 == L',' && ptr2 == NULL )
            ptr2 = ptr1;

        --ptr1;
    }
    if ( !ptr1 || !ptr2  || !ptr3  || !ptr4
        || *ptr1 != '(' || *ptr2 != L',' || *ptr3 != L',' || *ptr4 != L')' )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("Wrong format! (0x%x)\n", status);
        goto clean;
    }


    BDF bdf = {0};
    status = DecWsToU16(ptr1+1, (UINT32)(ptr2-(ptr1+1)), &bdf.Bus);
    if ( status != 0 )
        goto clean;
    status = DecWsToU16(ptr2+1, (UINT32)(ptr3-(ptr2+1)), &bdf.Device);
    if ( status != 0 )
        goto clean;
    status = DecWsToU16(ptr3+1, (UINT32)(ptr4-(ptr3+1)), &bdf.Function);
    if ( status != 0 )
        goto clean;
    DPrint("bus: 0x%x\n", bdf.Bus);
    DPrint("device: 0x%x\n", bdf.Device);
    DPrint("function: 0x%x\n", bdf.Function);

    *Bdf = bdf;

clean:
    
    FLeave();
    return status;
}

NTSTATUS deviceDescToName(
    _In_ HANDLE Key,
    _Inout_ PKEY_VALUE_PARTIAL_INFORMATION *Value,
    _Inout_ PULONG ValueSize,
    _Out_ PWCHAR *Name,
    _Out_ PUINT32 NameCch
)
{
    FEnter();
    NTSTATUS status = STATUS_BUFFER_TOO_SMALL;

    *Name = NULL;
    *NameCch = 0;

    while ( status == STATUS_BUFFER_TOO_SMALL )
    {
        status = ReadRegData(Key, L"DeviceDesc", Value, ValueSize);
        if ( status == STATUS_BUFFER_TOO_SMALL )
        {
            ExFreePool(*Value);
            *Value = NULL;
            *ValueSize = 0;
        }
    }
    if ( status != 0 )
    {
        EPrint("Reading DeviceDesc info failed! (0x%x)\n", status);
        goto clean;
    }

    if ( (*Value)->Type != REG_SZ
        || (*Value)->DataLength < 2 )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("Wrong type! (0x%x)\n", status);
        goto clean;
    }

    DPrint("DeviceDesc: %.*ws\n", (*Value)->DataLength/2, (PWCHAR)&(*Value)->Data[0]);
        
    // @oem33.inf,%pci\ven_8086&dev_3ec2desc%;Intel(R) Host Bridge/DRAM Registers - 3EC2
    PWCHAR desc = (PWCHAR)&(*Value)->Data[0];
    ULONG descCch = (*Value)->DataLength/2;
    if ( desc[descCch-1] == L'\0' )
        descCch--;
    if ( descCch == 0 )
    {
        status = STATUS_UNSUCCESSFUL;
        goto clean;
    }

    // search )
    PWCHAR ptr1 = &desc[descCch-1];

    while ( *ptr1 != L';' && *ptr1 != desc[0] )
    {
        --ptr1;
    }
    if ( !ptr1 || *ptr1 != ';' )
    {
        status = STATUS_UNSUCCESSFUL;
        EPrint("Wrong format! (0x%x)\n", status);
        goto clean;
    }

    *Name = ptr1+1;
    *NameCch = (UINT32)(((PUINT8)&desc[descCch-1] - (PUINT8)*Name) >> 1);

clean:
    
    FLeave();
    return status;
}
