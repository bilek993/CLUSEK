#include "VehiclePlayerControllerSystem.h"
#include "../Components/VehicleComponent.h"
#include "../Components/VehiclePlayerControllerComponent.h"
#include "PxPhysicsAPI.h"

void VehiclePlayerControllerSystem::Start()
{
}

void VehiclePlayerControllerSystem::Update(const float deltaTime)
{
	float accelerate, brake, left;

	if (InputOutputData->GamePadState.IsConnected())
		HandleGamepad(accelerate, brake, left);
	else
		HandleKeyboard(accelerate, brake, left);

	Registry->view<VehiclePlayerControllerComponent, VehicleComponent>().each([&accelerate, &brake, &left](VehiclePlayerControllerComponent &vehiclePlayerControllerComponent,
		VehicleComponent &vehicleComponent)
	{
		if (vehiclePlayerControllerComponent.Possessed)
		{
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, accelerate);
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, brake);
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, left);
		}
	});
}

void VehiclePlayerControllerSystem::HandleGamepad(float& accelerate, float& brake, float& left) const
{
	accelerate = InputOutputData->GamePadState.triggers.right;
	brake = InputOutputData->GamePadState.triggers.left;
	left = -InputOutputData->GamePadState.thumbSticks.leftX;
}

void VehiclePlayerControllerSystem::HandleKeyboard(float& accelerate, float& brake, float& left) const
{
	accelerate = InputOutputData->KeyboardState.Up ? 1.0f : 0.0f;
	brake = InputOutputData->KeyboardState.Down ? 1.0f : 0.0f;
	left = InputOutputData->KeyboardState.Left ? 1.0f : InputOutputData->KeyboardState.Right ? -1.0f : 0.0f;
}
