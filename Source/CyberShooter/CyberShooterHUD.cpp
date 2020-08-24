// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterHUD.h"

#include "CyberShooterPlayer.h"

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