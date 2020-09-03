#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RigidbodyStaticSphereComponent.h"

class RigidbodyStaticSphereComponentEditor final : public BaseComponentEditor<RigidbodyStaticSphereComponent>
{
protected:
	void Draw() override;

private:
	void DrawGeometryParameters(physx::PxSphereGeometry& geometry) const;

	void HandleIncorrectValues(physx::PxSphereGeometry& geometry) const;

	inline static const float EPSILON = 0.001f;
};
