#pragma once

class DebuggerDetection final
{
public:
	static bool Check();

private:
	static bool CheckIfDebuggerIsPresented();
	static bool CheckWindow();
};