#pragma once
#include "BaseWindow.h"

class PostProcessingWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawStack() const;

	inline static const std::string PAYLOAD_CELL_NAME = "PP_CELL";
};
