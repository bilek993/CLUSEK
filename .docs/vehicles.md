## Vehicles

This game engine supports advanced vehicles simulation, thanks to the physics library. At this moment only 4 wheels vehicles are supported. If other vehicles will be supported, this documentation will be updated. To make one new vehicle, a map designer has to place 4 entities on map: one for core vehicle and four for wheels.

### Wheel Component

This component is required for wheels. It has to be assigned to the wheels with other required components to make wheels works. Parameters that can be changed or set for this component:

* **VehicleId** (_required_, _std::string_)<br/>
Vehicle identificator, that is used to associate this wheel with the vehicle. It has to identical to the same parameter in the vehicle component.

* **Flip** (_required_, _bool_)<br/>
Visually rotates wheel around Y axis by Pi constant. Usually should be set to true for wheel with id 1 and 3.

* **WheelId** (_required_, _int_)<br/>
A numeric value, that is used to connect a wheel to the correct position in a vehicle. This value is required to be in range 0-4. This game engine requires this value to be: 0 for front left, 1 for front right, 2 for rear left and 3 for rear right wheel.

* **Mass** (_required_, _float_)<br/>
Wheel rigidbody mass in kilograms.

* **Width** (_required_, _float_)<br/>
Width of wheel.

* **Radius** (_required_, _float_)<br/>
Radius of wheel, which is half of diameter.

* **TireType** (_optional_, _int_)<br/>
Represents tire condition, where 0 represents normal (almost unused) tire and 1 represents worn tire.

* **SuspensionMaxCompression** (_optional_, _float_)<br/>
Maximum allowed compression by suspension spring.

* **SuspensionMaxDroop** (_optional_, _float_)<br/>
Maximum allowed elongation by suspension spring.

* **SuspensionSpringStrength** (_required_, _float_)<br/>
Strength of spring in suspension in kilograms per second-squared.

* **SuspensionSpringDamperRate** (_required_, _float_)<br/>
Spring damper rate in kilograms per second.

* **CamberAngleAtRest** (_optional_, _float_)<br/>
Angel between the vertical axis of the wheel and the vertical axis of the wheel, when viewed from the front/rear of the vehicle when the vehicle is in a rest position. Rest position means, that the vehicle should be moving perfectly forward when it would be in motion. This angel is represented in radians unit. For 99.9% type of vehicles it should be left to default value.

* **CamberAngleAtMaxDroop** (_optional_, _float_)<br/>
Angel between the vertical axis of the wheel and the vertical axis of the wheel, when viewed from the front/rear of the vehicle when the suspension is at maximum droop. This angel is represented in radians unit.

* **CamberAngleAtMaxCompression** (_optional_, _float_)<br/>
Angel between the vertical axis of the wheel and the vertical axis of the wheel, when viewed from the front/rear of the vehicle when the suspension is at maximum droop. This angel is represented in radians unit.

* **SuspensionForceApplicationPoinOffset** (_optional_, _float_)<br/>
Suspension force application offset on Y axis.

* **TireForceApplicationPoinOffset** (_optional_, _float_)<br/>
Tire force application offset on Y axis.

* **LatStiffXMultiplier** (_optional_, _float_)<br/>
Describes tire lateral stiffness minimum normalized load for X axis.

* **LatStiffYMultiplier** (_optional_, _float_)<br/>
Describes tire lateral stiffness minimum normalized load for Y axis.



### Vehicle Component

This component is required for core vehicle. It has to be assigned to the vehicle core (chase) with other required components to make vehicle work. Parameters that can be changed or set for this component:

* **VehicleId** (_required_, _std::string_)<br/>
A unique value for a vehicle. Each vehicle needs to have a different value in this parameter field.

* **Mass** (_required_, _float_)<br/>
Vehicle rigidbody mass in kilograms.

* **ConvexModelId** (_optional_, _std::string_)<br/>
Path to convex mesh collider. If id is not provided, then the convex mesh is generated based on dimensions parameters.

* **ConvexPositionOffsetVectorX** (_optional_, _float_)<br/>
Convex mesh position offest on X axis.

* **ConvexPositionOffsetVectorY** (_optional_, _float_)<br/>
Convex mesh position offest on Y axis.

* **ConvexPositionOffsetVectorZ** (_optional_, _float_)<br/>
Convex mesh position offest on Z axis.

* **ConvexRotationOffsetVectorX** (_optional_, _float_)<br/>
Convex mesh offest roll.

* **ConvexRotationOffsetVectorY** (_optional_, _float_)<br/>
Convex mesh offest pitch.

* **ConvexRotationOffsetVectorZ** (_optional_, _float_)<br/>
Convex mesh offest yaw.

* **DimensionX** (_required_, _float_)<br/>
Vehicle chassis size on axis X.

* **DimensionY** (_required_, _float_)<br/>
Vehicle chassis size on axis Y.

* **DimensionZ** (_required_, _float_)<br/>
Vehicle chassis size on axis Z.

* **CenterOfMassOffsetX** (_optional_, _float_)<br/>
Center of mass offset from default vehicle position on axis X.

* **CenterOfMassOffsetY** (_optional_, _float_)<br/>
Center of mass offset from default vehicle position on axis Y.

* **CenterOfMassOffsetZ** (_optional_, _float_)<br/>
Center of mass offset from default vehicle position on axis Z.

* **MaxBrakeTorque** (_required_, _float_)<br/>
Maximum brake torque that can be applied to all wheels. This value is specified in kilograms metres-squared per second-squared.

* **MaxHandBrakeTorque** (_required_, _float_)<br/>
Maximum torque that can be applied to rear wheels, when using handbrake. This value is specified in kilograms metres-squared per second-squared.

* **MaxSteer** (_optional_, _float_)<br/>
Maximum steering angle, that can be achieved by front wheels in radians.

* **AntiRollBarStiffness** (_optional_, _float_)<br/>
Stiffness of anti-roll bar in kilograms per second-squared. This value is shared by front and rear anti-roll bar.

* **DifferentialType** (_optional_, _std::string_)<br/>
Differential type identificator. Supported differential types values: LIMITED_SLIP_4W, LIMITED_SLIP_FRONT, LIMITED_SLIP_REAR, OPEN_4W, OPEN_FRONT, OPEN_REAR.

* **PeakTorque** (_optional_, _float_)<br/>
Maximum available torque in kilograms metres-squared per second-squared.

* **MaxOmega** (_optional_, _float_)<br/>
Maximum rotation speed of the engine in radians per second.

* **EngineMomentOfInertia** (_optional_, _float_)<br/>
Moment of inertia of the engine around the axis of rotation.

* **GearsSwitchTime** (_optional_, _float_)<br/>
Time to switch gear in seconds.

* **GearboxType** (_optional_, _std::string_)<br/>
Type of used vehicle. Supported gearbox types: AUTOMATIC, MANUAL.

* **ClutchAccuracyMode** (_optional_, _std::string_)<br/>
Accuracy of clutch, when calculating rotations. Accuracy of clutch can be set to BEST_POSSIBLE or ESTIMATE, but second mode should be used only, if first mode doesn't deliver enought performace.

* **ClutchStrength** (_optional_, _float_)<br/>
Strength of clutch in kilograms metres-squared per second.

* **AckermannAccuracy** (_optional_, _float_)<br/>
Accuracy of steer calculation, where 0.0 is no Ackermann steer-correction and 1.0 is equal to perfect Ackermann steer-correction.