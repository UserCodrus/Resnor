// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Ability.h"
#include "CyberShooterPlayer.h"

UAbility::UAbility()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> texture(TEXT("Texture2D'/Game/UI/Textures/Icons/IconX.IconX'"));
	if (texture.Succeeded())
	{
		Icon = texture.Object;
	}
	else
	{
		Icon = nullptr;
	}

	Script = nullptr;
	Cooldown = 0.0f;
}