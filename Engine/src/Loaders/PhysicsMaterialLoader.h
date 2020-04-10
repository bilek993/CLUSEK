#pragma once
#include <string>
#include "../Physics/PhysicsMaterialManager.h"
#include <json.hpp>

class PhysicsMaterialLoader final
{
public:
	static void Load(const std::string& path, PhysicsMaterialManager* physicsMaterialMaterialManager, physx::PxPhysics* physics);

private:
	static void ParseMaterial(nlohmann::json& jsonObject, PhysicsMaterialManager* physicsMaterialMaterialManager, physx::PxPhysics* physics);

	inline static const float DEFAULT_STATIC_FRICTION = 0.75f;
	inline static const float DEFAULT_DYNAMIC_FRICTION = 0.75f;
	inline static const float DEFAULT_TIRE_FRICTION = 0.75f;
	inline static const float DEFAULT_RESTITUTION = 0.1f;
};
