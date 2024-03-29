// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BulletMovementComponent.generated.h"

// The movement component for projectile weapons
UCLASS()
class CYBERSHOOTER_API UBulletMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()
	
public:
	UBulletMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Add velocity to the bullet
	void AddVelocity(FVector VelocityImpulse);

	// Change the tick speed
	void SetTickSpeed(float NewSpeed);
	// Change the static forces applied to the projectile
	void SetStaticForce(FVector NewForce);
	// Change air friction
	void SetAirFriction(float NewFriction);

protected:
	// Static forces applied to the bullet
	UPROPERTY(Category = "Physics", VisibleAnywhere)
		FVector StaticForce;
	// The current air friction
	UPROPERTY(Category = "Projectile", EditDefaultsOnly)
		float AirFriction;
	// The multiplier for component ticks
	UPROPERTY(Category = "Projectile", EditDefaultsOnly)
		float TickSpeed;
	// The acceleration of the projectile
	UPROPERTY(Category = "Projectile", EditDefaultsOnly)
		float Acceleration;
};
