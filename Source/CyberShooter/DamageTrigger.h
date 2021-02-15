// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTrigger.generated.h"

UCLASS()
class CYBERSHOOTER_API ADamageTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageTrigger();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	// Damage actors inside the trigger
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Stop damaging actors that leave the trigger
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:
	// Activate collision
	void ActivateTrigger();
	// Deactivate collision
	void DeactivateTrigger();

	// Called when the trigger activates
	UFUNCTION(BlueprintImplementableEvent)
		void OnActivate();
	// Called when the trigger deactivates
	UFUNCTION(BlueprintImplementableEvent)
		void OnDeactivate();

	// Actors that are currently inside the trigger
	UPROPERTY(VisibleAnywhere)
		TArray<AActor*> Targets;

	// The damage that actors will take when inside the trigger
	UPROPERTY(EditAnywhere)
		int32 Damage;

	// If set to true, the trigger will cycle between the active and inactive state over time
	UPROPERTY(EditAnywhere)
		bool AutoCycle;
	// Set to true when the trigger is active
	UPROPERTY(EditAnywhere)
		bool Active;
	// The amount of time in seconds that the trigger will be active
	UPROPERTY(EditAnywhere)
		float ActiveDuration;
	// The amount of time in seconds that the trigger will be dormant
	UPROPERTY(EditAnywhere)
		float InactiveDuration;
	// The offset that the timer starts at to ensure triggers alternate activity
	UPROPERTY(EditAnywhere)
		float TimerOffset;

	// The timer for managing the trigger
	float Timer;
};
