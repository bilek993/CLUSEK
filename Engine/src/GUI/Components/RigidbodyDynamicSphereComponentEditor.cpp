#include "RigidbodyDynamicSphereComponentEditor.h"

void RigidbodyDynamicSphereComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	physx::PxShape* shapes;
	component->Body->getShapes(&shapes, component->Body->getNbShapes());

	physx::PxSphereGeometry geometry;
	shapes->getSphereGeometry(geometry);

	DrawGeometryParameters(geometry);

	shapes->setGeometry(geometry);
}

void RigidbodyDynamicSphereComponentEditor::DrawGeometryParameters(physx::PxSphereGeometry& geometry) const
{
	ImGui::InputFloat("Radius", &geometry.radius);
}
