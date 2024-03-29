// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "Ability.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CyberShooterGameInstance.generated.h"

// The game instance
UCLASS(Blueprintable)
class CYBERSHOOTER_API UCyberShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UCyberShooterGameInstance();

	/// Accessors ///
	
	FORCEINLINE const class UCyberShooterSave* GetSaveData() const { return SaveGame; };
	FORCEINLINE float GetGravity() const { return Gravity; }
	FORCEINLINE float GetAirFriction() const { return AirFriction; }

	// Load the current save data
	void LoadGame();

	// Save the current checkpoint and then save data to a file
	void SaveCheckpoint(FVector Location);
	// Save the player's status
	void SavePlayer();

	// Check if a health upgrade has been collected in the current level
	UFUNCTION(BlueprintPure)
		bool CheckHealthUpgradeCollected(int32 Slot);
	// Check if a momentum upgrade has been collected in the current level
	UFUNCTION(BlueprintPure)
		bool CheckMomentumUpgradeCollected(int32 Slot);
	// Check if a data key has been collected in the current level
	UFUNCTION(BlueprintPure)
		bool CheckKeyCollected(int32 Slot);
	// Check if the weapon in the current level has been collected
	UFUNCTION(BlueprintPure)
		bool CheckWeaponCollected(int32 Slot);
	// Check if the ability in the current level has been collected
	UFUNCTION(BlueprintPure)
		bool CheckAbilityCollected(int32 Slot);
	// Check if a lock has been unlocked
	UFUNCTION(BlueprintPure)
		bool CheckUnlocked(int32 Slot);

	// Mark a health upgrade as collected
	UFUNCTION(BlueprintCallable)
		void CollectHealthUpgrade(int32 Slot);
	// Mark a momentum upgrade as collected
	UFUNCTION(BlueprintCallable)
		void CollectMomentumUpgrade(int32 Slot);
	// Mark a key as collected
	UFUNCTION(BlueprintCallable)
		void CollectKey(int32 Slot);
	// Mark a weapon as collected
	UFUNCTION(BlueprintCallable)
		void CollectWeapon(int32 Slot);
	// Mark an ability as collected
	UFUNCTION(BlueprintCallable)
		void CollectAbility(int32 Slot);
	// Mark a lock as unlocked
	UFUNCTION(BlueprintCallable)
		void SaveLock(int32 Slot);

	// Retrieve an ability script
	UAbilityScript* GetScript(TSubclassOf<UAbilityScript> ScriptType);

	// The ID of the trigger that sent the player to the current level, set to -1 when the game is loaded from a save
	UPROPERTY(Category = "Game", VisibleAnywhere)
		int32 LocationID;

protected:
	// Create new save data to store information
	bool CreateNewSave();
	// Get a reference to the current level state, or create a new one if one does not exist
	struct FLevelState* GetLevelState();

	// The save slot name
	UPROPERTY(Category = "Save Data", EditDefaultsOnly)
		FString SaveName;
	// The current save slot
	UPROPERTY(Category = "Save Data", EditInstanceOnly)
		int32 SaveSlot;
	// The current save state of the game
	UPROPERTY(Category = "Save Data", EditInstanceOnly)
		class UCyberShooterSave* SaveGame;

	// The current world gravity
	UPROPERTY(Category = "Physics", EditDefaultsOnly)
		float Gravity;
	// The current world air friction
	UPROPERTY(Category = "Physics", EditDefaultsOnly)
		float AirFriction;

	// The ability scripts used by the ability system
	UPROPERTY(Category = "Scripts", VisibleAnywhere)
		TArray<UAbilityScript*> Scripts;
};
