// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CyberShooterPlayerController.generated.h"

// The main player controller
UCLASS()
class CYBERSHOOTER_API ACyberShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACyberShooterPlayerController();

	/// Menus ///

	// Open the pause menu
	void OpenPauseMenu();
	// Open the weapon select wheel
	void OpenWeaponSelect();
	// Close the weapon select wheel
	void CloseWeaponSelect();
	// Close all open menus
	void CloseMenus();

	// Returns true if a radial menu is open
	bool IsMenuOpen();

protected:
	/// APlayerController ///

	virtual void SetupInputComponent() override;

	// The game speed while a radial menu is open
	UPROPERTY(Category = Gameplay, EditAnywhere)
		float MenuSpeed;

	// Set to true when a menu is opened
	bool MenuOpen;
};
