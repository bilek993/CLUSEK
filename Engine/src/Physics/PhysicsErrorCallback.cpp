#include "PhysicsErrorCallback.h"
#include "../Utils/Logger.h"

void PhysicsErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, const int line)
{
	Logger::Error("PhysX reported an error! Engine may work unstable after this error!");
	Logger::Error(
		"Line: " + std::to_string(line) + "\n" +
		"File: " + std::string(file) + "\n" +
		"Message: " + std::string(message));
}
