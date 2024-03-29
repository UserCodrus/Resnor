// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillZone.generated.h"

// An invisible box that forces the player to respawn
UCLASS()
class CYBERSHOOTER_API AKillZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AKillZone();

	// Respawn the player when it overlaps the zone
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
