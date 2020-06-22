#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RigidbodyStaticCapsuleComponent.h"

class RigidbodyStaticCapsuleComponentEditor final : public BaseComponentEditor<RigidbodyStaticCapsuleComponent>
{
protected:
	void Draw() override;

private:
	void DrawGeometryParameters(physx::PxCapsuleGeometry& geometry) const;
};
