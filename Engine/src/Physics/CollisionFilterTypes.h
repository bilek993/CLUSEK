#pragma once

enum CollisionFilterTypes
{
	CollisionVehicleObstacle = 1 << 0,
	CollisionWheel = 1 << 1,

	CollisionVehicleObstacleAgainst = CollisionVehicleObstacle,
	CollisionWheelAgainst = CollisionWheel
};