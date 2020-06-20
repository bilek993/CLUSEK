#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RigidbodyDynamicBoxComponent.h"

class RigidbodyDynamicBoxComponentEditor final : public BaseComponentEditor<RigidbodyDynamicBoxComponent>
{
protected:
	void Draw() override;

private:
	void DrawGeometryParameters(physx::PxBoxGeometry& geometry) const;
};
