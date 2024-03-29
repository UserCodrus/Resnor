// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PhysicsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPhysicsInterface : public UInterface
{
	GENERATED_BODY()
};

// An interface for actors that can interact with the physics system
class CYBERSHOOTER_API IPhysicsInterface
{
	GENERATED_BODY()

public:
	// Get the total velocity of the object
	UFUNCTION()
		virtual FVector GetVelocity() = 0;
	// Change the object's velocity
	UFUNCTION()
		virtual void ChangeVelocity(FVector NewVelocity) = 0;
	// Returns false if the object is unable to move
	UFUNCTION()
		virtual bool CanMove() = 0;

	// Add a velocity impulse to the object
	UFUNCTION()
		virtual void AddImpulse(FVector Force) = 0;
	// Add a velocity impulse relative to the object's frame of reference
	UFUNCTION()
		virtual void AddRelativeImpulse(FVector Force) = 0;
	// Add a static force to the object
	UFUNCTION()
		virtual void AddStaticForce(FVector Force) = 0;
	// Remove a static force to the object
	UFUNCTION()
		virtual void RemoveStaticForce(FVector Force) = 0;
	// Remove all static forces being applied to an object
	UFUNCTION()
		virtual void ResetStaticForce() = 0;

	// Change the gravity applied to an object
	UFUNCTION()
		virtual void SetGravity(float NewGravity) = 0;
	// Reset gravity to the world's default
	UFUNCTION()
		virtual void ResetGravity() = 0;
	// Change air friction
	UFUNCTION()
		virtual void SetAirFriction(float NewFriction) = 0;
	// Reset air friction to the world default
	UFUNCTION()
		virtual void ResetAirFriction() = 0;
	// Set global friction
	UFUNCTION()
		virtual void SetFriction(float NewFriction) = 0;
	// Reset global friction to world default
	UFUNCTION()
		virtual void ResetFriction() = 0;

	// Multiply the mass of the object
	UFUNCTION()
		virtual void SetMass(float Multiplier) = 0;
	// Reset the object's mass
	UFUNCTION()
		virtual void ResetMass() = 0;

	// Change the multiplier for ticks
	UFUNCTION()
		virtual void SetTickRate(float NewRate) = 0;

	// Get the mass of the object
	UFUNCTION()
		virtual float GetMass() const = 0;
	// Calculate the weight of the object
	UFUNCTION()
		virtual float GetWeight() const = 0;
};
