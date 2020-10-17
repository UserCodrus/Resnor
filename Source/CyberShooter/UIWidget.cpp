// Copyright © 2020 Brian Faubion. All rights reserved.

#include "UIWidget.h"

UUIWidget::UUIWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	HealthSegments = 10;
	MomentumSegments = 5;
}