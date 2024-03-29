// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GhostZone.generated.h"

// An object that only ghosts can pass through
UCLASS()
class CYBERSHOOTER_API AGhostZone : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	AGhostZone();

	// Notify ghosts when they enter the zone
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Notify ghosts when they exit the zone
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
