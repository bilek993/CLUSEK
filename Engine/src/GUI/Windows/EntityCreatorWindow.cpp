#include "EntityCreatorWindow.h"
#include <imgui.h>
#include "../../ECS/Components/TransformComponent.h"
#include "../../Utils/ImGuiExtensions.h"

void EntityCreatorWindow::Draw()
{
	ImGui::Begin("Entity creator", &IsEnabled);

	ImGui::InputTextMultiline(	"##EntityJsonText", 
								const_cast<char*>(EntityJsonText.c_str()),
								EntityJsonText.capacity() + 1,
								ImVec2(-FLT_MIN, 350.0f),
								ImGuiInputTextFlags_CallbackResize,
								ImGuiExtensions::InputTextStringCallback,
								&EntityJsonText);
	
	if (ImGui::Button("Create"))
		CreateEntityWithComponents();

	ImGui::End();
}

void EntityCreatorWindow::CreateEntityWithComponents()
{
	const auto entity = Registry->create();

	nlohmann::json jsonObject = EntityJsonText;

	AddTags(jsonObject["Tags"]);
	AddComponents(jsonObject["Components"]);

	Logger::Debug("New entity has been created!");

	Logger::Debug("Clearing entity text field...");
	EntityJsonText = "";
}

void EntityCreatorWindow::AddTags(nlohmann::json& json)
{
}

void EntityCreatorWindow::AddComponents(nlohmann::json& json)
{
}
