#pragma once

#ifndef ALIGN_UP_TO_NEXT_BY
#define ALIGN_UP_TO_NEXT_BY(Address, Align) ( (((Align)-1)&((ULONG_PTR)(Address))) ? ( ((ULONG_PTR)(Address) + (Align) - 1) & ~((Align) - 1) ) : ((ULONG_PTR)(Address)+(Align)) )
#endif

#ifndef ALIGN_UP_BY
#define ALIGN_UP_BY(Address, Align) ( ((ULONG_PTR)(Address) + (Align) - 1) & ~((Align) - 1) )
#endif

#ifndef ALIGN_DOWN_BY
#define ALIGN_DOWN_BY(Address, Align) ((ULONG_PTR)(Address) & ~((ULONG_PTR)(Align) - 1))
#endif

#ifndef _countof
#define _countof(array) (sizeof(array) / sizeof(array[0]))
#endif 

#ifndef offsetof
#define offsetof(s,m) ((size_t)&(((s*)0)->m))
#endif 


#ifndef BYTEOF
#define BYTEOF(d,i)     (((PUINT8)&(d))[i])
#endif 
#ifndef UINT16OF
#define UINT16OF(d,i)     (*(PUINT16)&(((PUINT8)&(d))[i]))
#endif 
#ifndef UINT32OF
#define UINT32OF(d,i)     (*(PUINT32)&(((PUINT8)&(d))[i]))
#endif 
#ifndef UINT64OF
#define UINT64OF(d,i)     (*(PUINT64)&(((PUINT8)&(d))[i]))
#endif 
