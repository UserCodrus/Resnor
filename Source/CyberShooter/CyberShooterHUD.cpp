// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterHUD.h"
#include "CyberShooterPlayer.h"

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
		// Draw debug text
		if (DrawDebug)
		{
			int32 x, y;
			GetOwningPlayerController()->GetViewportSize(x, y);

			// HP and MP
			DrawText("HP " + FString::FromInt(player->GetHealth()) + " / " + FString::FromInt(player->GetMaxHealth()), FLinearColor::White, x - 90, 20.0f);
			DrawText("MO " + FString::FromInt(player->GetMomentum()) + " / " + FString::FromInt(player->GetMaxMomentum()), FLinearColor::White, x - 90, 40.0f);

			// Velocity
			FVector velocity = player->GetVelocity();
			DrawText("VEL " + FString::FromInt((int32)FVector(velocity.X, velocity.Y, 0.0f).Size()), FLinearColor::White, x - 90, 80.0f);
			DrawText("GRA " + FString::FromInt((int32)velocity.Z), FLinearColor::White, x - 90, 100.0f);
			DrawText("TOT " + FString::FromInt((int32)velocity.Size()), FLinearColor::White, x - 90, 120.0f);
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

void ACyberShooterHUD::HideAllMenus()
{
	ClosePauseMenu();
	CloseWeaponMenu();
	CloseAbilityMenu();
}