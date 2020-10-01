// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Switch.h"
#include "Lock.h"
#include "CyberShooterPlayer.h"

/// ASwitch ///

ASwitch::ASwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &ASwitch::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ASwitch::EndOverlap);

	PermanentTrigger = true;

	TriggerDuration = 0.0f;
	NumTriggers = 0;
	RequiredTriggers = 1;
}

void ASwitch::TriggerTimeout()
{
	OnUntriggered();
}

/// Blueprint Events ///

void ASwitch::OnTriggered_Implementation()
{
	if (Target != nullptr)
	{
		Target->TriggerLock();
	}
}

void ASwitch::OnUntriggered_Implementation()
{
	if (Target != nullptr)
	{
		Target->UntriggerLock();
	}
}

/// Switch Functions ///

bool ASwitch::CheckTrigger(AActor* TriggeringActor)
{
	return true;
}

void ASwitch::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	NumTriggers++;

	// Unlock the target lock if enough actors are on the switch
	if (NumTriggers >= RequiredTriggers && !Triggered)
	{
		Triggered = true;
		if (TriggerDuration > 0.0f)
		{
			// Cancel the untrigger timer if one is set, otherwise trigger the switch
			if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_SwitchTimer) > 0.0f)
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SwitchTimer);
			}
			else
			{
				OnTriggered();
			}
		}
		else
		{
			// Trigger the switch
			OnTriggered();
		}
	}
}

void ASwitch::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	NumTriggers--;

	// Lock the target lock if too many actors leave the switch
	if (!PermanentTrigger)
	{
		if (NumTriggers < RequiredTriggers && Triggered)
		{
			Triggered = false;
			if (TriggerDuration > 0.0f)
			{
				// Stay triggered for a while
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchTimer, this, &ASwitch::TriggerTimeout, TriggerDuration);
			}
			else
			{
				// Untrigger instantly
				OnUntriggered();
			}
		}
	}
}

/// ASwitch_Player ///

ASwitch_Player::ASwitch_Player()
{

}

bool ASwitch_Player::CheckTrigger(AActor* TriggeringActor)
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(TriggeringActor);
	if (player != nullptr)
	{
		return true;
	}

	return false;
}