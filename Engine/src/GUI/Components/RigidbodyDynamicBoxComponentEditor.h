#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RigidbodyDynamicBoxComponent.h"

class RigidbodyDynamicBoxComponentEditor final : public BaseComponentEditor<RigidbodyDynamicBoxComponent>
{
protected:
	void Draw() override;

private:
	void DrawGeometryParameters(physx::PxBoxGeometry& geometry) const;

	void HandleIncorrectValues(physx::PxBoxGeometry& geometry) const;

	inline static const float EPSILON = 0.001f;
};
