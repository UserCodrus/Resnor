// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Lock.h"
#include "LockInterface.h"

ALock::ALock()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set defaults
	NumTriggers = 0;
	RequiredTriggers = 1;
	PermanentUnlock = false;
	Locked = true;
}

/// Lock Functions ///

bool ALock::TriggerLock()
{
	NumTriggers++;

	// Unlock the target object if enough triggers have been used
	if (NumTriggers >= RequiredTriggers && Locked)
	{
		Locked = false;
		OnUnlock();
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
			OnLock();
			return true;
		}
	}

	return false;
}

/// Blueprint Events ///

void ALock::OnUnlock_Implementation()
{
	ILockInterface* lock_target = Cast<ILockInterface>(Target);
	if (lock_target != nullptr)
	{
		lock_target->Unlock();
	}
}

void ALock::OnLock_Implementation()
{
	ILockInterface* lock_target = Cast<ILockInterface>(Target);
	if (lock_target != nullptr)
	{
		lock_target->Lock();
	}
}