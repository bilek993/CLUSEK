#pragma once
#include "BaseWindow.h"
#include <imgui.h>
#include "../Components/TransformComponentEditor.h"
#include "../Components/CameraComponentEditor.h"
#include "../Components/CameraTargetComponentEditor.h"
#include "../Components/CameraVehicleAnimatedTargetComponentEditor.h"
#include "../Components/ModelRenderComponentEditor.h"
#include "../Components/PhysicsMaterialComponentEditor.h"
#include "../Components/RigidbodyDynamicBoxComponentEditor.h"
#include "../Components/RigidbodyDynamicCapsuleComponentEditor.h"
#include "../Components/RigidbodyDynamicCylinderComponentEditor.h"
#include "../Components/RigidbodyDynamicSphereComponentEditor.h"
#include "../Components/RigidbodyStaticBoxComponentEditor.h"
#include "../Components/RigidbodyStaticCapsuleComponentEditor.h"
#include "../Components/RigidbodyStaticCylinderComponentEditor.h"
#include "../Components/RigidbodyStaticHeightFieldsComponentEditor.h"
#include "../Components/RigidbodyStaticPlaneComponentEditor.h"
#include "../Components/RigidbodyStaticSphereComponentEditor.h"
#include "../Components/SkyboxComponentEditor.h"
#include "../Components/TerrainComponentEditor.h"
#include "../Components/VehicleComponentEditor.h"
#include "../Components/VehiclePlayerControllerComponentEditor.h"
#include "../Components/WheelComponentEditor.h"

#define REGISTER_COMPONENT_EDITOR(COMPONENT_CLASS, COMPONENT_NAME) DrawComponentDetails<COMPONENT_CLASS>(SelectedEntity, COMPONENT_CLASS##EditorInstance, COMPONENT_NAME)

class EntityEditorWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawComponentsForEntity();
	void DrawNoElements() const;

	template <class C>
	void DrawComponentDetails(const entt::entity* entity, BaseComponentEditor<C>& editorInstance, const std::string& name) const;

	CameraComponentEditor CameraComponentEditorInstance;
	CameraTargetComponentEditor CameraTargetComponentEditorInstance;
	CameraVehicleAnimatedTargetComponentEditor CameraVehicleAnimatedTargetComponentEditorInstance;
	ModelRenderComponentEditor ModelRenderComponentEditorInstance;
	PhysicsMaterialComponentEditor PhysicsMaterialComponentEditorInstance;
	RigidbodyDynamicBoxComponentEditor RigidbodyDynamicBoxComponentEditorInstance;
	RigidbodyDynamicCapsuleComponentEditor RigidbodyDynamicCapsuleComponentEditorInstance;
	RigidbodyDynamicCylinderComponentEditor RigidbodyDynamicCylinderComponentEditorInstance;
	RigidbodyDynamicSphereComponentEditor RigidbodyDynamicSphereComponentEditorInstance;
	RigidbodyStaticBoxComponentEditor RigidbodyStaticBoxComponentEditorInstance;
	RigidbodyStaticCapsuleComponentEditor RigidbodyStaticCapsuleComponentEditorInstance;
	RigidbodyStaticCylinderComponentEditor RigidbodyStaticCylinderComponentEditorInstance;
	RigidbodyStaticHeightFieldsComponentEditor RigidbodyStaticHeightFieldsComponentEditorInstance;
	RigidbodyStaticPlaneComponentEditor RigidbodyStaticPlaneComponentEditorInstance;
	RigidbodyStaticSphereComponentEditor RigidbodyStaticSphereComponentEditorInstance;
	SkyboxComponentEditor SkyboxComponentEditorInstance;
	TerrainComponentEditor TerrainComponentEditorInstance;
	TransformComponentEditor TransformComponentEditorInstance;
	VehicleComponentEditor VehicleComponentEditorInstance;
	VehiclePlayerControllerComponentEditor VehiclePlayerControllerComponentEditorInstance;
	WheelComponentEditor WheelComponentEditorInstance;
};

template <class C>
void EntityEditorWindow::DrawComponentDetails(const entt::entity* entity, BaseComponentEditor<C>& editorInstance, const std::string& name) const
{
	if (!Registry->has<C>(*entity))
		return;

	if (ImGui::CollapsingHeader(name.c_str()))
	{
		editorInstance.Update(	DeltaTime, 
								Config, 
								RenderSettings, 
								Systems, 
								RenderSystemId,
								IoData, 
								CurrentPostProcessingSettings, 
								Registry, 
								SelectedEntity, 
								EntitySelected);
	}
}
