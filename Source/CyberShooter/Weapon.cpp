// Copyright © 2020 Brian Faubion. All rights reserved.

#include "Weapon.h"

UWeapon::UWeapon()
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

	FireRate = 0.5;
	FireAccuracy = 0.0f;
	NumBullets = 1;
	BulletOffset = 15.0f;
	Recoil = 0.0f;
	FanBullets = true;
}