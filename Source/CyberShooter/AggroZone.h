// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AggroZone.generated.h"

// A box that controls the aggro of enemies inside it
UCLASS()
class CYBERSHOOTER_API AAggroZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AAggroZone();

	// Aggro enemies when the player enters the zone
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Remove aggro when the player exits the zone
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:
	// Enemies controlled by this zone
	UPROPERTY(Category = "AI", EditInstanceOnly)
		TArray<class AEnemyBase*> Enemies;

	// The collision box for handling overlaps
	UPROPERTY(Category = "Components", EditAnywhere)
		class UBoxComponent* CollisionBox;
};
