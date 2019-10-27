#include "PhysicsErrorCallback.h"
#include "../Utils/Logger.h"

void PhysicsErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, const int line)
{
	Logger::Error(
		"PhysX reported an error! Line: " + 
		std::to_string(line) + " " +
		"File: " + std::string(file) + " " +
		"Message: " + std::string(message));
}
