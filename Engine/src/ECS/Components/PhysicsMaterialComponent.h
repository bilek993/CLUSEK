#pragma once
#include "PxPhysicsAPI.h"

struct PhysicsMaterialComponent final
{
	std::string Name;
	std::string SurfaceFilterType = "Drivable";
	physx::PxMaterial *Material;
};