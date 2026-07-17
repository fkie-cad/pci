#pragma once


//INT parsePlainBytes(
//    _In_ const char* Raw, 
//    _Inout_ UINT8** Buffer, 
//    _Inout_ PULONG Size, 
//    _In_ ULONG MaxBytes);


#define IS_NUM_CHAR(__char__) \
    ( __char__ >= '0' && __char__ <= '9' )

#define IS_LC_HEX_CHAR(__char__) \
    ( __char__ >= 'a' && __char__ <= 'f' )

#define IS_UC_HEX_CHAR(__char__) \
    ( __char__ >= 'A' && __char__ <= 'F' )

#define IN_HEX_RANGE(__char__) \
    ( IN_NUM(__char__) || ( __char__ >= 'a' && __char__ <= 'f' )  || ( __char__ >= 'A' && __char__ <= 'F' ) )

/**
 * Parse plain byte string into byte array
 */
INT parsePlainBytes(
    _In_ const char* Raw, 
    _Inout_ UINT8** Buffer, 
    _Inout_ PULONG Size, 
    _In_ ULONG MaxBytes
)
{
    ULONG i, j;
    SIZE_T arg_ln = strlen(Raw);
    UINT8* p = NULL;
    BOOL malloced = FALSE;
    ULONG buffer_ln;
    int s = 0;

    UINT8 m1, m2;

    if ( arg_ln > MaxBytes * 2ULL )
    {
        EPrint("Data too big !\n");
        return ERROR_BUFFER_OVERFLOW;
    }
    
    buffer_ln = (ULONG) (arg_ln / 2);

    if ( arg_ln == 0 )
    {
        EPrint("Buffer is empty!\n");
        return ERROR_INVALID_PARAMETER;
    }
    if ( arg_ln % 2 != 0 )
    {
        EPrint("Buffer data is not byte aligned!\n");
        return ERROR_INVALID_PARAMETER;
    }

    if ( *Buffer != NULL && buffer_ln > *Size )
    {
        EPrint("Buffer is too small %u > %u!\n", buffer_ln, *Size);
        return ERROR_INVALID_PARAMETER;
    }

    if ( *Buffer == NULL )
    {
        p = (UINT8*) malloc(buffer_ln);
        if ( p == NULL )
        {
            EPrint("No memory!\n");
            return GetLastError();
        }
        malloced = TRUE;
    }
    else
    {
        p = *Buffer;
    }

    for ( j = 0; j < buffer_ln; j++ )
    {
        i = j * 2;

        if ( IS_NUM_CHAR(Raw[i]) )
            m1 = 0x30;
        else if ( IS_UC_HEX_CHAR(Raw[i]) )
            m1 = 0x37;
        else if ( IS_LC_HEX_CHAR(Raw[i]) )
            m1 = 0x57;
        else
        {
            s = ERROR_INVALID_PARAMETER;
            EPrint("Byte string not in hex range!\n");
            break;
        }
        
        if ( IS_NUM_CHAR(Raw[i+1]) )
            m2 = 0x30;
        else if ( IS_UC_HEX_CHAR(Raw[i+1]) )
            m2 = 0x37;
        else if ( IS_LC_HEX_CHAR(Raw[i+1]) )
            m2 = 0x57;
        else
        {
            s = ERROR_INVALID_PARAMETER;
            EPrint("Byte string not in hex range!\n");
            break;
        }
        p[j] = ((Raw[i] - m1)<<4) | ((Raw[i+1] - m2) & 0x0F);
    }
    
    if ( s != 0 )
    {
        if ( malloced && p )
            free(p);
    }
    else
    {
        *Size = buffer_ln;
        *Buffer = p;
    }

    return s;
}

// Convert decimal wchar string into uint16
FORCEINLINE
NTSTATUS DecWsToU16(PWCHAR String, UINT32 Cch, PUINT16 Result)
{
    FEnter();
    
    NTSTATUS status = 0;

    UINT32 v = 0;
    UINT32 mod = 1;

    if ( !String || String[0] == 0 || Cch < 1 )
    {
        status = STATUS_INVALID_PARAMETER;
        EPrint("Wrong parameter! (0x%x)\n", status);
        return status;
    }

    PWCHAR ptr = &String[Cch-1];

    while ( ptr )
    {
        if ( !IS_NUM_CHAR((*ptr)) )
        {
            status = STATUS_UNSUCCESSFUL;
            EPrint("Wrong format! (0x%x)\n", status);
            return status;
        }
            
        v += ((*ptr)-0x30) * mod;
        if ( v > 0xFFFF )
        {
            status = STATUS_INTEGER_OVERFLOW; 
            goto clean; 
        }
        mod *= 10;

        if ( ptr == String )
            break;
        ptr--;
    }

    *Result = (UINT16)v;

    clean:
    return status;
}
