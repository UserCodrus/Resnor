// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CyberShooterHUD.generated.h"

// The core game HUD
UCLASS()
class CYBERSHOOTER_API ACyberShooterHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	ACyberShooterHUD();

	/// AHUD ///

	virtual void DrawHUD() override;
	virtual void BeginPlay() override;

	/// Menu Operations ///

	bool OpenPauseMenu();
	void ClosePauseMenu();
	bool OpenWeaponMenu();
	void CloseWeaponMenu();
	bool OpenAbilityMenu();
	void CloseAbilityMenu();

	// Rebuild the HUD to reflect status changes
	void RebuildHUD();

protected:
	/// Utility Functions ///

	// Create a widget if one does not exist yet
	void SafeCreateWidget(UClass* WidgetClass, UUserWidget*& Widget);
	void SafeCreateWidget(UClass* WidgetClass, class URadialMenuWidget*& Widget);
	void SafeCreateWidget(UClass* WidgetClass, class UUIWidget*& Widget);
	// Hide every menu to prevent them from stacking up
	void HideAllMenus();

	/// Properties ///

	// The main game hud
	UPROPERTY(Category = Widgets, EditDefaultsOnly)
		TSubclassOf<class UUIWidget> GameHUD;
	UUIWidget* GameHUDWidget;
	// The pause menu widget
	UPROPERTY(Category = Widgets, EditDefaultsOnly)
		TSubclassOf<class UUserWidget> PauseMenu;
	UUserWidget* PauseWidget;
	// The weapon radial menu widget
	UPROPERTY(Category = Widgets, EditDefaultsOnly)
		TSubclassOf<class URadialMenuWidget> WeaponMenu;
	URadialMenuWidget* WeaponWidget;
	// The ability radial menu widget
	UPROPERTY(Category = Widgets, EditDefaultsOnly)
		TSubclassOf<class URadialMenuWidget> AbilityMenu;
	URadialMenuWidget* AbilityWidget;

	// Set to true to draw debug text
	UPROPERTY(EditAnywhere)
		bool DrawDebug;
};
