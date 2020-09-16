// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PlayerMovementComponent.generated.h"

class UCapsuleComponent;

// The results of a floor collision test
USTRUCT(BlueprintType)
struct CYBERSHOOTER_API FFloor
{
	GENERATED_BODY()

	// Set to true if the floor is walkable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool Walkable;
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

	FFloor() : Walkable(false), Distance(0.0f), Friction(1.0f), Damage(0.0f), Velocity(0.0f, 0.0f, 0.0f), HitResult(1.0f) {};
};

// The main movement component for player controlled pawns
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent))
class CYBERSHOOTER_API UPlayerMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	UPlayerMovementComponent();

	/// UPawnMovementComponent ///

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnTeleported() override;

	// Change the speed multiplier to a new value
	void SetSpeed(float NewSpeed);
	// Change the gravity multiplier to a new value
	void SetGravity(float NewGravity);
	// Enable or disable gravity
	void EnableGravity(bool Enable);
	// Set air friction
	void SetAirFriction(float NewFriction);
	// Set global friction multiplier
	void SetFriction(float NewFriction);

	// Change the shape of the floor collision
	void SetCollision(UCapsuleComponent* NewCollision);
	// Change the orientation of movement
	void SetOrientation(FVector NewForward, FVector NewUp);

	// Set the static force applied to the pawn
	void SetStaticForce(FVector Force);
	// Convert static force to an impulse
	void ResetStaticForce();
	// Add a velocity impulse in world space to the object
	void AddImpulse(FVector Impulse);
	// Cancel the effects of gravity
	void StopGravity();

	// Change the tick speed of the component
	void SetTickSpeed(float NewSpeed);

	// Returns true if there is no floor under the pawn
	inline bool IsFalling() const;
	// Returns the amount of damage the floor the pawn is standing on will deal
	inline float GetFloorDamage() const;

protected:
	// Apply controller input to the component
	void ApplyControlInputToVelocity(float DeltaTime);
	// Apply gravity to the component
	void ApplyGravityToVelocity(float DeltaTime);
	// Find the floor below the pawn
	bool FindFloor(FFloor& Result);

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
	// Calculate terminal velocity
	inline float GetTerminalVelocity() const;

	// The maximum lateral movement speed of the pawn
	UPROPERTY(Category = "Movement", EditAnywhere)
		float MaxSpeed;
	// The acceleration applied by inputs
	UPROPERTY(Category = "Movement", EditAnywhere)
		float Acceleration;
	// The deceleration applied when no input is available
	UPROPERTY(Category = "Movement", EditAnywhere)
		float Deceleration;

	// A boost in acceleration when changing direction to make turns more responsive
	UPROPERTY(Category = "Movement", EditAnywhere)
		float TurningRate;
	// A boost to acceleration, deceleration and movement speed based on the player's stats
	UPROPERTY(Category = "Movement", EditAnywhere)
		float SpeedMultiplier;
	// The maximum distance the pawn will be from the floor before gravity shuts off
	UPROPERTY(Category = "Movement", EditAnywhere)
		float MaxFloorDistance;

	// The tick speed of the component
	UPROPERTY(Category = "Time", EditAnywhere)
		float TickSpeed;

	// The forward vector for the pawn
	UPROPERTY(Category = "Orientation", EditAnywhere)
		FVector Forward;
	// The up vector for the pawn
	UPROPERTY(Category = "Orientation", EditAnywhere)
		FVector Up;
	// If set to true momentum will be preserved through orientation changes, otherwise the direction of movement will change relative to orientation
	UPROPERTY(Category = "Orientation", EditAnywhere)
		bool KeepMomentum;
	
	// The collision shape used to detect the floor
	UPROPERTY(Category = "Physics|Collision", EditAnywhere)
		UCapsuleComponent* Collision;
	// The floor below the pawn
	UPROPERTY(Category = "Physics|Collision", EditAnywhere)
		FFloor CurrentFloor;

	// Static force applied to the pawn
	UPROPERTY(Category = "Physics|Force", EditAnywhere)
		FVector StaticForce;

	// The multiplier for friction on all surfaces
	UPROPERTY(Category = "Physics|Friction", EditAnywhere)
		float Friction;
	// Friction while in the air
	UPROPERTY(Category = "Physics|Friction", EditAnywhere)
		float AirFriction;
	// The default surface friction
	UPROPERTY(Category = "Physics|Friction", EditAnywhere)
		float DefaultSurfaceFriction;

	// The base strength of gravity
	UPROPERTY(Category = "Physics|Gravity", EditAnywhere)
		float Gravity;
	// If set to false, gravity calculations will be skipped
	UPROPERTY(Category = "Physics|Gravity", EditAnywhere)
		bool GravityEnabled;
	// The maximum acceleration of gravity
	UPROPERTY(Category = "Physics|Gravity", EditAnywhere)
		float TerminalVelocity;

	// Any velocity impulses that need to be added during the next input cycle
	UPROPERTY()
		FVector PendingImpulse;

	// Set to true to notify the pawn that it has been teleported
	bool Teleported;
};
