#pragma once
#include "PxPhysicsAPI.h"

struct PhysicsMaterialComponent final
{
	float StaticFriction;
	float DynamicFriction;
	float Restitution;
	physx::PxMaterial *Material;
};