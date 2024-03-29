// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class AActor;

// Enum to identify damage types in the editor
UENUM(meta = (Bitflags))
enum class EDamageTypeFlag : int32
{
	PLAYER,
	ENEMY,
	ENVIRONMENT,
	PHYSICS,
	EXPLOSION
};

// Enum to identify damage types
enum EDamageType : int32
{
	DAMAGETYPE_NONE = 0,				// Null damage type, ignores immunity and resistance
	DAMAGETYPE_PLAYER = 0b0001,			// Damage dealt by a player
	DAMAGETYPE_ENEMY = 0b0010,			// Damage dealt by an enemy
	DAMAGETYPE_ENVIRONMENT = 0b0100,	// Damage dealt by environmental hazards
	DAMAGETYPE_PHYSICS = 0b1000,		// Damage dealt by physics impacts
	DAMAGETYPE_EXPLOSION = 0b10000		// Damage dealt by explosions
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

// An interface for objects that can be damaged and destroyed
class CYBERSHOOTER_API ICombatInterface
{
	GENERATED_BODY()

public:
	// Damaging the object - returns true if damage was successfully applied
	UFUNCTION()
		virtual bool Damage(int32 Value, int32 DamageType, UForceFeedbackEffect* RumbleEffect, UPrimitiveComponent* HitComp, AActor* Source, AActor* Origin) = 0;
	// Heal the object
	UFUNCTION()
		virtual void Heal(int32 Value) = 0;
	// Called when the object is killed
	UFUNCTION()
		virtual void Kill() = 0;
};
