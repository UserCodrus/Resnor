// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrientationTrigger.generated.h"

#if WITH_EDITORONLY_DATA
class UArrowComponent;
#endif

// A gadget that changes the orientation of pawns and physics objects
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API AOrientationTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AOrientationTrigger();

	virtual void BeginPlay() override;

	/// Blueprint Events ///

	// Called when the trigger is activated
	UFUNCTION(BlueprintImplementableEvent)
		void ActivateTrigger();
	// Called when the trigger is deactivated
	UFUNCTION(BlueprintImplementableEvent)
		void DeactivateTrigger();

	/// Orientation Trigger Functions ///

	// Activate the trigger
	UFUNCTION(BlueprintCallable)
		void Activate();
	// Deactivate the trigger
	UFUNCTION(BlueprintCallable)
		void Deactivate();
	// Apply the trigger to all its targets
	UFUNCTION(BlueprintCallable)
		void Trigger();

protected:
	// Set an actor's orientation when it overlaps the trigger
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// Change the orientation of an actor
	void ChangeOrientation(AActor* Target);

	/// Properties ///

	// If set to false, the trigger will not activate when an actor hits it
	UPROPERTY(Category = "Orientation", EditAnywhere)
		bool Active;
	// If set to true, the trigger will only work if the player's up vector matches the RequiredOrientationComponent
	UPROPERTY(Category = "Orientation", EditAnywhere)
		bool RestrictOrientation;

	// The targets that will manually activate when unlocked
	UPROPERTY(Category = "Locking", EditInstanceOnly, BlueprintReadOnly, meta = (MustImplement = "OrientationInterface"))
		TArray<AActor*> Targets;

	// The component that controls the orientation
	UPROPERTY(Category = "Components", EditAnywhere)
		USceneComponent* OrientationComponent;

#if WITH_EDITORONLY_DATA
	// Arrow indicating the forward orientation vector
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* OrientationForwardArrow;
	// Arrow indicating the up orientation vector
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* OrientationUpArrow;
	// Arrow indicating the required orientation
	UPROPERTY(Category = "Components", EditAnywhere)
		UArrowComponent* RequiredArrow;
#endif
};
