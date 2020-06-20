#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RigidbodyDynamicCapsuleComponent.h"

class RigidbodyDynamicCapsuleComponentEditor final : public BaseComponentEditor<RigidbodyDynamicCapsuleComponent>
{
protected:
	void Draw() override;

private:
	void DrawGeometryParameters(physx::PxCapsuleGeometry& geometry) const;
};
