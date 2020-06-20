#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RigidbodyDynamicSphereComponent.h"

class RigidbodyDynamicSphereComponentEditor final : public BaseComponentEditor<RigidbodyDynamicSphereComponent>
{
protected:
	void Draw() override;

private:
	void DrawGeometryParameters(physx::PxSphereGeometry& geometry) const;
};
