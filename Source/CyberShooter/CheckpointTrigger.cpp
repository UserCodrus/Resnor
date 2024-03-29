// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CheckpointTrigger.h"
#include "CyberShooterGameInstance.h"
#include "CyberShooterPlayer.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

ACheckpointTrigger::ACheckpointTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &ACheckpointTrigger::BeginOverlap);

	// Create the root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->Mobility = EComponentMobility::Static;
	RootComponent->bVisualizeComponent = true;

#if WITH_EDITOR
	// Create an orientation arrow for the editor
	OrientationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	OrientationArrow->SetupAttachment(RootComponent);
	OrientationArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	OrientationArrow->SetUsingAbsoluteScale(true);
	OrientationArrow->ArrowColor = FColor::Yellow;
	OrientationArrow->bIsEditorOnly = true;
#endif
}

void ACheckpointTrigger::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Save the game and refill stats if the player overlaps the trigger
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Don't save if the player hasn't moved or is in a different orientation
		if (player->HasMoved() && player->CheckOrientation(RootComponent->GetUpVector()))
		{
			SaveGame();
			player->Refill();
		}
	}
}

void ACheckpointTrigger::SaveGame()
{
	UCyberShooterGameInstance* instance = Cast<UCyberShooterGameInstance>(GetWorld()->GetGameInstance());
	if (instance != nullptr)
	{
		instance->SaveCheckpoint(GetActorLocation());
	}
}