#pragma once
#include "BaseSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/ModelRenderComponent.h"
#include "../../Renderer/ModelViewLogic.h"
#include "../Components/SkyboxComponent.h"


class TransformSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
private:
	void RecalculateAll() const;

	template<class T>
	void Recalculate() const;
};

template <class T>
void TransformSystem::Recalculate() const
{
	Registry->view<TransformComponent, T>().each([](TransformComponent &transformComponent, T &secondComponent)
	{
		if (transformComponent.ValuesChanged)
		{
			transformComponent.ValuesChanged = false;
			ModelViewLogic::UpdateModelMatrix(secondComponent.WorldMatrix, transformComponent);
		}
	});
}
