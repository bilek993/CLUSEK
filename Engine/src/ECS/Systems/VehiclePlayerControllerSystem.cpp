#include "VehiclePlayerControllerSystem.h"
#include "PxPhysicsAPI.h"
#include "../../Utils/MathUtil.h"

void VehiclePlayerControllerSystem::Start()
{
}

void VehiclePlayerControllerSystem::Rebuild()
{
}

void VehiclePlayerControllerSystem::Update(const float deltaTime)
{
	float accelerate, brake, left, handbrake;
	bool changeToOrFromReverse, gearUp, gearDown;

	if (InputOutputData->GamePadState.IsConnected())
		HandleGamepad(accelerate, brake, left, handbrake, changeToOrFromReverse, gearUp, gearDown);
	else
		HandleKeyboard(accelerate, brake, left, handbrake, changeToOrFromReverse, gearUp, gearDown);

	Registry->view<VehiclePlayerControllerComponent, VehicleComponent>().each([this, &accelerate, &brake, &handbrake, &deltaTime, &left, &changeToOrFromReverse, &gearUp, &gearDown]
	(VehiclePlayerControllerComponent &vehiclePlayerControllerComponent, VehicleComponent &vehicleComponent)
	{
		if (vehiclePlayerControllerComponent.Possessed)
		{
			const auto vehicleSpeed = CalculateVehicleSpeed(vehicleComponent);

			if (vehicleComponent.Vehicle->mDriveDynData.mUseAutoGears)
			{
				gearUp = false;
				gearDown = false;
			}
			else
			{
				changeToOrFromReverse = false;
			}

			auto wheelAngle = vehicleComponent.Vehicle->mDriveDynData.getAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT);
			wheelAngle = CalculateAndSetWheelAngle(	deltaTime,
													left, 
													vehicleSpeed, 
													wheelAngle, 
													vehiclePlayerControllerComponent);

			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, accelerate);
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, brake);
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, wheelAngle);
			vehicleComponent.Vehicle->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_HANDBRAKE, handbrake);
			vehicleComponent.Vehicle->mDriveDynData.setGearUp(gearUp);
			vehicleComponent.Vehicle->mDriveDynData.setGearDown(gearDown);

			HandleChangingFromOrToReverse(vehicleSpeed, changeToOrFromReverse, vehiclePlayerControllerComponent, vehicleComponent);
		}
	});
}

void VehiclePlayerControllerSystem::HandleGamepad(float& accelerate, float& brake, float& left, float& handbrake, 
	bool& changeToOrFromReverse, bool& gearUp, bool& gearDown) const
{
	accelerate = InputOutputData->GamePadState.triggers.right;
	brake = InputOutputData->GamePadState.triggers.left;
	left = -InputOutputData->GamePadState.thumbSticks.leftX;
	handbrake = InputOutputData->GamePadState.IsAPressed() ? 1.0f : 0.0f;
	changeToOrFromReverse = InputOutputData->GamePadTracker.b == DirectX::GamePad::ButtonStateTracker::ButtonState::PRESSED;
	gearUp = InputOutputData->GamePadTracker.rightShoulder == DirectX::GamePad::ButtonStateTracker::ButtonState::PRESSED;
	gearDown = InputOutputData->GamePadTracker.leftShoulder == DirectX::GamePad::ButtonStateTracker::ButtonState::PRESSED;
}

void VehiclePlayerControllerSystem::HandleKeyboard(float& accelerate, float& brake, float& left, float& handbrake, 
	bool& changeToOrFromReverse, bool& gearUp, bool& gearDown) const
{
	accelerate = InputOutputData->KeyboardState.Up ? 1.0f : 0.0f;
	brake = InputOutputData->KeyboardState.Down ? 1.0f : 0.0f;
	left = InputOutputData->KeyboardState.Left ? 1.0f : InputOutputData->KeyboardState.Right ? -1.0f : 0.0f;
	handbrake = InputOutputData->KeyboardState.Space ? 1.0f : 0.0f;
	changeToOrFromReverse = InputOutputData->KeyboardTracker.pressed.LeftControl || InputOutputData->KeyboardTracker.pressed.RightControl;
	gearUp = InputOutputData->KeyboardTracker.pressed.O;
	gearDown = InputOutputData->KeyboardTracker.pressed.L;
}

float VehiclePlayerControllerSystem::CalculateVehicleSpeed(const VehicleComponent& vehicleComponent) const
{
	return (
		vehicleComponent.Vehicle->mWheelsDynData.getWheelRotationSpeed(0) +
		vehicleComponent.Vehicle->mWheelsDynData.getWheelRotationSpeed(1) +
		vehicleComponent.Vehicle->mWheelsDynData.getWheelRotationSpeed(2) +
		vehicleComponent.Vehicle->mWheelsDynData.getWheelRotationSpeed(3)
		) / 4;
}

float VehiclePlayerControllerSystem::CalculateAndSetWheelAngle(const float deltaTime, const float left, const float vehicleSpeed,
	const float currentWheelAngle, const VehiclePlayerControllerComponent& vehiclePlayerControllerComponent) const
{
	auto wheelAngel = currentWheelAngle + left * deltaTime * vehiclePlayerControllerComponent.SteeringSpeed * 
		(1 - MathUtil::Remap(std::clamp(vehicleSpeed, 0.0f, vehiclePlayerControllerComponent.SteeringLimiterTopSpeed), 0.0f, vehiclePlayerControllerComponent.SteeringLimiterTopSpeed, 0.0f, vehiclePlayerControllerComponent.SteeringLimiterFactor));
	
	if (MathUtil::IsAlmostZero(left, 0.05f))
		wheelAngel -= (wheelAngel * deltaTime * vehicleSpeed) * vehiclePlayerControllerComponent.WheelReturningToNeutralPosition;
	
	return std::clamp(wheelAngel, -1.0f, 1.0f);
}

void VehiclePlayerControllerSystem::HandleChangingFromOrToReverse(const float vehicleSpeed, const bool changeToOrFromReverse,
	VehiclePlayerControllerComponent& vehiclePlayerControllerComponent, VehicleComponent &vehicleComponent) const
{
	if (!changeToOrFromReverse)
		return;

	if (std::abs(vehicleSpeed) < vehiclePlayerControllerComponent.MinimalSpeedForGearChangeFromOrToReverse)
	{
		vehiclePlayerControllerComponent.Reverse = !vehiclePlayerControllerComponent.Reverse;

		if (vehiclePlayerControllerComponent.Reverse)
		{
			vehicleComponent.Vehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eREVERSE);
			Logger::Debug("Changed gear to reverse.");
		}
		else
		{
			vehicleComponent.Vehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
			Logger::Debug("Changed gear to first.");
		}
	}
	else
	{
		Logger::Debug("Stop vehicle before changing gear!");
	}
}
