#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RigidbodyStaticSphereComponent.h"

class RigidbodyStaticSphereComponentEditor final : public BaseComponentEditor<RigidbodyStaticSphereComponent>
{
protected:
	void Draw() override;

private:
	void DrawGeometryParameters(physx::PxSphereGeometry& geometry) const;
};
