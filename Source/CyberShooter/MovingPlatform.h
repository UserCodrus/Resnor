// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

class UBoxComponent;

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API APlatformTrigger : public AActor
{
	GENERATED_BODY()

public:
	APlatformTrigger();

protected:
	// The collision box used to detect overlaps with platforms
	UPROPERTY(EditAnywhere)
		UBoxComponent* Collision;
};

// A moving platform that changes direction after hitting a platform trigger
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AMovingPlatform();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the platform overlaps a trigger
	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// The direction of the platforms movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Direction;
	// The speed that the platform moves at
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Speed;
	// The speed while delayed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Slow;
	// The delay in seconds after hitting a trigger before the platform starts moving again
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Delay;

	// The time left for the current delay period
	float DelayTimer;
};
