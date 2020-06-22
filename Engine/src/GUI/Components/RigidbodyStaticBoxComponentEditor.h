#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RigidbodyStaticBoxComponent.h"

class RigidbodyStaticBoxComponentEditor final : public BaseComponentEditor<RigidbodyStaticBoxComponent>
{
protected:
	void Draw() override;

private:
	void DrawGeometryParameters(physx::PxBoxGeometry& geometry) const;
};
