// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Lock.h"
#include "CyberShooterGameInstance.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/Engine.h"

ALock::ALock()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set defaults
	UnlockCounter = 0;
	RequiredUnlocks = 1;
	Unlocked = true;
	ID = 0;
}

void ALock::BeginPlay()
{
	Super::BeginPlay();

	// Unlock at the start based on save data
	if (ID > -1)
	{
		UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
		if (instance != nullptr)
		{
			Unlocked = instance->CheckUnlocked(ID);
		}
	}
}

void ALock::Tick(float DeltaSeconds)
{
	// Call the lock's script when it unlocks
	if (Unlocked)
	{
		OnUnlock.Broadcast();

		FString msg("Unlocked ");
		msg.Append(GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, msg);
	}

	// Stop ticking
	SetActorTickEnabled(false);
}

/// Lock Functions ///

void ALock::Unlock()
{
	UnlockCounter++;

	// Unlock the lock after this function has been called enough times
	if (UnlockCounter >= RequiredUnlocks && !Unlocked)
	{
		// Save the lock's state
		UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
		if (instance != nullptr)
		{
			instance->SaveLock(ID);
		}

		// Prepare to activate the lock
		Unlocked = true;
		SetActorTickEnabled(true);
	}
}

void ALock::Lock()
{
	UnlockCounter--;
}