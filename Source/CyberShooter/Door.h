// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalStaticMesh.h"
#include "Door.generated.h"

// A barrier that can be removed using lock actors
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ADoor : public APhysicalStaticMesh
{
	GENERATED_BODY()
	
public:
	ADoor();

protected:
	virtual void BeginPlay() override;

public:

	/// Door Functions ///
	
	// Open the door
	UFUNCTION(Category = "Door", BlueprintCallable, CallInEditor)
		void Open();
	// Close the door
	UFUNCTION(Category = "Door", BlueprintCallable, CallInEditor)
		void Close();

protected:
	/// Blueprint Events ///
	
	// Called when the door is opened
	UFUNCTION(BlueprintNativeEvent)
		void DoorOpen();
	// Called when the door is closed
	UFUNCTION(BlueprintNativeEvent)
		void DoorClose();

	/// Properties ///
	
	// Set to true after the door opens
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool Opened;

	// The sound played when the door opens
	UPROPERTY(EditAnywhere)
		USoundBase* OpenSound;
	// The sound played when the door closes
	UPROPERTY(EditAnywhere)
		USoundBase* CloseSound;
};
