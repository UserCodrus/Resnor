// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "PhysicsInterface.h"

#include "CoreMinimal.h"
#include "PhysicalStaticMesh.h"
#include "PhysicsObject.generated.h"

class UPhysicsMovementComponent;

// A static mesh object with physics enabled
UCLASS()
class CYBERSHOOTER_API APhysicsObject : public APhysicalStaticMesh, public IPhysicsInterface
{
	GENERATED_BODY()
	
public:
	APhysicsObject();

	virtual void BeginPlay() override;

	/// IPhysicsInterface ///
	
	void AddImpulse(FVector Force) override;
	void AddRelativeImpulse(FVector Force) override;
	void AddStaticForce(FVector Force) override;
	void RemoveStaticForce(FVector Force) override;
	void ResetStaticForce() override;
	void SetGravityEnabled(bool Enable) override;
	void SetGravity(float NewGravity) override;
	void ResetGravity() override;
	void SetAirFriction(float NewFriction) override;
	void ResetAirFriction() override;
	void SetFriction(float NewFriction) override;
	void ResetFriction() override;
	void SetTickRate(float NewRate) override;

protected:
	/// Properties ///
	
	// The movement component that handles physics
	UPROPERTY(Category = "Components", VisibleAnywhere)
		UPhysicsMovementComponent* MovementComponent;
};
