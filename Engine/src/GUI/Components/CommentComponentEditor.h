#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/CommentComponent.h"

class CommentComponentEditor final : public BaseComponentEditor<CommentComponent>
{
protected:
	void Draw() override;
};
