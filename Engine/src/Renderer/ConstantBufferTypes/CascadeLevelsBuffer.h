#pragma once

struct CascadeLevelsBuffer final
{
	std::array<float, ShadowCamera::SHADOW_CASCADES_COUNT> CascadeEnds;
};
