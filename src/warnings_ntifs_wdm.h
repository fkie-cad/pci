#ifndef _WARNING_SNOOZE_FOR_NTIFS_WDM_H_
#define _WARNING_SNOOZE_FOR_NTIFS_WDM_H_

// ntifs/wdm warnings
// 
// km\wdm.h(44276): 6553: The annotation for function 'ZwSetValueKey' on _Param_(3) does not apply to a value type.
// km\wdm.h(23464): warning C28160: Error annotation: Must succeed pool allocations are forbidden.
// km\wdm.h(23331): warning C28118: The current function is permitted to run at an IRQ level above the maximum permitted for 'ExAllocatePoolWithTag' (1)
// km\ntstrsafe.h(6765): warning C28196: The requirement that '_Param_(1)->MaximumLength>0?
// km\wdm.h(23452): warning C28160: Error annotation: Must succeed pool allocations are forbidden. Allocation failures cause a system crash.
// km\wdm.h(23311): warning C6387: 'return' could be '0' <= yes, and not our fault.
// km\wdm.h(12147): warning C6101: Returning uninitialized memory '*variable'.
// km\wdm.h(27120): warning C28252: Inconsistent annotation for 'MmGetSystemRoutineAddress': return/function has 'SAL_maxIRQL(0)' on the prior instance.
// km\wdm.h(27120): warning C28253: Inconsistent annotation for 'MmGetSystemRoutineAddress': return/function has 'SAL_maxIRQL(1)' on this instance
// \km\ntifs.h(17671) warning C28021: The parameter 'return' being annotated with 'allocates or frees memory' must be a pointer
#pragma warning( disable : 6101 6387 6553 28021 28118 28132 28160 28196 28230 28252 28253 28285 28301)

#endif
