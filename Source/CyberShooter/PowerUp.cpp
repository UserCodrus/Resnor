// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PowerUp.h"
#include "Lock.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

APowerUp::APowerUp()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &APowerUp::BeginOverlap);

	Active = true;
	RespawnDuration = 60.0f;
	RestrictOrientation = true;

	Target = nullptr;
}

void APowerUp::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!Active)
		return;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Verify the player's orientation
		if (RestrictOrientation)
		{
			if (!player->CheckOrientation(RootComponent->GetUpVector()))
			{
				return;
			}
		}

		// Activate locks if needed
		if (Target != nullptr)
		{
			Target->TriggerLock();
		}

		// Collect the powerup and start the respawn timer
		Collect(player);
		if (RespawnDuration > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &APowerUp::Respawn, RespawnDuration);
		}

		Disable();

		if (CollectSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, CollectSound, GetActorLocation());
		}
	}
}

void APowerUp::Disable()
{
	Active = false;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void APowerUp::Respawn()
{
	// Show the power up
	Active = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	// Deactivate locks if needed
	if (Target != nullptr)
	{
		Target->UntriggerLock();
	}
}

void APowerUp::Unlock()
{
	Respawn();
}

void APowerUp::Lock()
{
	Disable();
}