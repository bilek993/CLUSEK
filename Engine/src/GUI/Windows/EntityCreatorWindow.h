#pragma once
#include <json.hpp>
#include "BaseWindow.h"

class EntityCreatorWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void CreateEntityWithComponents();

	void AddTags(nlohmann::json& json);
	void AddComponents(nlohmann::json& json);
	
	std::string EntityJsonText{};
};