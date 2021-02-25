// Copyright © 2020 Brian Faubion. All rights reserved.

#include "DamageTrigger.h"
#include "CombatInterface.h"

ADamageTrigger::ADamageTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &ADamageTrigger::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ADamageTrigger::EndOverlap);

	// Set property defaults
	Damage = 1;
	
	Disabled = false;
	EnableWhenUnlocked = true;
	AutoCycle = true;
	Active = false;
	InstantDamage = false;
	TargetOverlap = true;
	ActiveDuration = 1.0f;
	InactiveDuration = 1.0f;
	TimerOffset = 0.0f;
}


void ADamageTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Disabled)
		return;

	if (AutoCycle)
	{
		// Increment the activity timer
		Timer += DeltaTime;
		if (Active)
		{
			if (Timer > ActiveDuration)
			{
				// Deactivate the trigger
				DeactivateTrigger();
				Timer -= ActiveDuration;
			}
		}
		else
		{
			if (Timer > InactiveDuration)
			{
				// Activate the trigger
				ActivateTrigger();
				Timer -= InactiveDuration;
			}
		}
	}

	// Deal damage to each object inside the trigger
	if (!InstantDamage && Active)
	{
		ApplyDamage();
	}
}

void ADamageTrigger::BeginPlay()
{
	Super::BeginPlay();

	// Set the default timer
	Timer = TimerOffset;
}

/// ILockInterface ///

void ADamageTrigger::Unlock()
{
	if (EnableWhenUnlocked)
	{
		Disabled = false;
	}
	else
	{
		ActivateTrigger();
	}

	Execute_OnUnlock(this);
}

void ADamageTrigger::Lock()
{
	if (EnableWhenUnlocked)
	{
		Disabled = true;
	}
	else
	{
		DeactivateTrigger();
	}

	Execute_OnLock(this);
}

/// Damage Trigger Functions ///

void ADamageTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!TargetOverlap)
		return;

	// Add actors to the target list
	ICombatInterface* object = Cast<ICombatInterface>(OtherActor);
	if (object != nullptr)
	{
		Targets.Add(OtherActor);
	}
}

void ADamageTrigger::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!TargetOverlap)
		return;

	// Remove actors from the target list
	ICombatInterface* object = Cast<ICombatInterface>(OtherActor);
	if (object != nullptr)
	{
		Targets.Remove(OtherActor);
	}
}

void ADamageTrigger::ApplyDamage()
{
	// Damage each target
	for (int32 i = 0; i < Targets.Num(); ++i)
	{
		ICombatInterface* object = Cast<ICombatInterface>(Targets[i]);
		if (object != nullptr)
		{
			object->Damage(Damage, EDamageType::DAMAGETYPE_ENVIRONMENT, RumbleEffect, nullptr, this, this);
		}
	}
}

void ADamageTrigger::ActivateTrigger()
{
	Active = true;
	if (InstantDamage)
	{
		// Deal damage and cycle the timer
		ApplyDamage();
		Timer += ActiveDuration;
	}

	OnActivate();
}

void ADamageTrigger::DeactivateTrigger()
{
	Active = false;

	OnDeactivate();
}