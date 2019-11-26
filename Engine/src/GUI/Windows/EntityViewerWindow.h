#pragma once
#include "BaseWindow.h"

class EntityViewerWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void DrawList();
};
