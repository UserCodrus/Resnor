// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AggroInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAggroInterface : public UInterface
{
	GENERATED_BODY()
};

// An interface for objects that are triggered by aggro zones
class CYBERSHOOTER_API IAggroInterface
{
	GENERATED_BODY()

public:
	// Increase the aggro level
	virtual void Aggro() = 0;
	// Decrease aggro level
	virtual void EndAggro() = 0;
	// Returns true if the object has a sufficient aggro level to activate
	virtual bool IsAggro() = 0;

	// Called when the object enters its aggro zone
	virtual void EnterAggro() {};
	// Called when the object leaves its aggro zone
	virtual void ExitAggro() {};

	// Called when the aggro zone resets
	virtual void AggroReset() = 0;
	// Called when the aggro zone is disabled
	virtual void AggroDisable() = 0;

	// Register a parent aggro zone
	virtual void RegisterZone(class AAggroZone* Zone) = 0;
};
