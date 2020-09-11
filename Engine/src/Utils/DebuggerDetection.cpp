#include <Windows.h>
#include "DebuggerDetection.h"

bool DebuggerDetection::Check()
{
#ifdef _DEBUG
	return false;
#else
	if (CheckIfDebuggerIsPresented())
		return true;

	if (CheckWindow())
		return true;
	
	return false;
#endif
}

bool DebuggerDetection::CheckIfDebuggerIsPresented()
{
	return IsDebuggerPresent();
}

bool DebuggerDetection::CheckWindow()
{
	return FindWindow(nullptr, "x64dbg") || FindWindow("OLLYDBG", nullptr);
}
