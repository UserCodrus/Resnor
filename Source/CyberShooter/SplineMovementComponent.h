// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "SplineMovementComponent.generated.h"

class USplineComponent;

// A movement component that follows a spline
UCLASS()
class CYBERSHOOTER_API USplineMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
	
public:
	USplineMovementComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Start moving the actor when it is waiting at an endpoint
	bool StartMovement();
	// Force the platform to move towards its start point
	void MoveToStart();
	// Force the platform to move towards its end point
	void MoveToEnd();

	// Set to false when moving from the starting spline point to the end, and true to move from the end to the start
	// If OneWay is enabled, this will dictate the direction the pawn moves at all times
	// Otherwise the pawn will begin at the start or end of the spline when play begins based on this setting, then flip this setting when it reaches an endpoint
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool Reverse;

protected:
	// Start moving the object
	void StartMoving();
	// Stop the object's movement
	void StopMoving();

public:
	/// Properties ///

	// The spline the actor will follow
	UPROPERTY(Category = "Movement", EditAnywhere)
		USplineComponent* Spline;
	// If set to false, the actor will stop moving completely
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool CanMove;

protected:
	// If set to true, the actor will move from the start point of the spline to the end in a single trip
	// Otherwise, it will make trips from spline point to spline point, delaying at each point
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool FullPath;
	// If set to true, the actor will avoid penetrating objects when moving
	// May stop the actor if it gets too close to something, but required to properly register collisions
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool SafeMovement;
	// If set to true, the actor will move automatically, otherwise StartMovement must be called to begin moving
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool Automated;
	// If set to true, the actor will make a complete circuit before stopping after activated
	// If Automated is true this is ignored
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool RoundTrip;
	// The delay in seconds after reaching an endpoint before the object starts moving again
	UPROPERTY(Category = "Movement", EditAnywhere)
		float Delay;
	// If set to true, the object will warp back to the first spline point after reaching the end
	// Otherwise it will move back and forth between the two
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool OneWay;

	// The current spline point the actor is travelling from
	int32 CurrentPoint;
	// The timer for movement
	float Timer;
	// Set to true if the actor is waiting at an endpoint
	bool IsDelayed;
};
