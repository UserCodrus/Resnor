// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTrigger.generated.h"

#if WITH_EDITORONLY_DATA
class UArrowComponent;
#endif

// A trigger that sends the player to a different level
UCLASS()
class CYBERSHOOTER_API ALevelTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelTrigger();

	FORCEINLINE uint32 GetID() const { return TriggerID; }

protected:
	// Change levels when the player overlaps the trigger
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// The level the trigger will switch to
	UPROPERTY(EditAnywhere)
		FName TargetLevel;
	// The ID of this trigger
	UPROPERTY(EditAnywhere)
		uint32 TriggerID;

#if WITH_EDITORONLY_DATA
	// Arrow indicating the forward orientation vector
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* ForwardArrow;
	// Arrow indicating the up orientation vector
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* UpArrow;
#endif
};
