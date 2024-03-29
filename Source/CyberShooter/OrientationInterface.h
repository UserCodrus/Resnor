// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OrientationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOrientationInterface : public UInterface
{
	GENERATED_BODY()
};

// An interface for objects that can change orientation
class CYBERSHOOTER_API IOrientationInterface
{
	GENERATED_BODY()

public:
	// Set the orientation of the object
	UFUNCTION()
		virtual bool SetOrientation(FVector NewForward, FVector NewUp) = 0;
	// Verify that the orientation of the object matches the provided up vector
	UFUNCTION()
		virtual bool CheckOrientation(FVector DesiredUp) = 0;
};
