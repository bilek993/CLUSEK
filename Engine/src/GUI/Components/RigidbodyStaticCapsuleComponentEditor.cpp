#include "RigidbodyStaticCapsuleComponentEditor.h"

void RigidbodyStaticCapsuleComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	physx::PxShape* shapes;
	component->Body->getShapes(&shapes, component->Body->getNbShapes());

	physx::PxCapsuleGeometry geometry;
	shapes->getCapsuleGeometry(geometry);

	DrawGeometryParameters(geometry);

	shapes->setGeometry(geometry);
}

void RigidbodyStaticCapsuleComponentEditor::DrawGeometryParameters(physx::PxCapsuleGeometry& geometry) const
{
	auto height = geometry.halfHeight * 2.0f;

	ImGui::InputFloat("Width", &height);
	ImGui::InputFloat("Height", &geometry.radius);

	geometry.halfHeight = height / 2.0f;
}
