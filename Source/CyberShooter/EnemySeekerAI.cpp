// Copyright © 2020 Brian Faubion. All rights reserved.

#include "EnemySeekerAI.h"
#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"

AEnemySeekerAI::AEnemySeekerAI()
{
	EnemyPawn = nullptr;
}

void AEnemySeekerAI::BeginPlay()
{
	Super::BeginPlay();

	EnemyPawn = Cast<AEnemySeeker>(GetPawn());
	Player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AEnemySeekerAI::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (EnemyPawn != nullptr && Player != nullptr)
	{
		// Determine if the player is within range of the seeker
		FVector diff = Player->GetActorLocation() - EnemyPawn->GetActorLocation();
		if (diff.SizeSquared() < EnemyPawn->GetAggroDistance() * EnemyPawn->GetAggroDistance())
		{
			EnemyPawn->AddControlInput(diff);
		}
	}
}