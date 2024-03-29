// Copyright © 2020 Brian Faubion. All rights reserved.

#include "DecalHidden.h"

#include "Components/DecalComponent.h"

ADecalHidden::ADecalHidden()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->bVisualizeComponent = true;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->DecalSize = FVector(50.0f);
	DecalComponent->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	DecalComponent->SetupAttachment(RootComponent);
}

void ADecalHidden::ToggleDecal()
{
	DecalComponent->SetVisibility(!DecalComponent->IsVisible());
}
