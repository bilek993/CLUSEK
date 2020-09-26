#pragma once
#include <core/hashed_string.hpp>

struct Tags final
{
	inline static const entt::hashed_string::hash_type REQUIRES_REBUILD = "Requires rebuild"_hs; // This tag is used only internally
	
	inline static const entt::hashed_string::hash_type MAIN_CAMERA = "Main Camera"_hs;
};
