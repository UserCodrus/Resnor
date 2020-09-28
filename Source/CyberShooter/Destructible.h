// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CombatInterface.h"

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Destructible.generated.h"

UCLASS(Blueprintable)
class CYBERSHOOTER_API ADestructible : public AStaticMeshActor, public ICombatInterface
{
	GENERATED_BODY()
	
public:	
	ADestructible();

	virtual void BeginPlay() override;

	/// ICombatInterface ///
	
	virtual void Damage(int32 Value, int32 DamageType, AActor* Source, AActor* Origin) override;
	virtual void Heal(int32 Value) override;
	virtual void Kill() override;

protected:
	// The maximum health of the object
	UPROPERTY(Category = Attributes, EditAnywhere, BlueprintReadWrite)
		int32 MaxHealth;
	// The object's current health
	UPROPERTY(Category = Attributes, EditInstanceOnly, BlueprintReadWrite)
		int32 Health;
	// Flags for which types of damage can affect the pawn
	UPROPERTY(Category = Attributes, EditAnywhere, meta = (Bitmask, BitmaskEnum = EDamageTypeFlag))
		int32 DamageImmunity;

	// Particles that spawn when killed
	UPROPERTY(Category = Components, EditAnywhere, BlueprintReadOnly)
		UParticleSystem* DeathParticles;
	// The sound that plays when killed
	UPROPERTY(Category = Components, EditAnywhere, BlueprintReadOnly)
		USoundBase* DeathSound;
};
