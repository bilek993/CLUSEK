#include "RigidbodyDynamicBoxComponentEditor.h"

void RigidbodyDynamicBoxComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	physx::PxShape* shapes;
	component->Body->getShapes(&shapes, component->Body->getNbShapes());

	physx::PxBoxGeometry geometry;
	shapes->getBoxGeometry(geometry);

	DrawGeometryParameters(geometry);

	shapes->setGeometry(geometry);
}

void RigidbodyDynamicBoxComponentEditor::DrawGeometryParameters(physx::PxBoxGeometry& geometry) const
{
	ImGui::InputFloat("Width", &geometry.halfExtents.x);
	ImGui::InputFloat("Height", &geometry.halfExtents.y);
	ImGui::InputFloat("Depth", &geometry.halfExtents.z);
}
