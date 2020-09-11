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

	if (CheckCloseHandleException())
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

bool DebuggerDetection::CheckCloseHandleException()
{
	auto* hInvalid = reinterpret_cast<HANDLE>(0xDEADBEEF);
	auto found = FALSE;

	__try
	{
		CloseHandle(hInvalid);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		found = TRUE;
	}

	return found == TRUE;
}
