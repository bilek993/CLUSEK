#include "VehiclePlayerControllerSystem.h"
#include "../Components/VehicleComponent.h"
#include "../Components/VehiclePlayerControllerComponent.h"
#include "PxPhysicsAPI.h"

void VehiclePlayerControllerSystem::Start()
{
}

void VehiclePlayerControllerSystem::Update(const float deltaTime)
{
	float accelerate, brake;

	if (InputOutputData->GamePadState.IsConnected())
		HandleGamepad(accelerate, brake);
	else
		HandleKeyboard(accelerate, brake);

	Registry->view<VehiclePlayerControllerComponent, VehicleComponent>().each([&accelerate, &brake](VehiclePlayerControllerComponent &vehiclePlayerControllerComponent,
		VehicleComponent &vehicleComponent)
	{
		if (vehiclePlayerControllerComponent.Possessed)
		{
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, accelerate);
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, brake);
		}
	});
}

void VehiclePlayerControllerSystem::HandleGamepad(float& accelerate, float& brake) const
{
	accelerate = InputOutputData->GamePadState.triggers.right;
	brake = InputOutputData->GamePadState.triggers.left;
}

void VehiclePlayerControllerSystem::HandleKeyboard(float& accelerate, float& brake) const
{
	accelerate = InputOutputData->KeyboardState.Up ? 1.0f : 0.0f;
	brake = InputOutputData->KeyboardState.Down ? 1.0f : 0.0f;
}
