#pragma once
#include "BaseWindow.h"

class GrassWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void HandleIncorrectValues() const;

	inline static const float EPSILON = 0.001f;
};