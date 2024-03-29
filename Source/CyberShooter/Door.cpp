// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Door.h"

#include "Kismet/GameplayStatics.h"

ADoor::ADoor()
{
	Opened = false;
}

void ADoor::BeginPlay()
{
	// Open the door at the start of the level
	if (Opened)
	{
		DoorOpen();
	}
}

/// Door Functions ///

void ADoor::Open()
{
	// Call the OnOpen event
	if (!Opened)
	{
		Opened = true;
		DoorOpen();

		if (OpenSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
		}
	}
}

void ADoor::Close()
{
	// Call the OnClose event
	if (Opened)
	{
		Opened = false;
		DoorClose();

		if (CloseSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());
		}
	}
}

/// Blueprint Events ///

void ADoor::DoorOpen_Implementation()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}

void ADoor::DoorClose_Implementation()
{
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
}
