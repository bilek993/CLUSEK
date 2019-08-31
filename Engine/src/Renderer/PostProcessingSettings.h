#pragma once
#include "PostProcessing/ReusablePostProcessing.h"
#include "vector"

struct PostProcessingSettings final
{
	std::vector<std::unique_ptr<ReusablePostProcessing>> List;
	DXGI_FORMAT Format;
};
