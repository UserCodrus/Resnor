// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CyberShooterPlayerController.generated.h"

class UWeapon;
class UAbility;

UENUM()
enum class EMenuState : uint8
{
	NONE,
	PAUSE,
	WEAPON_WHEEL,
	ABILITY_WHEEL
};

// The main player controller
UCLASS()
class CYBERSHOOTER_API ACyberShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACyberShooterPlayerController();

protected:
	virtual void SetupInputComponent() override;

public:
	/// Menu Interface ///

	// Open the pause menu
	void OpenPauseMenu();
	// Open the weapon select wheel
	void OpenWeaponSelect();
	// Close the weapon select wheel
	void CloseWeaponSelect();
	// Open the ability select wheel
	void OpenAbilitySelect();
	// Close the ability select wheel
	void CloseAbilitySelect();
	// Close all open menus
	void CloseMenus();

	// Returns true if a radial menu is open
	bool IsMenuOpen() const;

	// Activate or deactivate camera transitions
	void SetCameraTransition(bool IsTransitionActive);
	// Get the time dilation for camera transitions
	float GetTransitionSpeed() const;

	/// Player Interface ///

	// Get the number of weapons the player owns
	UFUNCTION(BlueprintCallable)
		int32 GetWeaponSlots();
	// Get the player's weapon set
	UFUNCTION(BlueprintCallable)
		TArray<UWeapon*> GetPlayerWeapons();
	// Get the number of abilities the player has
	UFUNCTION(BlueprintCallable)
		int32 GetAbilitySlots();
	// Get the player's ability set
	UFUNCTION(BlueprintCallable)
		TArray<UAbility*> GetPlayerAbilities();

	// Get the angle of the current radial selection
	UFUNCTION(BlueprintCallable)
		float GetRadialAngle();
	// Get the aiming direction
	UFUNCTION(BlueprintCallable)
		FVector GetFireDirection();

protected:
	// Activate or deactivate time dilation
	void SetTimeDilation();

	// The game speed while a radial menu is open
	UPROPERTY(Category = Gameplay, EditAnywhere)
		float TimeDilation;
	// The current menu state
	UPROPERTY(VisibleAnywhere)
		EMenuState MenuState;

	// Bindings for the radial menu
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

	// Set to true when a camera transition is happening
	bool TransitionActive;
};
