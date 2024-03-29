// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckpointTrigger.generated.h"

// A checkpoint that saves the game when the player enters it
UCLASS()
class CYBERSHOOTER_API ACheckpointTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ACheckpointTrigger();

protected:
	// Save the game when the player overlaps the trigger
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Save game data
	void SaveGame();

#if WITH_EDITORONLY_DATA
	// Arrow indicating the orientation of the zone
	UPROPERTY(Category = "Components", EditAnywhere)
		class UArrowComponent* OrientationArrow;
#endif
};
