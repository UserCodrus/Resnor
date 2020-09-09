// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterPlayerController.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterHUD.h"
#include "Weapon.h"

#include "Engine/Engine.h"

const FName ACyberShooterPlayerController::FireForwardBinding("FireForward");
const FName ACyberShooterPlayerController::FireRightBinding("FireRight");

ACyberShooterPlayerController::ACyberShooterPlayerController()
{
	// Set defaults
	TimeDilation = 0.1f;
	MenuOpen = false;
	TransitionActive = false;
}

void ACyberShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Menu", IE_Pressed, this, &ACyberShooterPlayerController::OpenPauseMenu).bExecuteWhenPaused = true;
	InputComponent->BindAction("WeaponSelect", IE_Pressed, this, &ACyberShooterPlayerController::OpenWeaponSelect);
	InputComponent->BindAction("WeaponSelect", IE_Released, this, &ACyberShooterPlayerController::CloseWeaponSelect);
	InputComponent->BindAction("AbilitySelect", IE_Pressed, this, &ACyberShooterPlayerController::OpenAbilitySelect);
	InputComponent->BindAction("AbilitySelect", IE_Released, this, &ACyberShooterPlayerController::CloseAbilitySelect);

	InputComponent->BindAxis(FireForwardBinding).bExecuteWhenPaused = true;
	InputComponent->BindAxis(FireRightBinding).bExecuteWhenPaused = true;
}

/// Menu Interface ///

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
					player->StopAction();
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
				MenuOpen = true;
				SetTimeDilation();
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
		MenuOpen = false;
		SetTimeDilation();

		ACyberShooterHUD* hud = Cast<ACyberShooterHUD>(GetHUD());
		if (hud != nullptr)
		{
			hud->CloseWeaponMenu();
		}

		// Select a weapon
		ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(GetPawn());
		if (player != nullptr)
		{
			int32 slots = GetWeaponSlots();
			player->SelectWeapon(slots * GetRadialAngle());
		}
	}
}

void ACyberShooterPlayerController::OpenAbilitySelect()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "Ability Select Open");

	if (!MenuOpen)
	{
		ACyberShooterHUD* hud = Cast<ACyberShooterHUD>(GetHUD());
		if (hud != nullptr)
		{
			// Open the weapon wheel
			if (hud->OpenAbilityMenu())
			{
				MenuOpen = true;
				SetTimeDilation();
			}
		}
	}
}

void ACyberShooterPlayerController::CloseAbilitySelect()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "Ability Select Closed");

	if (MenuOpen)
	{
		// Close the weapon wheel
		MenuOpen = false;
		SetTimeDilation();

		ACyberShooterHUD* hud = Cast<ACyberShooterHUD>(GetHUD());
		if (hud != nullptr)
		{
			hud->CloseAbilityMenu();
		}

		// Select a weapon
		/*ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(GetPawn());
		if (player != nullptr)
		{
			int32 slots = GetWeaponSlots();
			player->SelectWeapon(slots * GetRadialAngle());
		}*/
	}
}

void ACyberShooterPlayerController::CloseMenus()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "Close Menus");

	SetPause(false);
	MenuOpen = false;
	SetTimeDilation();
}

bool ACyberShooterPlayerController::IsMenuOpen() const
{
	return MenuOpen;
}

void ACyberShooterPlayerController::SetCameraTransition(bool IsTransitionActive)
{
	TransitionActive = IsTransitionActive;
	SetTimeDilation();
}

float ACyberShooterPlayerController::GetTransitionSpeed() const
{
	return TimeDilation;
}

/// Player Interface ///

int32 ACyberShooterPlayerController::GetWeaponSlots()
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(GetPawn());
	if (player != nullptr)
	{
		const TArray<UWeapon*>* weapons = player->GetWeaponSet();
		return weapons->Num();
	}

	return 0;
}

TArray<UWeapon*> ACyberShooterPlayerController::GetPlayerWeapons()
{
	TArray<UWeapon*> weapons;

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(GetPawn());
	if (player != nullptr)
	{
		const TArray<UWeapon*>* player_weapons = player->GetWeaponSet();
		weapons = *player_weapons;
	}

	return weapons;
}

float ACyberShooterPlayerController::GetRadialAngle()
{
	// Get the rotation corresponding to the 
	FVector input(GetInputAxisValue(FireRightBinding), -GetInputAxisValue(FireForwardBinding), 0.0f);
	FRotator rotation = input.Rotation().Clamp();

	// Return the normalized angle
	return rotation.Yaw / 360.0f;
}

FVector ACyberShooterPlayerController::GetFireDirection()
{
	return FVector(GetInputAxisValue(FireForwardBinding), GetInputAxisValue(FireRightBinding), 0.0f);
}

void ACyberShooterPlayerController::SetTimeDilation()
{
	if (MenuOpen || TransitionActive)
	{
		GetWorld()->GetWorldSettings()->TimeDilation = TimeDilation;
	}
	else
	{
		GetWorld()->GetWorldSettings()->TimeDilation = 1.0f;
	}
}