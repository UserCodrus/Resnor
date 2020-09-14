// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsZone.generated.h"

// An actor that changes the physics of all actor inside it
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API APhysicsZone : public AActor
{
	GENERATED_BODY()
	
public:	
	APhysicsZone();

	// Called when a pawn enters the physics zone to set the pawn's physics state
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Called when a a pawn leaves the physics zone to reset its physics state
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:

	// Whether gravity is active in the zone or not
	UPROPERTY(Category = "ZoneSettings|Gravity", EditAnywhere)
		bool AllowGravity;
	// The gravity multiplier inside the zone
	UPROPERTY(Category = "ZoneSettings|Gravity", EditAnywhere)
		float Gravity;

	UPROPERTY(Category = "ZoneSettings|Orientation", EditAnywhere)
		bool SetOrientation;
	UPROPERTY(Category = "ZoneSettings|Orientation", EditAnywhere)
		FVector Forward;
	UPROPERTY(Category = "ZoneSettings|Orientation", EditAnywhere)
		FVector Up;

	UPROPERTY(Category = "ZoneSettings|Friction", EditAnywhere)
		float Friction;
	UPROPERTY(Category = "ZoneSettings|Friction", EditAnywhere)
		float AirFriction;

	UPROPERTY(Category = "ZoneSettings|Force", EditAnywhere)
		FVector Force;

	UPROPERTY(Category = "ZoneSettings|Time", EditAnywhere)
		float TickRate;
};

/*UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API APhysicsZoneBox : public APhysicsZone
{
	GENERATED_BODY()

public:
	APhysicsZoneBox();

protected:
	// The collision box used to generate overlaps
	UPROPERTY(VisibleAnywhere)
		UBoxComponent* Collision;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API APhysicsZoneSphere : public APhysicsZone
{
	GENERATED_BODY()

public:
	APhysicsZoneSphere();

protected:
	// The collision sphere used to generate overlaps
	UPROPERTY(VisibleAnywhere)
		USphereComponent* Collision;
};*/