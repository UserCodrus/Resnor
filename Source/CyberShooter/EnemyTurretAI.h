// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyTurretAI.generated.h"

class AEnemyTurret;
class ACyberShooterPlayer;

// Enemy AI that sits in place and aims at nearby players
UCLASS()
class CYBERSHOOTER_API AEnemyTurretAI : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyTurretAI();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// If set to true the turret will try to compensate for player movement when aiming
	UPROPERTY(VisibleAnywhere)
		bool SmartAim;

protected:
	// The pawn being controlled by this AI
	UPROPERTY(VisibleAnywhere)
		AEnemyTurret* EnemyPawn;
	// The player character
	UPROPERTY(VisibleAnywhere)
		ACyberShooterPlayer* Player;
};
