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

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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

protected:
	// The delay in seconds after reaching an endpoint before the object starts moving again
	UPROPERTY(Category = "Movement", EditAnywhere)
		float DelayTime;
	// If set to true, the object will travel back and forth, otherwise it will warp back to the beginning
	UPROPERTY(Category = "Movement", EditAnywhere)
		bool RoundTrip;

	// The timer for movement
	float Timer;
	// Set to true if the actor is waiting at an endpoint
	bool Delayed;
	// Set to false when moving from the starting spline point to the end, and true to move from the end to the start
	bool ReturnTrip;
};
