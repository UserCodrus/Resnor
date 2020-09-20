// Copyright © 2020 Brian Faubion. All rights reserved.

#include "CyberShooterEnemy.h"
#include "CyberShooterPlayer.h"

#include "Components/CapsuleComponent.h"

ACyberShooterEnemy::ACyberShooterEnemy()
{
	// Apply the hit event handler
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACyberShooterEnemy::OnHit);

	// Set defaults
	CollisionForce = 400.0f;
	ContactDamage = 1;
	ContactDamageType = DAMAGETYPE_ENEMY;
	Momentum = MaxMomentum;
}

void ACyberShooterEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Apply contact damage and knockback to players
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		FVector direction = player->GetActorLocation() - GetActorLocation();
		player->AddImpulse(direction.GetSafeNormal() * CollisionForce);

		player->Damage(ContactDamage, ContactDamageType, this, this);
	}
}