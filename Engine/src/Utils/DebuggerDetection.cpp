#include <Windows.h>
#include "DebuggerDetection.h"

bool DebuggerDetection::Check()
{
#ifdef _DEBUG
	return false;
#else
	if (CheckIfDebuggerIsPresented())
		return true;

	if (CheckIfRemoteDebuggerIsPresented())
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

bool DebuggerDetection::CheckIfRemoteDebuggerIsPresented()
{
	const auto hProcess = GetCurrentProcess();
	auto found = FALSE;
	CheckRemoteDebuggerPresent(hProcess, &found);
	return found == TRUE;
}

bool DebuggerDetection::CheckWindow()
{
	return FindWindow(nullptr, "x64dbg") || FindWindow("OLLYDBG", nullptr);
}
