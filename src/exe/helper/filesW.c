#pragma warning( push )
#include "../../warnings_ntifs_wdm.h"
#include <ntifs.h>
#include <ntstrsafe.h>
#pragma warning( pop )

#include <stdio.h>
#include <direct.h>
#include <malloc.h>

#include "../inc/nt.h"

#include "filesW.h"

//warning C6054: String 'Buffer' might not be zero-terminated
#pragma warning( disable : 6054 )
ULONG ntGetFullPathName(
    _In_ PWCHAR FileName,
    _In_ ULONG BufferCb,
    _Out_writes_bytes_to_(BufferCb, return) PWSTR Buffer,
    _Out_opt_ PWSTR *FilePart
)
{
    INT s;
    
    if ( FilePart )
        *FilePart = NULL;

    if ( BufferCb < 8 )
    {
        //SetLastError(ERROR_BUFFER_OVERFLOW);
        return 0;
    }
    *(PUINT64)Buffer = NT_PATH_PREFIX_W;
    BufferCb -= 8;
    Buffer += 4;

    // wants and returns cb
    s = RtlGetFullPathName_U(FileName, BufferCb, Buffer, FilePart);

    return s + 8;
}
#pragma warning( default : 6054 )
