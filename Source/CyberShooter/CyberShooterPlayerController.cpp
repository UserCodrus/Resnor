// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterPlayerController.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterHUD.h"

#include "Engine/Engine.h"

ACyberShooterPlayerController::ACyberShooterPlayerController()
{
	// Set defaults
	MenuSpeed = 0.1f;
	MenuOpen = false;
}

void ACyberShooterPlayerController::OpenPauseMenu()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "Main Menu");

	ACyberShooterHUD* hud = Cast<ACyberShooterHUD>(GetHUD());
	if (hud != nullptr)
	{
		if (IsPaused())
		{
			// Close the pause menu
			CloseMenus();
			hud->ClosePauseMenu();
		}
		else
		{
			// Open the pause menu
			if (hud->OpenPauseMenu())
			{
				// Tell the player to stop firing
				ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(GetPawn());
				if (player != nullptr)
				{
					player->StopFiring();
				}

				CloseMenus();
				SetPause(true);
			}
		}
	}
}

void ACyberShooterPlayerController::OpenWeaponSelect()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "Weapon Select Open");

	if (!MenuOpen)
	{
		ACyberShooterHUD* hud = Cast<ACyberShooterHUD>(GetHUD());
		if (hud != nullptr)
		{
			// Open the weapon wheel
			if (hud->OpenWeaponMenu())
			{
				GetWorld()->GetWorldSettings()->TimeDilation = MenuSpeed;
				MenuOpen = true;
			}
		}
	}
}

void ACyberShooterPlayerController::CloseWeaponSelect()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "Weapon Select Closed");

	if (MenuOpen)
	{
		// Close the weapon wheel
		GetWorld()->GetWorldSettings()->TimeDilation = 1.0f;
		MenuOpen = false;

		ACyberShooterHUD* hud = Cast<ACyberShooterHUD>(GetHUD());
		if (hud != nullptr)
		{
			hud->CloseWeaponMenu();
		}
	}
}

void ACyberShooterPlayerController::CloseMenus()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "Close Menus");

	SetPause(false);
	GetWorld()->GetWorldSettings()->TimeDilation = 1.0f;
	MenuOpen = false;
}

bool ACyberShooterPlayerController::IsMenuOpen()
{
	return MenuOpen;
}

void ACyberShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Menu", IE_Pressed, this, &ACyberShooterPlayerController::OpenPauseMenu).bExecuteWhenPaused = true;
	InputComponent->BindAction("WeaponSelect", IE_Pressed, this, &ACyberShooterPlayerController::OpenWeaponSelect);
	InputComponent->BindAction("WeaponSelect", IE_Released, this, &ACyberShooterPlayerController::CloseWeaponSelect);
}