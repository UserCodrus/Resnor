// Copyright © 2020 Brian Faubion. All rights reserved.

#include "LevelTrigger.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

ALevelTrigger::ALevelTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &ALevelTrigger::BeginOverlap);

	TargetLevel = NAME_None;
	TriggerID = 0;
}

void ALevelTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Ignore null triggers
	if (TargetLevel.IsNone())
		return;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Ignore the trigger if the player is loading into the level for the first time
		if (player->HasMoved())
		{
			// Change the location ID to match this trigger
			UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
			if (instance != nullptr)
			{
				instance->LocationID = TriggerID;
				instance->SavePlayer();
			}

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Loading level: " + TargetLevel.ToString());

			// Switch levels
			UGameplayStatics::OpenLevel(GetWorld(), TargetLevel);
		}
	}
}