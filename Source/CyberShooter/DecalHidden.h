// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "LockInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DecalHidden.generated.h"

// A decal that can be shown or hidden by locks
UCLASS()
class CYBERSHOOTER_API ADecalHidden : public AActor, public ILockInterface
{
	GENERATED_BODY()
	
public:	
	ADecalHidden();

	/// ILockInterface ///

	void Unlock() override;
	void Lock() override;

	// Swap the state of the decal
	void ToggleDecal();

protected:
	// The decal component
	UPROPERTY(Category = "Components", EditAnywhere)
		UDecalComponent* DecalComponent;
};
