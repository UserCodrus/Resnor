// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterGameInstance.h"
#include "CyberShooterSave.h"
#include "CyberShooterPlayer.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

UCyberShooterGameInstance::UCyberShooterGameInstance()
{
	SaveName = "SaveSlot";
	SaveSlot = 0;
	SaveGame = nullptr;

	Gravity = 1000.0f;
	AirFriction = 0.5f;
}

void UCyberShooterGameInstance::LoadGame()
{
	// Copy save data from the currently selected file
	SaveGame = Cast<UCyberShooterSave>(UGameplayStatics::LoadGameFromSlot(SaveName, SaveSlot));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Loading level: " + SaveGame->CurrentLevel);

	LocationID = -1;

	// Switch levels
	UGameplayStatics::OpenLevel(GetWorld(), FName(SaveGame->CurrentLevel));
}

void UCyberShooterGameInstance::SaveCheckpoint(FVector Location)
{
	// Make sure a save exists
	CreateNewSave();
		
	// Save player data
	SavePlayer();

	// Store the checkpoint
	SaveGame->CurrentLevel = GetWorld()->GetMapName();
	SaveGame->Location = Location;

	// Save the game to disk
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveName, SaveSlot);

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Checkpoint saved");
}

void UCyberShooterGameInstance::SavePlayer()
{
	// Make sure save data exists
	CreateNewSave();

	// Save player data
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player != nullptr)
	{
		SaveGame->MaxHealth = player->GetMaxHealth();
		SaveGame->MaxMomentum = player->GetMaxMomentum();
		SaveGame->TotalKeys = player->GetKeys();

		SaveGame->Weapons = player->GetWeaponSet();
		SaveGame->Abilities = player->GetAbilitySet();

		SaveGame->PlayerForward = player->GetForwardVector();
		SaveGame->PlayerUp = player->GetUpVector();

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Player data saved");
	}
}

bool UCyberShooterGameInstance::CheckHealthUpgradeCollected(int32 Slot)
{
	FLevelState* state = GetLevelState();

	if (state->HealthUpgradeCollected.Num() > Slot)
	{
		return state->HealthUpgradeCollected[Slot];
	}
	return false;
}

bool UCyberShooterGameInstance::CheckMomentumUpgradeCollected(int32 Slot)
{
	FLevelState* state = GetLevelState();

	if (state->MomentumUpgradeCollected.Num() > Slot)
	{
		return state->MomentumUpgradeCollected[Slot];
	}
	return false;
}

bool UCyberShooterGameInstance::CheckKeyCollected(int32 Slot)
{
	FLevelState* state = GetLevelState();

	if (state->KeyCollected.Num() > Slot)
	{
		return state->KeyCollected[Slot];
	}
	return false;
}

bool UCyberShooterGameInstance::CheckWeaponCollected(int32 Slot)
{
	return GetLevelState()->WeaponCollected;
}

bool UCyberShooterGameInstance::CheckAbilityCollected(int32 Slot)
{
	return GetLevelState()->AbilityCollected;
}

bool UCyberShooterGameInstance::CheckUnlocked(int32 Slot)
{
	FLevelState* state = GetLevelState();

	if (state->LockState.Num() > Slot)
	{
		return state->LockState[Slot];
	}
	return false;
}

void UCyberShooterGameInstance::CollectHealthUpgrade(int32 Slot)
{
	FLevelState* state = GetLevelState();

	// Resize the array if needed
	if (state->HealthUpgradeCollected.Num() <= Slot)
	{
		state->HealthUpgradeCollected.SetNum(Slot + 1);
	}
	
	state->HealthUpgradeCollected[Slot] = true;
}

void UCyberShooterGameInstance::CollectMomentumUpgrade(int32 Slot)
{
	FLevelState* state = GetLevelState();

	// Resize the array if needed
	if (state->MomentumUpgradeCollected.Num() <= Slot)
	{
		state->MomentumUpgradeCollected.SetNum(Slot + 1);
	}

	state->MomentumUpgradeCollected[Slot] = true;
}

void UCyberShooterGameInstance::CollectKey(int32 Slot)
{
	FLevelState* state = GetLevelState();

	// Resize the array if needed
	if (state->KeyCollected.Num() <= Slot)
	{
		state->KeyCollected.SetNum(Slot + 1);
	}

	state->KeyCollected[Slot] = true;
}

void UCyberShooterGameInstance::CollectWeapon(int32 Slot)
{
	GetLevelState()->WeaponCollected = true;
}

void UCyberShooterGameInstance::CollectAbility(int32 Slot)
{
	GetLevelState()->AbilityCollected = true;
}

void UCyberShooterGameInstance::SaveLock(int32 Slot)
{
	FLevelState* state = GetLevelState();

	// Resize the array if needed
	if (state->LockState.Num() <= Slot)
	{
		state->LockState.SetNum(Slot + 1);
	}

	state->LockState[Slot] = true;
}

UAbilityScript* UCyberShooterGameInstance::GetScript(TSubclassOf<UAbilityScript> ScriptType)
{
	// Check for existing scripts
	for (int32 i = 0; i < Scripts.Num(); ++i)
	{
		if (Scripts[i]->StaticClass() == ScriptType)
		{
			return Scripts[i];
		}
	}

	// Create a new script if one does not exist
	Scripts.Add(NewObject<UAbilityScript>(this, ScriptType));
	return Scripts.Last();
}

bool UCyberShooterGameInstance::CreateNewSave()
{
	if (SaveGame == nullptr)
	{
		// Create the save
		SaveGame = Cast<UCyberShooterSave>(UGameplayStatics::CreateSaveGameObject(UCyberShooterSave::StaticClass()));

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "New save data created");
		return true;
	}

	return false;
}

FLevelState* UCyberShooterGameInstance::GetLevelState()
{
	CreateNewSave();

	// Try to find level data in the current save game
	FString level = GetWorld()->GetMapName();
	for (int32 i = 0; i < SaveGame->LevelStatus.Num(); ++i)
	{
		if (SaveGame->LevelStatus[i].LevelName == level)
		{
			return &SaveGame->LevelStatus[i];
		}
	}

	// Create new level data
	SaveGame->LevelStatus.Emplace();
	SaveGame->LevelStatus.Last().LevelName = level;
	return &SaveGame->LevelStatus.Last();
}