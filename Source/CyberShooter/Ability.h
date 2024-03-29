// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Ability.generated.h"

class ACyberShooterPawn;

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API UAbilityScript : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		bool Activate(ACyberShooterPawn* Target);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		bool Deactivate(ACyberShooterPawn* Target);
};

// An ability that can be used by the player
UCLASS(BlueprintType)
class CYBERSHOOTER_API UAbility : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UAbility();

	// The UI icon for the ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UTexture2D* Icon;
	// The name of the ability in the UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FText DisplayName;

	// The momentum cost of the ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float Cost;
	// If set to true, the ability will be used continuously while active
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool Continuous;

	// The cooldown in seconds after using this ability before another can be used
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float Cooldown;

	// The script used by the ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<UAbilityScript> Script;
	// The particles used for the ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UParticleSystem* Particles;
};
