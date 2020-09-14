// Copyright © 2020 Brian Faubion. All rights reserved.

#include "VelocityTrigger.h"
#include "CyberShooterPlayer.h"

/// AVelocityTrigger ///

AVelocityTrigger::AVelocityTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	// Set defaults
	Direction = FVector(1.0f, 0.0f, 0.0f);
	Magnitude = 1000.0f;
	RestrictOrientation = true;
	RequiredUp = FVector(0.0f, 0.0f, 1.0f);

	OnActorBeginOverlap.AddDynamic(this, &AVelocityTrigger::BeginOverlap);
}

void AVelocityTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Direction.IsNearlyZero() || Magnitude == 0.0f)
		return;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Make sure the player has the orientation required by the trigger
		if (RestrictOrientation)
		{
			if (!player->CheckOrientation(RequiredUp))
			{
				return;
			}
		}

		// Apply the velocity boost
		player->AddImpulse(Direction.GetSafeNormal() * Magnitude);
	}
}