#pragma once

#include <winerror.h>

#define MAX_PATH 260

#define far
#define near

#define WINAPI      __stdcall

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL near           *PBOOL;
typedef BOOL far            *LPBOOL;
typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;
typedef int near            *PINT;
typedef int far             *LPINT;
typedef WORD near           *PWORD;
typedef WORD far            *LPWORD;
typedef long far            *LPLONG;
typedef DWORD near          *PDWORD;
typedef DWORD far           *LPDWORD;
typedef void far            *LPVOID;
typedef CONST void far      *LPCVOID;

#define STD_INPUT_HANDLE    ((DWORD)-10)
#define STD_OUTPUT_HANDLE   ((DWORD)-11)
#define STD_ERROR_HANDLE    ((DWORD)-12)

#define WINBASEAPI DECLSPEC_IMPORT

DWORD
WINAPI
GetLastError(
    VOID
    );



typedef struct _CONSOLE_CURSOR_INFO {
    DWORD  dwSize;
    BOOL   bVisible;
} CONSOLE_CURSOR_INFO, *PCONSOLE_CURSOR_INFO;

typedef struct _COORD {
    SHORT X;
    SHORT Y;
} COORD, *PCOORD;

typedef struct _SMALL_RECT {
    SHORT Left;
    SHORT Top;
    SHORT Right;
    SHORT Bottom;
} SMALL_RECT, *PSMALL_RECT;

typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
    COORD dwSize;
    COORD dwCursorPosition;
    WORD  wAttributes;
    SMALL_RECT srWindow;
    COORD dwMaximumWindowSize;
} CONSOLE_SCREEN_BUFFER_INFO, *PCONSOLE_SCREEN_BUFFER_INFO;


HANDLE
WINAPI
GetStdHandle(
    _In_ DWORD nStdHandle
    );

BOOL
WINAPI
SetStdHandle(
    _In_ DWORD nStdHandle,
    _In_ HANDLE hHandle
    );

BOOL
WINAPI
GetConsoleCursorInfo(
    _In_ HANDLE hConsoleOutput,
    _Out_ PCONSOLE_CURSOR_INFO lpConsoleCursorInfo
    );

BOOL
WINAPI
SetConsoleCursorInfo(
    _In_ HANDLE hConsoleOutput,
    _In_ CONST CONSOLE_CURSOR_INFO* lpConsoleCursorInfo
    );

BOOL
WINAPI
GetConsoleScreenBufferInfo(
    _In_ HANDLE hConsoleOutput,
    _Out_ PCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo
    );

BOOL
WINAPI
SetConsoleCursorPosition(
    _In_ HANDLE hConsoleOutput,
    _In_ COORD dwCursorPosition
    );

VOID
WINAPI
Sleep(
    _In_ DWORD dwMilliseconds
    );


typedef struct _SYSTEMTIME {
  WORD wYear;
  WORD wMonth;
  WORD wDayOfWeek;
  WORD wDay;
  WORD wHour;
  WORD wMinute;
  WORD wSecond;
  WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

void GetSystemTime(
  _Out_ LPSYSTEMTIME lpSystemTime
);

void GetLocalTime(
  _Out_ LPSYSTEMTIME lpSystemTime
);


//
// fileapi.h
//

WINBASEAPI
_Success_(return != 0 && return < nBufferLength)
DWORD
WINAPI
GetFullPathNameW(
    _In_ LPCWSTR lpFileName,
    _In_ DWORD nBufferLength,
    _Out_writes_to_opt_(nBufferLength,return + 1) LPWSTR lpBuffer,
    _Outptr_opt_ LPWSTR* lpFilePart
    );

WINBASEAPI
BOOL
WINAPI
GetFileSizeEx(
    _In_ HANDLE hFile,
    _Out_ PLARGE_INTEGER lpFileSize
    );

BOOL QueryPerformanceFrequency(
  _Out_ LARGE_INTEGER *lpFrequency
);

BOOL QueryPerformanceCounter(
  _Out_ LARGE_INTEGER *lpPerformanceCount
);
