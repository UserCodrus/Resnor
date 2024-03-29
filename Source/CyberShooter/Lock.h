// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lock.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLockEvent);

// An actor that calls a delegate after being unlocked, and saves its lock state when the game is saved
UCLASS(BlueprintType, Blueprintable)
class CYBERSHOOTER_API ALock : public AActor
{
	GENERATED_BODY()
	
public:	
	ALock();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/// Blueprint Events ///

	// Called when the lock is finally unlocked
	UPROPERTY(BlueprintAssignable)
		FLockEvent OnUnlock;

	/// Lock Functions ///

	// Attempt to unlock this lock
	UFUNCTION(BlueprintCallable)
		void Unlock();
	// Re-lock the lock
	UFUNCTION(BlueprintCallable)
		void Lock();

protected:
	/// Properties ///
	
	// Set to true when the lock is in a locked state
	UPROPERTY(Category = "Lock", VisibleAnywhere, BlueprintReadOnly)
		bool Unlocked;
	// The number of times the lock has been unlocked
	UPROPERTY(Category = "Lock", VisibleAnywhere, BlueprintReadOnly)
		int32 UnlockCounter;
	// The number of times the unlock function must be called to actually unlock the lock
	UPROPERTY(Category = "Lock", EditAnywhere, BlueprintReadOnly)
		int32 RequiredUnlocks;
	// The ID of the lock used when saving
	UPROPERTY(Category = "Lock", EditAnywhere)
		int32 ID;
};
