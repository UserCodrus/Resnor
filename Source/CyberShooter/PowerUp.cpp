// Copyright © 2020 Brian Faubion. All rights reserved.

#include "PowerUp.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

APowerUp::APowerUp()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &APowerUp::BeginOverlap);

	Active = true;
	RespawnDuration = 60.0f;
	CanRespawn = true;
	RestrictOrientation = true;
}

void APowerUp::BeginPlay()
{
	Super::BeginPlay();

	if (!Active)
	{
		Disable();
	}
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

		// Collect the powerup and start the respawn timer
		Collect(player);
		TriggerCollectEvent();
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
	if (CanRespawn)
	{
		// Show the power up
		Active = true;
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);

		// Call blueprint events
		TriggerRespawnEvent();
	}
}

void APowerUp::TriggerCollectEvent()
{
	if (OnCollect.IsBound())
	{
		OnCollect.Broadcast();
	}
	else
	{
		// Call parent delegates
		APowerUp* parent = Cast<APowerUp>(GetAttachParentActor());
		if (parent != nullptr)
		{
			parent->TriggerCollectEvent();
		}
	}
}

void APowerUp::TriggerRespawnEvent()
{
	if (OnRespawn.IsBound())
	{
		OnRespawn.Broadcast();
	}
	else
	{
		// Call parent delegates
		APowerUp* parent = Cast<APowerUp>(GetAttachParentActor());
		if (parent != nullptr)
		{
			parent->TriggerRespawnEvent();
		}
	}
}