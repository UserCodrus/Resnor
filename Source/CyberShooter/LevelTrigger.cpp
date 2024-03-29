// Copyright © 2020 Brian Faubion. All rights reserved.

#include "LevelTrigger.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

ALevelTrigger::ALevelTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &ALevelTrigger::BeginOverlap);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Arrow components for editor visualizations
#if WITH_EDITOR
	UpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	UpArrow->SetupAttachment(RootComponent);
	UpArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	UpArrow->SetUsingAbsoluteScale(true);
	UpArrow->ArrowColor = FColor::Blue;
	UpArrow->bIsEditorOnly = true;

	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
	ForwardArrow->SetupAttachment(RootComponent);
	ForwardArrow->SetUsingAbsoluteScale(true);
	ForwardArrow->ArrowColor = FColor::Red;
	ForwardArrow->bIsEditorOnly = true;
#endif

	// Set defaults
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