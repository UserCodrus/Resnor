// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CyberShooterPawn.h"
#include "CyberShooterEnemy.generated.h"

// A standard enemy type
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ACyberShooterEnemy : public ACyberShooterPawn
{
	GENERATED_BODY()
	
public:
	ACyberShooterEnemy();

	// The function that handles the enemy hitting obstacles
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Damage the enemy deals on contact to players
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, BlueprintReadWrite)
		int32 ContactDamage;
	// The damage type dealt on contact
	UPROPERTY(Category = "Attributes|Damage", EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = EDamageType))
		int32 ContactDamageType;
};
