// Copyright © 2020 Brian Faubion. All rights reserved.

#include "VanishingPlatform.h"

AVanishingPlatform::AVanishingPlatform()
{
	// Enable ticking for timers
	PrimaryActorTick.bCanEverTick = true;

	// Adjust mesh settings
	UStaticMeshComponent* mesh = GetStaticMeshComponent();
	mesh->SetMobility(EComponentMobility::Movable);

	Solid = true;

	SolidTime = 10.0f;
	HiddenTime = 10.0f;
	AlertDuration = 1.0f;
	BlinkSpeed = 20;
	TimerOffset = 0.0f;

	Active = true;
}

void AVanishingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// Set the initial state
	Change(Solid);
	Timer -= TimerOffset;
}

void AVanishingPlatform::Tick(float DeltaSeconds)
{
	if (!Active)
		return;

	if (Timer > 0.0f)
	{
		// Manage the timer if it has been set
		Timer -= DeltaSeconds;
		if (Timer <= 0.0f)
		{
			Change(!Solid);
		}
		else if (Solid && Timer < AlertDuration)
		{
			// Blink when the timer is low
			int state = (int)(Timer * BlinkSpeed) & 2;
			SetActorHiddenInGame((bool)state);
		}
	}
}

bool AVanishingPlatform::IsStable() const
{
	return false;
}

void AVanishingPlatform::TogglePlatform()
{
	if (!Active)
		return;

	// Force the platform to change states
	Timer = 0.0f;
	Change(!Solid);
}

void AVanishingPlatform::SetTimer(float Time, bool Overwrite)
{
	if (Overwrite || Timer <= 0.0f)
	{
		Timer = Time;
	}
}

void AVanishingPlatform::Activate()
{
	Active = true;

	// Reset the timer then reveal the platform
	Timer = 0.0f;
	Change(true);
}

void AVanishingPlatform::Deactivate()
{
	Active = false;

	// Hide the platform
	Change(false);
}

/// Vanishing Platform Functions ///

void AVanishingPlatform::Change(bool State)
{
	// Change the state of the platform
	Solid = State;
	SetActorHiddenInGame(!Solid);
	SetActorEnableCollision(Solid);

	// Set the timer if needed
	if (Solid)
	{
		Timer += SolidTime;
	}
	else
	{
		Timer += HiddenTime;
	}
}

