#include "DebuggerDetection.h"

bool DebuggerDetection::Check()
{
#ifdef _DEBUG
	return false;
#else
	return true;
#endif
}
