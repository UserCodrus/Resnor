// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LockInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULockInterface : public UInterface
{
	GENERATED_BODY()
};

// An interface for objects that can interact with lock actors
class CYBERSHOOTER_API ILockInterface
{
	GENERATED_BODY()

public:
	// Called when the lock unlocks
	UFUNCTION()
		virtual void Unlock() = 0;
	// Called when the lock re-locks
	UFUNCTION()
		virtual void Lock() = 0;
};
