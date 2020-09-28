// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "PhysicsInterface.h"
#include "OrientationInterface.h"

#include "CoreMinimal.h"
#include "PhysicalStaticMesh.h"
#include "PhysicsObject.generated.h"

class UPhysicsMovementComponent;

// A static mesh object with physics enabled
UCLASS()
class CYBERSHOOTER_API APhysicsObject : public APhysicalStaticMesh, public IPhysicsInterface, public IOrientationInterface
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

	/// IOrientationInterface ///

	bool SetOrientation(FVector NewForward, FVector NewUp) override;
	bool RevertOrientation() override;
	bool CheckOrientation(FVector DesiredUp) override;

protected:
	/// Properties ///
	
	// If set to true, the object will be able to change orientation
	UPROPERTY(Category = "Orientation", EditAnywhere)
		bool CanChangeOrientation;
	// The forward vector the object possessed before the last orientation change
	UPROPERTY(Category = "Orientation", EditAnywhere)
		FVector PreviousForward;
	// The up vector the object possessed before the last orientation change
	UPROPERTY(Category = "Orientation", EditAnywhere)
		FVector PreviousUp;

	// The movement component that handles physics
	UPROPERTY(Category = "Components", VisibleAnywhere)
		UPhysicsMovementComponent* MovementComponent;
};
