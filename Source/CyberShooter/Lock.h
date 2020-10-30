// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lock.generated.h"

// An actor that can control doors and other gadgets by locking and unlocking via switches
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ALock : public AActor
{
	GENERATED_BODY()
	
public:	
	ALock();

	/// Lock Functions ///

	// Returns true if the lock is currently locked
	UFUNCTION(BlueprintPure)
		bool IsLocked() const { return Locked; }

	// Called when the lock is triggered by a switch or other gadget
	UFUNCTION(BlueprintCallable)
		bool TriggerLock();
	// Called when a trigger is reversed
	UFUNCTION(BlueprintCallable)
		bool UntriggerLock();

protected:
	// Unlock the lock's target
	void Unlock();
	// Re-lock the lock's target
	void Lock();

	/// Blueprint Events ///
	
	// Called when the lock is unlocked
	UFUNCTION(BlueprintImplementableEvent)
		void OnUnlock();
	// Called when the lock is re-locked
	UFUNCTION(BlueprintImplementableEvent)
		void OnLock();

	/// Properties ///
	
	// The target actor that is activated by this lock
	UPROPERTY(Category = "Lock|Trigger", EditInstanceOnly, BlueprintReadOnly, meta = (MustImplement = "LockInterface"))
		TArray<AActor*> Targets;

	// The number of triggers used on the lock so far
	UPROPERTY(Category = "Lock|Trigger", EditAnywhere, BlueprintReadOnly)
		int32 NumTriggers;
	// The number of triggers required to open this lock
	UPROPERTY(Category = "Lock|Trigger", EditAnywhere, BlueprintReadOnly)
		int32 RequiredTriggers;
	// If set to true, the lock can not be unlocked after being locked
	UPROPERTY(Category = "Lock|Trigger", EditAnywhere, BlueprintReadOnly)
		bool PermanentUnlock;
	// If set to true, the lock will call Lock() on targets when it unlocks, and vice versa
	UPROPERTY(Category = "Lock|Trigger", EditAnywhere, BlueprintReadOnly)
		bool Invert;

	// The sound played when the switch triggers
	UPROPERTY(Category = "Lock|Sound", EditAnywhere)
		USoundBase* UnlockSound;
	// The sound played when the switch untriggers
	UPROPERTY(Category = "Lock|Sound", EditAnywhere)
		USoundBase* LockSound;

	// Set to true when the lock is locked
	UPROPERTY(VisibleAnywhere)
		bool Locked;
};
