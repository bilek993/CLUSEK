#pragma once
#include "PxPhysicsAPI.h"

struct PhysicsMaterialComponent final
{
	float StaticFriction;
	float DynamicFriction;
	float Restitution;
	std::string SurfaceFilterType = "Drivable";
	physx::PxMaterial *Material;
};