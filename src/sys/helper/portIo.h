#pragma once


#define PROC_DELAY (50)

NTSTATUS pio_Read_8_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT8 Index,
    _Out_ PUINT8 Result
);
#pragma alloc_text (NONPAGE, pio_Read_8_8)
NTSTATUS pio_Read_8_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT8 Index,
    _Out_ PUINT8 Result
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outbyte(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        *Result = __inbyte(DataPort);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Read_16_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT16 Index,
    _Out_ PUINT8 Result
);
#pragma alloc_text (NONPAGE, pio_Read_16_8)
NTSTATUS pio_Read_16_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT16 Index,
    _Out_ PUINT8 Result
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outword(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        *Result = __inbyte(DataPort);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Read_16_16(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT16 Index,
    _Out_ PUINT16 Result
);
#pragma alloc_text (NONPAGE, pio_Read_16_16)
NTSTATUS pio_Read_16_16(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT16 Index,
    _Out_ PUINT16 Result
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outword(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        *Result = __inword(DataPort);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Read_32_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT32 Index,
    _Out_ PUINT8 Result
);
#pragma alloc_text (NONPAGE, pio_Read_32_8)
NTSTATUS pio_Read_32_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT32 Index,
    _Out_ PUINT8 Result
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outdword(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        *Result = __inbyte(DataPort);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Read_32_32(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT32 Index,
    _Out_ PUINT32 Result
);
#pragma alloc_text (NONPAGE, pio_Read_32_32)
NTSTATUS pio_Read_32_32(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT32 Index,
    _Out_ PUINT32 Result
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outdword(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        *Result = __indword(DataPort);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Read_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT8 Index,
    _Inout_ PUINT8 Buffer,
    _In_ UINT8 Size
);
#pragma alloc_text (NONPAGE, pio_Read_8)
NTSTATUS pio_Read_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT8 Index,
    _Inout_ PUINT8 Buffer,
    _In_ UINT8 Size
)
{
    NTSTATUS status = STATUS_SUCCESS;
    
    UINT8 i, j;
    UINT8 end = Index + Size;

    for ( i = Index, j = 0; i < end; i++, j++ )
    {
        status = pio_Read_8_8(IndexPort, DataPort, i, &Buffer[j]);
    }

    return status;
}



NTSTATUS pio_Write_8_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT8 Index,
    _In_ UINT8 Value
);
#pragma alloc_text (NONPAGE, pio_Write_8_8)
NTSTATUS pio_Write_8_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT8 Index,
    _In_ UINT8 Value
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outbyte(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        __outbyte(DataPort, Value);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Write_16_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT16 Index,
    _In_ UINT8 Value
);
#pragma alloc_text (NONPAGE, pio_Write_16_8)
NTSTATUS pio_Write_16_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT16 Index,
    _In_ UINT8 Value
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outword(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        __outbyte(DataPort, Value);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Write_16_16(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT16 Index,
    _In_ UINT16 Value
);
#pragma alloc_text (NONPAGE, pio_Write_16_16)
NTSTATUS pio_Write_16_16(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT16 Index,
    _In_ UINT16 Value
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outword(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        __outword(DataPort, Value);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Write_32_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT32 Index,
    _In_ UINT8 Value
);
#pragma alloc_text (NONPAGE, pio_Write_32_8)
NTSTATUS pio_Write_32_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT32 Index,
    _In_ UINT8 Value
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outdword(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        __outbyte(DataPort, Value);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Write_32_16(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT32 Index,
    _In_ UINT16 Value
);
#pragma alloc_text (NONPAGE, pio_Write_32_16)
NTSTATUS pio_Write_32_16(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT32 Index,
    _In_ UINT16 Value
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outdword(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        __outword(DataPort, Value);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Write_32_32(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT32 Index,
    _In_ UINT32 Value
);
#pragma alloc_text (NONPAGE, pio_Write_32_32)
NTSTATUS pio_Write_32_32(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT32 Index,
    _In_ UINT32 Value
)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        _disable();
        __outdword(IndexPort, Index);
        KeStallExecutionProcessor(PROC_DELAY);
        __outdword(DataPort, Value);
        KeStallExecutionProcessor(PROC_DELAY);
        _enable();
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS pio_Write_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT8 Index,
    _In_ PUINT8 Buffer,
    _In_ UINT8 Size
);
#pragma alloc_text (NONPAGE, pio_Write_8)
NTSTATUS pio_Write_8(
    _In_ UINT16 IndexPort,
    _In_ UINT16 DataPort,
    _In_ UINT8 Index,
    _In_ PUINT8 Buffer,
    _In_ UINT8 Size
)
{
    NTSTATUS status = STATUS_SUCCESS;

    UINT8 i, j;
    UINT8 end = Index + Size;

    for ( i = Index, j = 0; i < end; i++, j++ )
    {
        status = pio_Write_8_8(IndexPort, DataPort, i, Buffer[j]);
    }

    return status;
}
