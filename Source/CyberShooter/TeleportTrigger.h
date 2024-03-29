// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportTrigger.generated.h"

class IOrientationInterface;

#if WITH_EDITORONLY_DATA
	class UArrowComponent;
#endif

// The base class for all teleporters and teleport destinations
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ATeleportBase : public AActor
{
	GENERATED_BODY()

public:
	ATeleportBase();

	// Change the orientation of a player to match this component
	void OrientPlayer(IOrientationInterface* Player);

protected:
	// If set to true, the player's orientation will change after teleporting
	UPROPERTY(Category = "TeleportTarget", EditAnywhere)
		bool SetOrientation;

#if WITH_EDITORONLY_DATA
	// Arrow indicating the forward orientation vector
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* OrientationForwardArrow;
	// Arrow indicating the up orientation vector
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* OrientationUpArrow;
#endif
};

// A trigger that teleports the player to another trigger or a teleport target
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ATeleportTrigger : public ATeleportBase
{
	GENERATED_BODY()
	
public:	
	ATeleportTrigger();

	virtual void BeginPlay() override;

	// Enable the teleporter
	UFUNCTION(BlueprintCallable)
		void Activate();
	// Disable the teleporter
	UFUNCTION(BlueprintCallable)
		void Deactivate();

	/// Blueprint Events ///

	// Called when the trigger is activated
	UFUNCTION(BlueprintImplementableEvent)
		void ActivateTrigger();
	// Called when the trigger is deactivated
	UFUNCTION(BlueprintImplementableEvent)
		void DeactivateTrigger();

protected:
	// Teleport the player when it overlaps the trigger
	UFUNCTION()
		virtual void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// If set to false, the trigger will not activate when an actor hits it
	UPROPERTY(Category = "Teleporter", EditAnywhere)
		bool Active;
	// The teleport target that the player will teleport to
	UPROPERTY(Category = "Teleporter", EditInstanceOnly)
		ATeleportBase* Target;
	// If set to true the player will require the same up vector as the teleporter to use it
	UPROPERTY(Category = "Teleporter", EditAnywhere)
		bool RestrictOrientation;
};