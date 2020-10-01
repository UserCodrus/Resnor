// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Switch.generated.h"

class ALock;

// A switch that can trigger a lock
UCLASS(Abstract)
class CYBERSHOOTER_API ASwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	ASwitch();

	// Returns true if the switch has been activated
	UFUNCTION(BlueprintPure)
		bool IsTriggered() const { return Triggered; }
	// Called when the switch timer expires after the switch is untriggered
	void TriggerTimeout();

protected:
	/// Blueprint Events ///

	// Called when the switch is activated
	UFUNCTION(BlueprintNativeEvent)
		void OnTriggered();
	// Called when the switch is deactivated
	UFUNCTION(BlueprintNativeEvent)
		void OnUntriggered();

	/// Switch Functions ///

	// Determine if a given actor can trigger this switch
	virtual bool CheckTrigger(AActor* TriggeringActor);

	// Trigger a lock when the switch activates
	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	// Reverse a trigger when the switch deactivates
	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// The lock that this switch triggers
	UPROPERTY(Category = "Switch|Trigger", EditInstanceOnly)
		ALock* Target;
	// If set to true, the switch will stay on after triggered
	UPROPERTY(Category = "Switch|Trigger", EditAnywhere)
		bool PermanentTrigger;
	// The amount of time the switch will stay triggered after being pressed
	UPROPERTY(Category = "Switch|Trigger", EditDefaultsOnly)
		float TriggerDuration;
	// The number of actors on the switch
	UPROPERTY(Category = "Switch|Trigger", VisibleAnywhere)
		int32 NumTriggers;
	// The number of actors that must be on the switch to activate it
	UPROPERTY(Category = "Switch|Trigger", VisibleAnywhere)
		int32 RequiredTriggers;

	// Set to true after the switch has been activated
	UPROPERTY(Category = "Switch|Status", VisibleAnywhere)
		bool Triggered;

	// The timer handle for the trigger timer
	FTimerHandle TimerHandle_SwitchTimer;
};

UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ASwitch_Player : public ASwitch
{
	GENERATED_BODY()

public:
	ASwitch_Player();

protected:
	virtual bool CheckTrigger(AActor* TriggeringActor);
};