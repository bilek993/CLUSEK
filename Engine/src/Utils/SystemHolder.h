#pragma once
#include <string>
#include <utility>
#include "../ECS/Systems/BaseSystem.h"

struct SystemHolder final
{
	SystemHolder(const std::string& name, std::unique_ptr<BaseSystem>& baseSystem, bool enabled)
		: Name(name),
		  System(std::move(baseSystem)),
		  Enabled(enabled) { }

	std::string Name;
	std::unique_ptr<BaseSystem> System;
	bool Enabled = true;
};
