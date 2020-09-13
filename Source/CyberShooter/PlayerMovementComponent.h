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
	// The velocity of the object the player is standing on
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector Velocity;
	// The result of the floor test
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FHitResult HitResult;

	FFloor() : Walkable(false), Distance(0.0f), Friction(1.0f), Velocity(0.0f, 0.0f, 0.0f), HitResult(1.0f) {};
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
	virtual void SetSpeed(float NewSpeed);
	// Change the gravity multiplier to a new value
	virtual void SetGravity(float NewGravity);
	// Enable or disable gravity
	virtual void EnableGravity(bool Enable);
	// Set air friction
	virtual void SetAirFriction(float NewFriction);
	// Set global friction multiplier
	virtual void SetFriction(float NewFriction);
	// Change the shape of the floor collision
	virtual void SetCollision(UCapsuleComponent* NewCollision);
	// Change the orientation of movement
	virtual void SetOrientation(FVector NewForward, FVector NewUp);
	// Add a velocity impulse in world space to the object
	virtual void AddImpulse(FVector Impulse);
	// Cancel the effects of gravity
	virtual void StopGravity();

	// Returns true if there is no floor under the pawn
	bool IsFalling() const;

protected:
	// Apply controller input to the component
	virtual void ApplyControlInputToVelocity(float DeltaTime);
	// Apply gravity to the component
	virtual void ApplyGravityToVelocity(float DeltaTime);
	// Find the floor below the pawn
	virtual bool FindFloor(FFloor& Result);

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
	UPROPERTY(Category = Movement, EditAnywhere)
		float MaxSpeed;
	// The acceleration applied by inputs
	UPROPERTY(Category = Movement, EditAnywhere)
		float Acceleration;
	// The deceleration applied when no input is available
	UPROPERTY(Category = Movement, EditAnywhere)
		float Deceleration;

	// A boost in acceleration when changing direction to make turns more responsive
	UPROPERTY(Category = Movement, EditAnywhere)
		float TurningRate;
	// A boost to acceleration, deceleration and movement speed based on the player's stats
	UPROPERTY(Category = Movement, EditAnywhere)
		float SpeedMultiplier;
	// The maximum distance the pawn will be from the floor before gravity shuts off
	UPROPERTY(Category = Movement, EditAnywhere)
		float MaxFloorDistance;

	// The forward vector for the pawn
	UPROPERTY(Category = Orientation, EditAnywhere)
		FVector Forward;
	// The up vector for the pawn
	UPROPERTY(Category = Orientation, EditAnywhere)
		FVector Up;
	// If set to true momentum will be preserved through orientation changes, otherwise the direction of movement will change relative to orientation
	UPROPERTY(Category = Orientation, EditAnywhere)
		bool KeepMomentum;
	
	// The collision shape used to detect the floor
	UPROPERTY(Category = Physics, EditAnywhere)
		UCapsuleComponent* Collision;
	// The floor below the pawn
	UPROPERTY(Category = Physics, EditAnywhere)
		FFloor CurrentFloor;

	// The multiplier for friction on all surfaces
	UPROPERTY(Category = Physics, EditAnywhere)
		float Friction;
	// Friction while in the air
	UPROPERTY(Category = Physics, EditAnywhere)
		float AirFriction;
	// The default surface friction
	UPROPERTY(Category = Physics, EditAnywhere)
		float DefaultSurfaceFriction;

	// The base strength of gravity
	UPROPERTY(Category = Physics, EditAnywhere)
		float Gravity;
	// If set to false, gravity calculations will be skipped
	UPROPERTY(Category = Physics, EditAnywhere)
		bool GravityEnabled;
	// The maximum acceleration of gravity
	UPROPERTY(Category = Physics, EditAnywhere)
		float TerminalVelocity;

	// Any velocity impulses that need to be added during the next input cycle
	UPROPERTY()
		FVector PendingImpulse;

	// Set to true to notify the pawn that it has been teleported
	bool Teleported;
};
