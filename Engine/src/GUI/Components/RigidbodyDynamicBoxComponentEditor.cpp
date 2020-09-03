#include "RigidbodyDynamicBoxComponentEditor.h"

void RigidbodyDynamicBoxComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	physx::PxShape* shapes;
	component->Body->getShapes(&shapes, component->Body->getNbShapes());

	physx::PxBoxGeometry geometry;
	shapes->getBoxGeometry(geometry);

	DrawGeometryParameters(geometry);
	HandleIncorrectValues(geometry);

	shapes->setGeometry(geometry);
}

void RigidbodyDynamicBoxComponentEditor::DrawGeometryParameters(physx::PxBoxGeometry& geometry) const
{
	ImGui::InputFloat("Width", &geometry.halfExtents.x);
	ImGui::InputFloat("Height", &geometry.halfExtents.y);
	ImGui::InputFloat("Depth", &geometry.halfExtents.z);
}

void RigidbodyDynamicBoxComponentEditor::HandleIncorrectValues(physx::PxBoxGeometry& geometry) const
{
	geometry.halfExtents.x = std::max(geometry.halfExtents.x, EPSILON);
	geometry.halfExtents.y = std::max(geometry.halfExtents.y, EPSILON);
	geometry.halfExtents.z = std::max(geometry.halfExtents.z, EPSILON);
}
