#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/RigidbodyStaticHeightFieldsComponent.h"
#include <PxPhysicsAPI.h>

class RigidbodyStaticHeightFieldsComponentEditor final : public BaseComponentEditor<RigidbodyStaticHeightFieldsComponent>
{
protected:
	void Draw() override;

private:
	void DrawGeometryParameters(physx::PxHeightFieldGeometry& geometry) const;
};
