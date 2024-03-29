// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterHUD.h"
#include "CyberShooterPlayer.h"
#include "CyberShooterPlayerController.h"
#include "UIWidget.h"
#include "RadialMenuWidget.h"

#include "Blueprint/UserWidget.h"

ACyberShooterHUD::ACyberShooterHUD()
{
	GameHUDWidget = nullptr;
	PauseWidget = nullptr;
	WeaponWidget = nullptr;
	AbilityWidget = nullptr;

	DrawDebug = true;
}

/// AHUD ///

void ACyberShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(GetOwningPlayerController()->GetPawn());
	if (player != nullptr)
	{
		// Refresh the main HUD widget
		if (GameHUDWidget != nullptr)
		{
			GameHUDWidget->Health = player->GetHealth();
			GameHUDWidget->Momentum = player->GetMomentum();
			GameHUDWidget->Armor = player->GetArmor();
			GameHUDWidget->Refresh();
		}

		// Draw debug text
		if (DrawDebug)
		{
			int32 x, y;
			GetOwningPlayerController()->GetViewportSize(x, y);

			// HP and MP
			//DrawText("HP " + FString::FromInt(player->GetHealth()) + " / " + FString::FromInt(player->GetMaxHealth()), FLinearColor::White, x - 90, 20.0f);
			//DrawText("MO " + FString::FromInt(player->GetMomentum()) + " / " + FString::FromInt(player->GetMaxMomentum()), FLinearColor::White, x - 90, 40.0f);

			// Velocity
			FVector velocity = player->GetVelocity();
			DrawText("VELX " + FString::FromInt((int32)velocity.X), FLinearColor::White, x - 90, 80.0f);
			DrawText("VELY " + FString::FromInt((int32)velocity.Y), FLinearColor::White, x - 90, 100.0f);
			DrawText("VELZ " + FString::FromInt((int32)velocity.Z), FLinearColor::White, x - 90, 120.0f);
			DrawText("VEL " + FString::FromInt((int32)velocity.Size()), FLinearColor::White, x - 90, 140.0f);

			// Radial input
			ACyberShooterPlayerController* controller = Cast<ACyberShooterPlayerController>(GetOwningPlayerController());
			if (controller != nullptr)
			{
				DrawText("RAD " + FString::SanitizeFloat(controller->GetRadialAngle()), FLinearColor::White, x - 90, 180.0f);
			}
		}
	}
}

void ACyberShooterHUD::BeginPlay()
{
	Super::BeginPlay();

	// Create the main HUD
	SafeCreateWidget(GameHUD, GameHUDWidget);
}

/// Menu Operations ///

bool ACyberShooterHUD::OpenPauseMenu()
{
	SafeCreateWidget(PauseMenu, PauseWidget);
	if (PauseWidget != nullptr)
	{
		HideAllMenus();
		PauseWidget->SetVisibility(ESlateVisibility::Visible);
		return true;
	}

	return false;
}

void ACyberShooterHUD::ClosePauseMenu()
{
	if (PauseWidget != nullptr)
	{
		PauseWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool ACyberShooterHUD::OpenWeaponMenu()
{
	SafeCreateWidget(WeaponMenu, WeaponWidget);
	if (WeaponWidget != nullptr)
	{
		HideAllMenus();
		WeaponWidget->Refresh();
		WeaponWidget->SetVisibility(ESlateVisibility::Visible);
		return true;
	}

	return false;
}

void ACyberShooterHUD::CloseWeaponMenu()
{
	if (WeaponWidget != nullptr)
	{
		WeaponWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool ACyberShooterHUD::OpenAbilityMenu()
{
	SafeCreateWidget(AbilityMenu, AbilityWidget);
	if (AbilityWidget != nullptr)
	{
		HideAllMenus();
		AbilityWidget->Refresh();
		AbilityWidget->SetVisibility(ESlateVisibility::Visible);
		return true;
	}

	return false;
}

void ACyberShooterHUD::CloseAbilityMenu()
{
	if (AbilityWidget != nullptr)
	{
		AbilityWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ACyberShooterHUD::RebuildHUD()
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(GetOwningPlayerController()->GetPawn());
	if (player != nullptr)
	{
		if (GameHUDWidget != nullptr)
		{
			// Load the player's current stats into the UI
			GameHUDWidget->MaxHealth = player->GetMaxHealth();
			GameHUDWidget->MaxMomentum = player->GetMaxMomentum();

			GameHUDWidget->Rebuild();
		}
	}
}

/// Utility Functions ///

void ACyberShooterHUD::SafeCreateWidget(UClass* WidgetClass, UUserWidget*& Widget)
{
	if (WidgetClass != nullptr && Widget == nullptr)
	{
		Widget = CreateWidget<UUserWidget>(GetOwningPlayerController(), WidgetClass);
		if (Widget != nullptr)
		{
			Widget->AddToViewport();
		}
	}
}

void ACyberShooterHUD::SafeCreateWidget(UClass* WidgetClass, class URadialMenuWidget*& Widget)
{
	if (WidgetClass != nullptr && Widget == nullptr)
	{
		Widget = CreateWidget<URadialMenuWidget>(GetOwningPlayerController(), WidgetClass);
		if (Widget != nullptr)
		{
			Widget->AddToViewport();
			Widget->SetOwner(GetOwningPlayerController());
		}
	}
}

void ACyberShooterHUD::SafeCreateWidget(UClass* WidgetClass, class UUIWidget*& Widget)
{
	if (WidgetClass != nullptr && Widget == nullptr)
	{
		Widget = CreateWidget<UUIWidget>(GetOwningPlayerController(), WidgetClass);
		if (Widget != nullptr)
		{
			Widget->AddToViewport();

			// Fill in player stats
			ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(GetOwningPlayerController()->GetPawn());
			if (player != nullptr)
			{
				Widget->MaxHealth = player->GetMaxHealth();
				Widget->MaxMomentum = player->GetMaxMomentum();

				Widget->MomentumBarSize = player->GetMomentumBlockSize();
			}
			Widget->Rebuild();
		}
	}
}

void ACyberShooterHUD::HideAllMenus()
{
	ClosePauseMenu();
	CloseWeaponMenu();
	CloseAbilityMenu();
}