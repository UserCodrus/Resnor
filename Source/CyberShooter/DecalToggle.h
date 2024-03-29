// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DecalToggle.generated.h"

// A decal that can be toggled between two states using a lock
UCLASS()
class CYBERSHOOTER_API ADecalToggle : public AActor
{
	GENERATED_BODY()
	
public:	
	ADecalToggle();

	// Swap the state of the decal toggle
	UFUNCTION(BlueprintCallable)
		void ToggleDecals();

protected:
	virtual void BeginPlay() override;

	// Set the visibility of the decals based on the active flag
	void UpdateDecals();

	// Set to true when the decal is activated
	UPROPERTY(Category = "Status", EditAnywhere, BlueprintReadOnly)
		bool Active;

	// The decal that show when the toggle is active
	UPROPERTY(Category = "Components", EditAnywhere)
		UDecalComponent* ActiveDecal;
	// The decal that show when the toggle is inactive
	UPROPERTY(Category = "Components", EditAnywhere)
		UDecalComponent* InactiveDecal;
};
