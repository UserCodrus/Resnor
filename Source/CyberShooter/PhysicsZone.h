// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsZone.generated.h"

#if WITH_EDITORONLY_DATA
class UArrowComponent;
#endif

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

	// If set to true, the orientation of pawns will change when inside the physics zone
	UPROPERTY(Category = "ZoneSettings|Orientation", EditAnywhere)
		bool SetOrientation;

	// Surface friction multiplier inside the zone
	UPROPERTY(Category = "ZoneSettings|Friction", EditAnywhere)
		float Friction;
	// Air friction inside the zone
	UPROPERTY(Category = "ZoneSettings|Friction", EditAnywhere)
		float AirFriction;

	// Static forces applied to pawns in the zone
	UPROPERTY(Category = "ZoneSettings|Force", EditAnywhere)
		FVector Force;

	// The tick speed of pawns inside the zone
	UPROPERTY(Category = "ZoneSettings|Time", EditAnywhere)
		float TickSpeed;

	// The component that controls the orientation
	UPROPERTY(Category = "Components", EditAnywhere)
		USceneComponent* OrientationComponent;

#if WITH_EDITORONLY_DATA
	// Arrow indicating the forward orientation vector
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* OrientationForwardArrow;
	// Arrow indicating the up orientation vector
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* OrientationUpArrow;
#endif
};