// Copyright © 2020 Brian Faubion. All rights reserved.

#include "AggroZone.h"

#include "CyberShooterPlayer.h"
#include "CyberShooterEnemy.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

AAggroZone::AAggroZone()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &AAggroZone::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AAggroZone::EndOverlap);

	// Set up the overlap box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName("OverlapAll");
	CollisionBox->SetBoxExtent(FVector(50.0f));
	RootComponent = CollisionBox;

	Active = true;
}

void AAggroZone::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!Active)
		return;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		for (int32 i = 0; i < Enemies.Num(); ++i)
		{
			if (Enemies[i] != nullptr)
			{
				Enemies[i]->Aggro();
			}
		}
	}
}

void AAggroZone::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!Active)
		return;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		for (int32 i = 0; i < Enemies.Num(); ++i)
		{
			if (Enemies[i] != nullptr)
			{
				Enemies[i]->EndAggro();
			}
		}
	}
}

void AAggroZone::UpdateEnemyList()
{
	// Get all the enemies the aggro zone overlaps
	TArray<TEnumAsByte<EObjectTypeQuery>> types;
	TArray<AActor*> ignore;
	TArray<AActor*> actors;
	UKismetSystemLibrary::BoxOverlapActors(GetWorld(), CollisionBox->GetComponentLocation(), CollisionBox->GetScaledBoxExtent(), types, AEnemyBase::StaticClass(), ignore, actors);

	// Add overlapped enemies to the zone
	Enemies.Empty();
	for (int32 i = 0; i < actors.Num(); ++i)
	{
		AEnemyBase* enemy = Cast<AEnemyBase>(actors[i]);
		if (enemy != nullptr)
		{
			Enemies.Add(enemy);
		}
	}
}

/// ILockInterface ///

void AAggroZone::Unlock()
{
	ClearComponentOverlaps();
	Active = true;
	Execute_OnUnlock(this);

	// Update overlaps to activate the aggro zone if the player is inside it
	UpdateOverlaps();
}

void AAggroZone::Lock()
{
	Active = false;
	Execute_OnLock(this);
}