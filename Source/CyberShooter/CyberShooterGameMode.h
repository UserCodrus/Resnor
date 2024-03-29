// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CyberShooterGameMode.generated.h"

UCLASS(MinimalAPI)
class ACyberShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACyberShooterGameMode();

	// Load the current save slot from the game instance
	UFUNCTION(Exec)
		void LoadGame();
	// Move to a level with a specified LocationID
	UFUNCTION(Exec)
		void LoadLevel(FString LevelName, int32 LocationID);
	// Refill the player's stats
	UFUNCTION(Exec)
		void RefillPlayer();
};



