// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

// A moving platform that changes direction after hitting a platform trigger
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AMovingPlatform();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual FVector GetVelocity() const override;

	// Move the marker to the center of the platform
	UFUNCTION(Category = Platform, CallInEditor)
		void ResetMarker();

protected:
	// The marker for the platform's destination
	UPROPERTY(Category = Platform, EditAnywhere, BlueprintReadWrite)
		USceneComponent* DestinationMarker;
	// The starting point of the platform
	UPROPERTY(Category = Platform, EditAnywhere, BlueprintReadWrite)
		FVector Origin;
	// The time it takes the platform to move between points
	UPROPERTY(Category = Timing, EditAnywhere, BlueprintReadWrite)
		float TravelTime;
	// The delay in seconds after reaching an endpoint before the platform starts moving again
	UPROPERTY(Category = Timing, EditAnywhere, BlueprintReadWrite)
		float DelayTime;

	// The speed the platform moves at
	float Speed;
	// The timer for platform movement
	float Timer;
	// Set to true if the platform is being delayed
	bool Delayed;
	// Set to true when moving to the origin, otherwise the platform moves to the destination marker
	bool ReturnTrip;
};
