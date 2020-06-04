#pragma once
#include "BaseWindow.h"
#include <imgui.h>

class EntityEditorWindow final : public BaseWindow
{
protected:
	void Draw() override;
};