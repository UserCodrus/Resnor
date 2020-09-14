// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VelocityTrigger.generated.h"

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AVelocityTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AVelocityTrigger();

protected:
	// Apply a velocity impulse to the player when they hit the trigger
	UFUNCTION()
		virtual void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// The direction of the velocity impulse
	UPROPERTY(Category = VelocityTrigger, EditAnywhere)
		FVector Direction;
	// The strength of the velocity impulse
	UPROPERTY(Category = VelocityTrigger, EditAnywhere)
		float Magnitude;

	// If set to true the player will require the provided up vector to use this trigger
	UPROPERTY(Category = VelocityTrigger, EditAnywhere)
		bool RestrictOrientation;
	// The up vector required to use the restricted trigger
	UPROPERTY(Category = VelocityTrigger, EditAnywhere)
		FVector RequiredUp;
};