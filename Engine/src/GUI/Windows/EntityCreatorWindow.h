#pragma once
#include "BaseWindow.h"

class EntityCreatorWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void CreateEntityWithComponents();
	
	std::string EntityJsonText{};
};