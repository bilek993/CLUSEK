#pragma once
#include "BaseComponentEditor.h"

class TransformComponentEditor final : public BaseComponentEditor
class TransformComponentEditor final : public BaseComponentEditor<TransformComponent>
{
protected:
	void Draw() override;
};