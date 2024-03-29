// Copyright © 2020 Brian Faubion. All rights reserved.

#include "RadialMenuWidget.h"
#include "CyberShooterPlayerController.h"

URadialMenuWidget::URadialMenuWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	Segments = 8;
	Index = 0;
	Controller = nullptr;
}

void URadialMenuWidget::Refresh_Implementation()
{
	// Nothing
}

void URadialMenuWidget::SetOwner(APlayerController* Owner)
{
	if (Controller == nullptr)
	{
		Controller = Cast<ACyberShooterPlayerController>(Owner);
	}
}