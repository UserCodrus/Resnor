// Copyright © 2020 Brian Faubion. All rights reserved.

#include "KillZone.h"
#include "CyberShooterPlayer.h"

AKillZone::AKillZone()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &AKillZone::BeginOverlap);
}

void AKillZone::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Check to make sure a player entered the zone
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Force a respawn
		player->ForceRespawn();
	}
}