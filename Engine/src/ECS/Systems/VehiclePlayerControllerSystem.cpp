#include "VehiclePlayerControllerSystem.h"
#include "../Components/VehicleComponent.h"
#include "../Components/VehiclePlayerControllerComponent.h"
#include "PxPhysicsAPI.h"

void VehiclePlayerControllerSystem::Start()
{
}

void VehiclePlayerControllerSystem::Update(const float deltaTime)
{
	float accelerate, brake, left, handbrake;

	if (InputOutputData->GamePadState.IsConnected())
		HandleGamepad(accelerate, brake, left, handbrake);
	else
		HandleKeyboard(accelerate, brake, left, handbrake);

	Registry->view<VehiclePlayerControllerComponent, VehicleComponent>().each([this, &accelerate, &brake, &handbrake, &deltaTime, &left](VehiclePlayerControllerComponent &vehiclePlayerControllerComponent,
		VehicleComponent &vehicleComponent)
	{
		if (vehiclePlayerControllerComponent.Possessed)
		{
			const auto vehicleSpeed = (
				vehicleComponent.Vehicle->mWheelsDynData.getWheelRotationSpeed(0) + 
				vehicleComponent.Vehicle->mWheelsDynData.getWheelRotationSpeed(1) +
				vehicleComponent.Vehicle->mWheelsDynData.getWheelRotationSpeed(2) + 
				vehicleComponent.Vehicle->mWheelsDynData.getWheelRotationSpeed(3)
				) / 4;

			WheelAngel = std::clamp(WheelAngel + (left * deltaTime * vehiclePlayerControllerComponent.SteeringSpeed), -1.0f, 1.0f);
			WheelAngel -= std::clamp((WheelAngel * deltaTime * vehicleSpeed) * vehiclePlayerControllerComponent.WheelReturningToNeutralPosition, -1.0f, 1.0f);

			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, accelerate);
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, brake);
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, WheelAngel);
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_HANDBRAKE, handbrake);
		}
	});
}

void VehiclePlayerControllerSystem::HandleGamepad(float& accelerate, float& brake, float& left, float& handbrake) const
{
	accelerate = InputOutputData->GamePadState.triggers.right;
	brake = InputOutputData->GamePadState.triggers.left;
	left = -InputOutputData->GamePadState.thumbSticks.leftX;
	handbrake = InputOutputData->GamePadState.IsAPressed() ? 1.0f : 0.0f;
}

void VehiclePlayerControllerSystem::HandleKeyboard(float& accelerate, float& brake, float& left, float& handbrake) const
{
	accelerate = InputOutputData->KeyboardState.Up ? 1.0f : 0.0f;
	brake = InputOutputData->KeyboardState.Down ? 1.0f : 0.0f;
	left = InputOutputData->KeyboardState.Left ? 1.0f : InputOutputData->KeyboardState.Right ? -1.0f : 0.0f;
	handbrake = InputOutputData->KeyboardState.Space ? 1.0f : 0.0f;
}
