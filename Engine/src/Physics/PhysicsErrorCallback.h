#pragma once
#include <foundation/PxErrorCallback.h>

class PhysicsErrorCallback final : public physx::PxErrorCallback
{
public:
	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
};