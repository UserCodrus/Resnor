// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PlayerMovementComponent.generated.h"

// The main movement component for player controlled pawns
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent))
class CYBERSHOOTER_API UPlayerMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	UPlayerMovementComponent();

	/// UPawnMovementComponent ///

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Change the speed multiplier to a new value
	virtual void ChangeSpeed(float NewSpeed);

protected:
	// Apply controller input to the component
	virtual void ApplyControlInputToVelocity(float DeltaTime);

	// The max speed of the pawn
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
		float TurningMultiplier;
	// A boost to acceleration, deceleration and movement speed based on the player's stats
	UPROPERTY(Category = Movement, EditAnywhere)
		float SpeedMultiplier;

	// If set to true the pawn will rotate in the direction of its velocity
	UPROPERTY(Category = Movement, EditAnywhere)
		bool FaceVelocity;

	// The maximum angle of a surface that the pawn will be able to climb
	UPROPERTY(Category = Movement, EditAnywhere)
		float MaxIncline;
	// The up vector for the pawn
	UPROPERTY(Category = Physics, EditAnywhere)
		FVector Up;
	// The strength of gravity
	UPROPERTY(Category = Physics, EditAnywhere)
		float Gravity;
};
