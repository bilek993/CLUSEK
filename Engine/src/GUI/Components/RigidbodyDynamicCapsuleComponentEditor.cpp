#include "RigidbodyDynamicCapsuleComponentEditor.h"

void RigidbodyDynamicCapsuleComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	physx::PxShape* shapes;
	component->Body->getShapes(&shapes, component->Body->getNbShapes());

	physx::PxCapsuleGeometry geometry;
	shapes->getCapsuleGeometry(geometry);

	DrawGeometryParameters(geometry);

	shapes->setGeometry(geometry);
}

void RigidbodyDynamicCapsuleComponentEditor::DrawGeometryParameters(physx::PxCapsuleGeometry& geometry) const
{
	auto height = geometry.halfHeight * 2.0f;

	ImGui::InputFloat("Height", &height);
	ImGui::InputFloat("Radius", &geometry.radius);

	geometry.halfHeight = height / 2.0f;
}

void RigidbodyDynamicCapsuleComponentEditor::HandleIncorrectValues(physx::PxCapsuleGeometry& geometry) const
{
	geometry.radius = std::max(geometry.radius, EPSILON);
	geometry.halfHeight = std::max(geometry.halfHeight, EPSILON);
}
