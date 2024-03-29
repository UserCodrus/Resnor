// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialMenuWidget.generated.h"

class ACyberShooterPlayerController;

// A widget class used to create radial menus
UCLASS()
class CYBERSHOOTER_API URadialMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URadialMenuWidget(const FObjectInitializer& ObjectInitializer);

	// Refresh the radial menu to display the user's inventory
	UFUNCTION(BlueprintNativeEvent)
		void Refresh();
	// Assign the widget to a controller
	void SetOwner(APlayerController* Owner);

protected:
	// The number of segments in the radial menu
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 Segments;
	// The currently highlighted segment
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 Index;
	// The controller that the widget is tied to
	UPROPERTY(BlueprintReadOnly)
		ACyberShooterPlayerController* Controller;
};
