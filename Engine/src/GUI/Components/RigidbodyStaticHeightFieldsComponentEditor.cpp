#include "RigidbodyStaticHeightFieldsComponentEditor.h"

void RigidbodyStaticHeightFieldsComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	physx::PxHeightFieldGeometry geometry;
	component->Shape->getHeightFieldGeometry(geometry);

	DrawGeometryParameters(geometry);
	HandleIncorrectValues(geometry);

	component->Shape->setGeometry(geometry);
}

void RigidbodyStaticHeightFieldsComponentEditor::DrawGeometryParameters(physx::PxHeightFieldGeometry& geometry) const
{
	ImGui::InputFloat("Column scale", &geometry.columnScale);
	ImGui::InputFloat("Height", &geometry.heightScale);
	ImGui::InputFloat("Row scale", &geometry.rowScale);
}

void RigidbodyStaticHeightFieldsComponentEditor::HandleIncorrectValues(physx::PxHeightFieldGeometry& geometry) const
{
	geometry.columnScale = std::max(geometry.columnScale, EPSILON);
	geometry.heightScale = std::max(geometry.heightScale, EPSILON);
	geometry.columnScale = std::max(geometry.columnScale, EPSILON);
}
