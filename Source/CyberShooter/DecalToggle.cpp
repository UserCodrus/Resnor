// Copyright © 2020 Brian Faubion. All rights reserved.

#include "DecalToggle.h"

#include "Components/DecalComponent.h"

ADecalToggle::ADecalToggle()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->bVisualizeComponent = true;

	ActiveDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("ActiveDecal"));
	ActiveDecal->DecalSize = FVector(50.0f);
	ActiveDecal->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	ActiveDecal->SetupAttachment(RootComponent);

	InactiveDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("InactiveDecal"));
	InactiveDecal->DecalSize = FVector(50.0f);
	InactiveDecal->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	InactiveDecal->SetupAttachment(RootComponent);

	// Set defaults
	Active = false;
}

void ADecalToggle::ToggleDecals()
{
	Active = !Active;
	UpdateDecals();
}

void ADecalToggle::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateDecals();
}

void ADecalToggle::UpdateDecals()
{
	ActiveDecal->SetVisibility(Active);
	InactiveDecal->SetVisibility(!Active);
}
