// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "Ability.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CyberShooterGameInstance.generated.h"

// The game instance
UCLASS(Blueprintable)
class CYBERSHOOTER_API UCyberShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Retrieve an ability script
	UAbilityScript* GetScript(TSubclassOf<UAbilityScript> ScriptType);

protected:
	// The ability scripts used by the ability system
	UPROPERTY(VisibleAnywhere)
		TArray<UAbilityScript*> Scripts;
};
