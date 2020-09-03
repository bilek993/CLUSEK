#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RigidbodyDynamicSphereComponent.h"

class RigidbodyDynamicSphereComponentEditor final : public BaseComponentEditor<RigidbodyDynamicSphereComponent>
{
protected:
	void Draw() override;

private:
	void DrawGeometryParameters(physx::PxSphereGeometry& geometry) const;

	void HandleIncorrectValues(physx::PxSphereGeometry& geometry) const;

	inline static const float EPSILON = 0.001f;
};
