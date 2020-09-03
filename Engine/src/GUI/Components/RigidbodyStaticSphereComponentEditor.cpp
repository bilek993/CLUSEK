#include "RigidbodyStaticSphereComponentEditor.h"

void RigidbodyStaticSphereComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	physx::PxShape* shapes;
	component->Body->getShapes(&shapes, component->Body->getNbShapes());

	physx::PxSphereGeometry geometry;
	shapes->getSphereGeometry(geometry);

	DrawGeometryParameters(geometry);

	shapes->setGeometry(geometry);
}

void RigidbodyStaticSphereComponentEditor::DrawGeometryParameters(physx::PxSphereGeometry& geometry) const
{
	ImGui::InputFloat("Radius", &geometry.radius);
	geometry.radius = std::max(geometry.radius, EPSILON);
}
