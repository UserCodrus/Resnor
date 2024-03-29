// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DecalHidden.generated.h"

// A decal that can be shown or hidden by blueprints
UCLASS()
class CYBERSHOOTER_API ADecalHidden : public AActor
{
	GENERATED_BODY()
	
public:	
	ADecalHidden();

	// Swap the state of the decal
	UFUNCTION(BlueprintCallable)
		void ToggleDecal();

protected:
	// The decal component
	UPROPERTY(Category = "Components", EditAnywhere)
		UDecalComponent* DecalComponent;
};
