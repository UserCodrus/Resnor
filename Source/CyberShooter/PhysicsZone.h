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

	// If set to true, the zone will affect gravity
	UPROPERTY(Category = "ZoneSettings|Gravity", EditAnywhere)
		bool AffectGravity;
	// The gravity multiplier inside the zone
	UPROPERTY(Category = "ZoneSettings|Gravity", EditAnywhere)
		float Gravity;

	// If set to true, the zone will affect friction
	UPROPERTY(Category = "ZoneSettings|Friction", EditAnywhere)
		bool AffectFriction;
	// Surface friction multiplier inside the zone
	UPROPERTY(Category = "ZoneSettings|Friction", EditAnywhere)
		float Friction;
	// Air friction inside the zone
	UPROPERTY(Category = "ZoneSettings|Friction", EditAnywhere)
		float AirFriction;

	// The mass multiplier in the zone
	UPROPERTY(Category = "ZoneSettings|Mass", EditAnywhere)
		float Mass;

	// Static forces applied to pawns in the zone, will be ignored if set to zero
	UPROPERTY(Category = "ZoneSettings|Force", EditAnywhere)
		FVector Force;

	// The tick speed of pawns inside the zone, will be ignored if set to 1
	UPROPERTY(Category = "ZoneSettings|Time", EditAnywhere)
		float TickSpeed;
};