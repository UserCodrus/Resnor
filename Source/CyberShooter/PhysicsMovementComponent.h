// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "PhysicsMovementComponent.generated.h"

class UCapsuleComponent;
class IPhysicsInterface;

// The results of a floor collision test
USTRUCT(BlueprintType)
struct CYBERSHOOTER_API FFloor
{
	GENERATED_BODY()

	// Set to true if the floor is walkable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool Walkable;
	// Set to true if the platform below is a static object
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool Stable;
	// The distance to the floor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Distance;
	// The surface friction of the floor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Friction;
	// The damage the floor deals to pawns standing on it
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Damage;
	// The velocity of the object the player is standing on
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector Velocity;
	// The result of the floor test
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FHitResult HitResult;

	FFloor() : Walkable(false), Stable(false), Distance(0.0f), Friction(1.0f), Damage(0.0f), Velocity(0.0f, 0.0f, 0.0f), HitResult(1.0f) {};
};

// The main movement component for physics enabled pawns and objects
UCLASS(ClassGroup = Movement, BlueprintType, meta = (BlueprintSpawnableComponent))
class CYBERSHOOTER_API UPhysicsMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
	
public:
	UPhysicsMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// Physics ///

	// Set the total world space velocity of the object, accounting for moving platforms and static force
	void SetVelocity(FVector NewVelocity);
	// Impact with a physics object
	void Impact(IPhysicsInterface* OtherObject, FVector ImpactNormal);
	// Teleport the object to a new location
	void Teleport(FVector Location);
	
	// Change the speed multiplier to a new value
	void SetSpeed(float NewSpeed);

	// Add a movement input to the controller
	void AddControlInput(FVector Input);
	// Set the static force applied to the pawn
	void SetStaticForce(FVector Force);
	// Convert static force to an impulse
	void ResetStaticForce();
	// Add a velocity impulse in world space to the object
	void AddImpulse(FVector Impulse);
	// Cancel the effects of gravity
	void StopGravity();
	// Set velocity to zero
	void StopMovement();

	// Change the tick speed of the component
	void SetTickSpeed(float NewSpeed);

	// Get the total velocity of the component in world space, counting base velocity and external forces
	FVector GetTotalVelocity() const;
	// Returns true if there is no floor under the pawn
	inline bool IsFalling() const;
	// Returns true if the player is not on a moving platform
	inline bool IsStable() const;
	// Returns the amount of damage the floor the pawn is standing on will deal
	inline float GetFloorDamage() const;

	// Wake the object
	void Wake();
	// Put the object to sleep
	void Sleep();
	// Enable physics
	void Enable();
	// Disable physics
	void Disable();

	// Returns true if physics are enabled
	FORCEINLINE bool IsEnabled() const { return !Disabled; }
	// Returns true if the object is teleporting
	FORCEINLINE bool IsTeleporting() const { return TeleportReady; }

protected:
	/// Movement ///

	// Move the attached component
	void MoveComponent(FVector Delta, float DeltaTime);

	// Apply deceleration to the component's velocity
	void ApplyDeceleration(float DeltaTime);
	// Apply impulses to the component's velocity
	void ApplyImpulseToVelocity();
	// Apply gravity to the component's velocity
	void ApplyGravityToVelocity(float DeltaTime);

	// Find the floor below the pawn
	bool FindFloor(FFloor& Result);

	/// Properties ///

public:
	// Calculate the maximum lateral speed
	inline float GetMaxSpeed() const override;
	// Calculate lateral acceleration
	inline float GetAcceleration() const;
	// Calculate lateral deceleration
	inline float GetDeceleration() const;
	// Calculate turning rate
	inline float GetTurnRate() const;
	// Calculate surface friction
	inline float GetFriction() const;
	// Calculate gravity strength
	inline float GetGravity() const;
	// Calculate static force
	inline FVector GetStaticForce() const;
	// Calculate the force behind the object's movement
	inline FVector GetForce() const;
	// Calculate terminal velocity
	inline float GetTerminalVelocity() const;

	// Set to true when physics should be ignored
	UPROPERTY(EditAnywhere)
		bool Disabled;

	// The height of the object
	UPROPERTY(Category = "Physics|Collision", EditAnywhere)
		float Height;

	// The mass of the object being controlled
	UPROPERTY(Category = "Physics|World", EditAnywhere)
		float Mass;
	// The base strength of gravity
	UPROPERTY(Category = "Physics|World", EditAnywhere)
		float Gravity;
	// The maximum acceleration of gravity
	UPROPERTY(Category = "Physics|World", EditAnywhere)
		float TerminalVelocity;
	// The multiplier for friction on all surfaces
	UPROPERTY(Category = "Physics|World", EditAnywhere)
		float Friction;
	// Friction while in the air
	UPROPERTY(Category = "Physics|World", EditAnywhere)
		float AirFriction;
	// The default surface friction
	UPROPERTY(Category = "Physics|World", EditAnywhere)
		float SurfaceFriction;

protected:
	// The floor below the pawn
	UPROPERTY(EditAnywhere)
		FFloor CurrentFloor;

	// The acceleration applied by inputs
	UPROPERTY(Category = "Movement", EditAnywhere)
		float Acceleration;
	// The deceleration applied when no input is available
	UPROPERTY(Category = "Movement", EditAnywhere)
		float Deceleration;
	// A boost in acceleration when changing direction to make turns more responsive
	UPROPERTY(Category = "Movement", EditAnywhere)
		float TurningRate;

	// The maximum lateral movement speed of the pawn
	UPROPERTY(Category = "Movement", EditAnywhere)
		float MaxSpeed;
	// A boost to acceleration, deceleration and movement speed based on the player's stats
	UPROPERTY(Category = "Movement", EditAnywhere)
		float SpeedMultiplier;

	// The maximum distance the pawn can be from the floor before gravity shuts off
	UPROPERTY(Category = "Movement", EditAnywhere)
		float MaxFloorDistance;
	// The minimum distance the pawn should be from the floor when resting, must be less than MaxFloorDistance
	UPROPERTY(Category = "Movement", EditAnywhere)
		float MinFloorDistance;

	// The tick speed of the component
	UPROPERTY(Category = "Time", EditAnywhere)
		float TickSpeed;

	// If set to true momentum will be preserved through orientation changes, otherwise the direction of movement will change relative to orientation
	UPROPERTY(Category = "Orientation", EditAnywhere)
		bool KeepMomentum;
	
	// Static force applied to the pawn
	UPROPERTY(Category = "Physics|Force", EditAnywhere)
		FVector StaticForce;

	// Pending inputs for this component
	UPROPERTY()
		FVector PendingInput;
	// Any velocity impulses that need to be added during the next input cycle
	UPROPERTY()
		FVector PendingImpulse;
	// The pending teleport location
	UPROPERTY()
		FVector PendingTeleport;

	// Set to true to notify the movement component that it should teleport during the next movement frame
	bool TeleportReady;
};

// The main movement component for player controlled pawns
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent))
class CYBERSHOOTER_API UPlayerMovementComponent : public UPhysicsMovementComponent
{
	GENERATED_BODY()

public:
	UPlayerMovementComponent();
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	/// Movement ///
	
	// Apply controller input to the component
	void ApplyControlInputToVelocity(float DeltaTime);
};

// A simple enemy movement component for seekers
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent))
class CYBERSHOOTER_API UEnemyMovementComponent : public UPlayerMovementComponent
{
	GENERATED_BODY()

public:
	UEnemyMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};