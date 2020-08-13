#include "EntityEditorWindow.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../ECS/Components/CameraComponent.h"
#include "../../ECS/Components/CameraTargetComponent.h"
#include "../../ECS/Components/CameraVehicleAnimatedTargetComponent.h"
#include "../../ECS/Components/ModelRenderComponent.h"
#include "../../ECS/Components/PhysicsMaterialComponent.h"
#include "../../ECS/Components/RigidbodyDynamicBoxComponent.h"
#include "../../ECS/Components/RigidbodyDynamicCapsuleComponent.h"
#include "../../ECS/Components/RigidbodyDynamicCylinderComponent.h"
#include "../../ECS/Components/RigidbodyDynamicSphereComponent.h"
#include "../../ECS/Components/RigidbodyStaticBoxComponent.h"
#include "../../ECS/Components/RigidbodyStaticCapsuleComponent.h"
#include "../../ECS/Components/RigidbodyStaticCylinderComponent.h"
#include "../../ECS/Components/RigidbodyStaticHeightFieldsComponent.h"
#include "../../ECS/Components/RigidbodyStaticPlaneComponent.h"
#include "../../ECS/Components/RigidbodyStaticSphereComponent.h"
#include "../../ECS/Components/SkyboxComponent.h"
#include "../../ECS/Components/TerrainComponent.h"
#include "../../ECS/Components/VehicleComponent.h"
#include "../../ECS/Components/VehiclePlayerControllerComponent.h"
#include "../../ECS/Components/CommentComponent.h"

void EntityEditorWindow::Draw()
{
	ImGui::Begin("Entity Editor", &IsEnabled);

	if (*EntitySelected)
		DrawComponentsForEntity();
	else
		DrawNoElements();

	ImGui::End();
}

void EntityEditorWindow::DrawComponentsForEntity()
{
	REGISTER_COMPONENT_EDITOR(CameraComponent, "Camera Component");
	REGISTER_COMPONENT_EDITOR(CameraTargetComponent, "Camera Target Component");
	REGISTER_COMPONENT_EDITOR(CameraVehicleAnimatedTargetComponent, "Camera Vehicle Animated Target Component");
	REGISTER_COMPONENT_EDITOR(ModelRenderComponent, "Model Render Component");
	REGISTER_COMPONENT_EDITOR(PhysicsMaterialComponent, "Physics Material Component");
	REGISTER_COMPONENT_EDITOR(RigidbodyDynamicBoxComponent, "Rigidbody Dynamic Box Component");
	REGISTER_COMPONENT_EDITOR(RigidbodyDynamicCapsuleComponent, "Rigidbody Dynamic Capsule Component");
	REGISTER_COMPONENT_EDITOR(RigidbodyDynamicCylinderComponent, "Rigidbody Dynamic Cylinder Component");
	REGISTER_COMPONENT_EDITOR(RigidbodyDynamicSphereComponent, "Rigidbody Dynamic Sphere Component");
	REGISTER_COMPONENT_EDITOR(RigidbodyStaticBoxComponent, "Rigidbody Static Box Component");
	REGISTER_COMPONENT_EDITOR(RigidbodyStaticCapsuleComponent, "Rigidbody Static Capsule Component");
	REGISTER_COMPONENT_EDITOR(RigidbodyStaticCylinderComponent, "Rigidbody Static Cylinder Component");
	REGISTER_COMPONENT_EDITOR(RigidbodyStaticHeightFieldsComponent, "Rigidbody Static Height Fields Component");
	REGISTER_COMPONENT_EDITOR(RigidbodyStaticPlaneComponent, "Rigidbody Static Plane Component");
	REGISTER_COMPONENT_EDITOR(RigidbodyStaticSphereComponent, "Rigidbody Static Sphere Component");
	REGISTER_COMPONENT_EDITOR(SkyboxComponent, "Skybox Component");
	REGISTER_COMPONENT_EDITOR(TerrainComponent, "Terrain Component");
	REGISTER_COMPONENT_EDITOR(TransformComponent, "Transform Component");
	REGISTER_COMPONENT_EDITOR(VehicleComponent, "Vehicle Component");
	REGISTER_COMPONENT_EDITOR(VehiclePlayerControllerComponent, "Vehicle Player Controller Component");
	REGISTER_COMPONENT_EDITOR(WheelComponent, "Wheel Component");
	REGISTER_COMPONENT_EDITOR(CommentComponent, "Comment Component");
	REGISTER_COMPONENT_EDITOR(GrassComponent, "Grass Component");
}

void EntityEditorWindow::DrawNoElements() const
{
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2.0f) - 117.0f);
	ImGui::Text("Please select entity to enable this editor.");
}
