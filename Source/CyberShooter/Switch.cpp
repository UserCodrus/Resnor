// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Switch.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

/// ASwitch ///

ASwitch::ASwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &ASwitch::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ASwitch::EndOverlap);

	PermanentTrigger = true;

	TriggerDuration = 0.0f;
	CooldownDuration = 0.0f;
	TotalWeight = 0.0f;
	RequiredWeight = 1.0f;

	Triggered = false;
	Cooldown = false;
}

void ASwitch::TriggerTimeout()
{
	Triggered = false;
	ReleaseSwitch();
}

void ASwitch::CooldownTimeout()
{
	Cooldown = false;
}

void ASwitch::ActivateEvent()
{
	if (OnActivate.IsBound())
	{
		OnActivate.Broadcast();
	}
	else
	{
		// Call parent delegates
		ASwitch* parent = Cast<ASwitch>(GetAttachParentActor());
		if (parent != nullptr)
		{
			parent->ActivateEvent();
		}
	}
}

void ASwitch::DeactivateEvent()
{
	if (OnDeactivate.IsBound())
	{
		OnDeactivate.Broadcast();
	}
	else
	{
		// Call parent delegates
		ASwitch* parent = Cast<ASwitch>(GetAttachParentActor());
		if (parent != nullptr)
		{
			parent->DeactivateEvent();
		}
	}
}

/// Blueprint Events ///

void ASwitch::TriggerSwitch()
{
	if (!Cooldown)
	{
		Triggered = true;

		// Play the trigger sound
		if (TriggerSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, TriggerSound, GetActorLocation());
		}

		// Start the switch cooldown
		if (CooldownDuration > 0.0f)
		{
			Cooldown = true;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchCooldown, this, &ASwitch::CooldownTimeout, CooldownDuration);
		}

		// Call blueprint events
		ActivateEvent();
		Trigger();
	}
}

void ASwitch::ReleaseSwitch()
{
	if (!Cooldown)
	{
		Triggered = false;

		// Play the untrigger sound
		if (UntriggerSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, UntriggerSound, GetActorLocation());
		}

		// Start the switch cooldown
		if (CooldownDuration > 0.0f)
		{
			Cooldown = true;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchCooldown, this, &ASwitch::CooldownTimeout, CooldownDuration);
		}

		// Call blueprint events
		DeactivateEvent();
		Release();
	}
}

/// Switch Functions ///

float ASwitch::GetTriggerValue(AActor* TriggeringActor)
{
	return 1.0f;
}

void ASwitch::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	TotalWeight += GetTriggerValue(OtherActor);

	// Unlock the target lock if enough actors are on the switch
	if (TotalWeight >= RequiredWeight && !Triggered)
	{
		if (TriggerDuration > 0.0f)
		{
			// Cancel the untrigger timer if one is set, otherwise trigger the switch
			if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_SwitchTimer) > 0.0f)
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SwitchTimer);
			}
			else
			{
				TriggerSwitch();
			}
		}
		else
		{
			// Trigger the switch
			TriggerSwitch();
		}
	}
}

void ASwitch::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	TotalWeight -= GetTriggerValue(OtherActor);

	// Lock the target lock if too many actors leave the switch
	if (!PermanentTrigger)
	{
		if (TotalWeight < RequiredWeight && Triggered)
		{
			if (TriggerDuration > 0.0f)
			{
				// Stay triggered for a while
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchTimer, this, &ASwitch::TriggerTimeout, TriggerDuration);
			}
			else
			{
				// Release instantly
				ReleaseSwitch();
			}
		}
	}
}

/// ASwitch_Player ///

ASwitch_Player::ASwitch_Player()
{
	
}

float ASwitch_Player::GetTriggerValue(AActor* TriggeringActor)
{
	// Check to see if the tiggering actor is a player
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(TriggeringActor);
	if (player != nullptr)
	{
		return player->GetKeys() + 1;
	}

	return 0.0f;
}

/// ASwitch_Physics ///

ASwitch_Physics::ASwitch_Physics()
{
	RequiredWeight = 1000.0f;
}

float ASwitch_Physics::GetTriggerValue(AActor* TriggeringActor)
{
	// Check to see if the triggering actor is a physics object
	IPhysicsInterface* object = Cast<IPhysicsInterface>(TriggeringActor);
	if (object != nullptr)
	{
		return object->GetWeight();
	}

	return 0.0f;
}

/// ASwitch_Damage ///

ASwitch_Damage::ASwitch_Damage()
{
	DamageImmunity = DAMAGETYPE_NONE;
}

/// ICombatInterface ///

bool ASwitch_Damage::Damage(int32 Value, int32 DamageType, UForceFeedbackEffect* RumbleEffect, UPrimitiveComponent* HitComp, AActor* Source, AActor* Origin)
{
	// Make sure the damage is sufficient to trigger the switch
	if (!(DamageImmunity & DamageType) && Value >= (int32)RequiredWeight)
	{
		if (!Triggered)
		{
			// Trigger the switch
			TriggerSwitch();

			if (TriggerDuration > 0.0f)
			{
				// Set the trigger timer
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchTimer, this, &ASwitch_Damage::TriggerTimeout, TriggerDuration);
			}
		}
		else if (!PermanentTrigger)
		{
			// Untrigger the switch
			ReleaseSwitch();

			if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_SwitchTimer) > 0.0f)
			{
				// Cancel the untrigger timer
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SwitchTimer);
			}
		}
		else
		{
			if (TriggerDuration > 0.0f)
			{
				// Reset the timer
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchTimer, this, &ASwitch_Damage::TriggerTimeout, TriggerDuration);
			}
		}

		return true;
	}

	return false;
}

void ASwitch_Damage::Heal(int32 Value)
{
	// Do nothing
}

void ASwitch_Damage::Kill()
{
	// Do nothing
}

float ASwitch_Damage::GetTriggerValue(AActor* TriggeringActor)
{
	return 0.0f;
}