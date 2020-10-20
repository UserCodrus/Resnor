// Copyright © 2020 Brian Faubion. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemySeekerAI.generated.h"

class AEnemySeeker;
class ACyberShooterPlayer;

// Enemy AI that chases after players
UCLASS()
class CYBERSHOOTER_API AEnemySeekerAI : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemySeekerAI();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	// The pawn being controlled by this AI
	UPROPERTY(VisibleAnywhere)
		AEnemySeeker* EnemyPawn;
	// The player character
	UPROPERTY(VisibleAnywhere)
		ACyberShooterPlayer* Player;
};
