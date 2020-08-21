#pragma once
#include "BaseWindow.h"

class GrassWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void FixValues() const;
};