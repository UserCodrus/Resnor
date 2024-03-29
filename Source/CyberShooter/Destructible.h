// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CombatInterface.h"
#include "AggroInterface.h"

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Destructible.generated.h"

UCLASS(Blueprintable)
class CYBERSHOOTER_API ADestructible : public AActor, public ICombatInterface, public IAggroInterface
{
	GENERATED_BODY()
	
public:	
	ADestructible();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Remove the destructible from play
	void Enable();
	// Restore the destructible
	void Disable();

	/// ICombatInterface ///
	
	virtual bool Damage(int32 Value, int32 DamageType, UForceFeedbackEffect* RumbleEffect, UPrimitiveComponent* HitComp, AActor* Source, AActor* Origin) override;
	virtual void Heal(int32 Value) override;
	virtual void Kill() override;

	/// IAggroInterface ///

	virtual void Aggro() override;
	virtual void EndAggro() override;
	virtual bool IsAggro() override;
	virtual void AggroReset() override;
	virtual void AggroDisable() override {};
	virtual void RegisterZone(class AAggroZone* Zone) override {};

protected:
	// The aggro counter for the destructible
	UPROPERTY(Category = "Aggro", EditAnywhere)
		int32 AggroLevel;
	// The aggro amount required to damage the destructible
	UPROPERTY(Category = "Aggro", EditAnywhere)
		int32 RequiredAggro;

	// Set to true when the object is disabled
	UPROPERTY(Category = "Attributes", VisibleAnywhere)
		bool Disabled;
	// If set to true, the destructible will break permanently after being destroyed, otherwise it will respawn when its aggro zone resets
	UPROPERTY(Category = "Attributes", EditAnywhere)
		bool Ephemeral;
	// The maximum health of the object
	UPROPERTY(Category = "Attributes", EditAnywhere)
		int32 MaxHealth;
	// The object's current health
	UPROPERTY(Category = "Attributes", EditInstanceOnly)
		int32 Health;
	// Flags for which types of damage can affect the pawn
	UPROPERTY(Category = "Attributes", EditAnywhere, meta = (Bitmask, BitmaskEnum = EDamageTypeFlag))
		int32 DamageImmunity;
	// The damage reduction applied to damage taken
	UPROPERTY(Category = "Attributes", EditAnywhere)
		int32 Resistance;

	// The time it takes the object to spawn in after respawning
	UPROPERTY(Category = "Attributes", EditAnywhere)
		float RespawnCooldown;
	// The duration the pawn will blink after taking damage
	UPROPERTY(Category = "Attributes", EditAnywhere)
		float DamageCooldownDuration;
	// The blink rate when the destructible is respawning
	UPROPERTY(Category = "Attributes", EditAnywhere)
		int32 BlinkRate;

	// Particles that spawn when killed
	UPROPERTY(Category = "Components", EditAnywhere)
		UParticleSystem* DeathParticles;
	// The sound that plays when killed
	UPROPERTY(Category = "Components", EditAnywhere)
		USoundBase* DeathSound;

	// The timer used to track invincibility after taking damage
	float DamageCooldown;
};
