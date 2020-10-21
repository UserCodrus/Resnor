// Copyright © 2020 Brian Faubion. All rights reserved.

#include "AggroZone.h"

#include "CyberShooterPlayer.h"
#include "CyberShooterEnemy.h"

#include "Components/BoxComponent.h"

AAggroZone::AAggroZone()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &AAggroZone::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AAggroZone::EndOverlap);

	// Set up the overlap box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName("OverlapAll");
	RootComponent = CollisionBox;
}

void AAggroZone::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		for (int32 i = 0; i < Enemies.Num(); ++i)
		{
			Enemies[i]->Aggro();
		}
	}
}

void AAggroZone::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		for (int32 i = 0; i < Enemies.Num(); ++i)
		{
			Enemies[i]->DeAggro();
		}
	}
}