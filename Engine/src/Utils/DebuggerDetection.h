#pragma once

class DebuggerDetection final
{
public:
	static bool Check();

private:
	static bool CheckIfDebuggerIsPresented();
	static bool CheckIfRemoteDebuggerIsPresented();
	static bool CheckWindow();
	static bool CheckCloseHandleException();
};