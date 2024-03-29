// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon.generated.h"

// A weapon that can be used by the player or an enemy
UCLASS(BlueprintType)
class CYBERSHOOTER_API UWeapon : public UDataAsset
{
	GENERATED_BODY()

public:
	UWeapon();

	// The UI icon for the weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UTexture2D* Icon;
	// The name of the weapon in the UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FText DisplayName;
	// The sound played when firing the weapon
	UPROPERTY(EditDefaultsOnly)
		USoundBase* Sound;

	// The projectile produced by the weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<AActor> Projectile;

	// The firing rate in beats
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float FireRate;
	// A random angle added to bullets
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float FireAccuracy;
	// The recoil from firing the weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float Recoil;

	// The number of bullets fired with each shot
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 NumBullets;
	// The angle between bullets when multiple shots are fired
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float BulletOffset;
	// If true shots will be fired at the same angle as BulletOffset, otherwise they all fire forward
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool FanBullets;
};
