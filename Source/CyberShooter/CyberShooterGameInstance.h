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
	UCyberShooterGameInstance();

	// Retrieve an ability script
	UAbilityScript* GetScript(TSubclassOf<UAbilityScript> ScriptType);
	
	/// Accessors ///
	
	inline float GetGravity() const { return Gravity; }
	inline float GetAirFriction() const { return AirFriction; }

protected:
	UPROPERTY(Category = Physics, EditDefaultsOnly)
		float Gravity;
	UPROPERTY(Category = Physics, EditDefaultsOnly)
		float AirFriction;

	// The ability scripts used by the ability system
	UPROPERTY(VisibleAnywhere)
		TArray<UAbilityScript*> Scripts;
};
