// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "Breakable.h"

#include "CoreMinimal.h"
#include "PhysicalStaticMesh.h"
#include "PhysicsObject.generated.h"

class UPhysicsMovementComponent;

// A static mesh object with physics enabled
UCLASS()
class CYBERSHOOTER_API APhysicsObject : public APhysicalStaticMesh, public IBreakable
{
	GENERATED_BODY()
	
public:
	APhysicsObject();

	/// IBreakable Interface ///
	
	virtual void Impulse(FVector Force) override;

protected:
	/// Properties ///
	
	// The movement component that handles physics
	UPROPERTY(Category = "Components", VisibleAnywhere)
		UPhysicsMovementComponent* MovementComponent;
};
