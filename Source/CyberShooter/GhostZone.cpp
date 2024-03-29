// Copyright © 2020 Brian Faubion. All rights reserved.

#include "GhostZone.h"
#include "CyberShooterPlayer.h"

AGhostZone::AGhostZone()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &AGhostZone::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AGhostZone::EndOverlap);

	// Set the collision profile to block everything but overlap ghosts
	GetStaticMeshComponent()->SetCollisionProfileName("Phantom");
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
}

void AGhostZone::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Notify the player that it has entered a ghost zone
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		player->EnterGhostZone();
	}
}

void AGhostZone::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Notify the player that it has exited a ghost zone
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		player->ExitGhostZone();
	}
}

