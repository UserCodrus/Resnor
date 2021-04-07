// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Lock.h"
#include "CyberShooterGameInstance.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/Engine.h"

ALock::ALock()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set defaults
	NumTriggers = 0;
	RequiredTriggers = 1;
	PermanentUnlock = false;
	Locked = true;
	InvertLock = false;
	InvertMechanism = false;
	ID = 0;
}

void ALock::BeginPlay()
{
	Super::BeginPlay();

	// Unlock at the start if the lock is permanent and has been unlocked before
	if (PermanentUnlock)
	{
		UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
		if (instance != nullptr)
		{
			if (instance->CheckUnlocked(ID))
			{
				UnlockTarget(true);
				Locked = false;
			}
		}
	}
}

/// ILockInterface ///

void ALock::Unlock()
{
	if (InvertMechanism)
	{
		UntriggerLock();
	}
	else
	{
		TriggerLock();
	}
}

void ALock::Lock()
{
	if (InvertMechanism)
	{
		TriggerLock();
	}
	else
	{
		UntriggerLock();
	}
}

/// Lock Functions ///

bool ALock::TriggerLock()
{
	NumTriggers++;

	// Unlock the target object if enough triggers have been used
	if (NumTriggers >= RequiredTriggers && Locked)
	{
		Locked = false;
		UnlockTarget();
		return true;
	}

	return false;
}

bool ALock::UntriggerLock()
{
	NumTriggers--;

	// Lock the target object if enough triggers have been used
	if (!PermanentUnlock)
	{
		if (NumTriggers < RequiredTriggers && !Locked)
		{
			Locked = true;
			LockTarget();
			return true;
		}
	}

	return false;
}

void ALock::UnlockTarget(bool Silent)
{
	// Unlock the targets
	for (int32 i = 0; i < Targets.Num(); ++i)
	{
		/*ILockInterface* lock_target = Cast<ILockInterface>(Targets[i]);
		if (lock_target != nullptr)
		{
			if (InvertLock)
			{
				lock_target->Lock();
			}
			else
			{
				lock_target->Unlock();
			}
		}*/
	}

	if (!Silent)
	{
		// Play the unlock sound
		if (UnlockSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, UnlockSound, GetActorLocation());
		}

		// Save the lock state
		if (PermanentUnlock)
		{
			UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
			if (instance != nullptr)
			{
				instance->SaveLock(ID);
			}
		}
	}

	FString msg("Unlocked ");
	msg.Append(GetName());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, msg);
}

void ALock::LockTarget(bool Silent)
{
	// Lock the targets
	for (int32 i = 0; i < Targets.Num(); ++i)
	{
		/*ILockInterface* lock_target = Cast<ILockInterface>(Targets[i]);
		if (lock_target != nullptr)
		{
			if (InvertLock)
			{
				lock_target->Unlock();
			}
			else
			{
				lock_target->Lock();
			}
		}*/
	}

	if (!Silent)
	{
		// Play the Lock sound
		if (LockSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, LockSound, GetActorLocation());
		}
	}

	FString msg("Locked ");
	msg.Append(GetName());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, msg);
}