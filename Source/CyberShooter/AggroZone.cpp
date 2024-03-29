// Copyright © 2020 Brian Faubion. All rights reserved.

#include "AggroZone.h"

#include "CyberShooterPlayer.h"
#include "CyberShooterEnemy.h"
#include "Spawner.h"
#include "OrientationInterface.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#if WITH_EDITOR
#include "Components/ArrowComponent.h"
#endif

AAggroZone::AAggroZone()
{
	PrimaryActorTick.bCanEverTick = false;

	OnActorBeginOverlap.AddDynamic(this, &AAggroZone::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AAggroZone::EndOverlap);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->Mobility = EComponentMobility::Static;
	RootComponent->bVisualizeComponent = true;

	// Set up the overlap box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName("OverlapAll");
	CollisionBox->SetBoxExtent(FVector(50.0f));
	CollisionBox->SetUsingAbsoluteRotation(true);
	CollisionBox->Mobility = EComponentMobility::Static;
	CollisionBox->SetupAttachment(RootComponent);
	
	DisableActors = true;
	RestrictOrientation = true;
	RespawnActors = true;
	RespawnTime = 600.0f;

	Active = true;
	Cleared = false;
	Aggro = false;

	TotalEnemies = 0;
	DespawnedEnemies = 0;

	SetCamera = false;
	CameraDistance = 1500.0f;

#if WITH_EDITOR
	OrientationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	OrientationArrow->SetupAttachment(RootComponent);
	OrientationArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	OrientationArrow->SetUsingAbsoluteScale(true);
	OrientationArrow->ArrowColor = FColor::Yellow;
	OrientationArrow->bIsEditorOnly = true;
#endif
}

void AAggroZone::BeginPlay()
{
	Super::BeginPlay();

	// Register the aggro zone with actors inside it
	for (int32 i = 0; i < Actors.Num(); ++i)
	{
		IAggroInterface* object = Cast<IAggroInterface>(Actors[i]);
		if (object != nullptr)
		{
			object->RegisterZone(this);
		}
	}

	// Disable actors in the aggro zone
	if (!Active && DisableActors)
	{
		Disable();
	}
}

void AAggroZone::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Ignore the player entering the zone if it is inactive
		if (!Active)
		{
			return;
		}

		// Make sure the player matches the orientation of the zone
		if (RestrictOrientation)
		{
			if (!player->CheckOrientation(RootComponent->GetUpVector()))
			{
				return;
			}
		}

		// Set the player's camera distance
		if (SetCamera && !Cleared)
		{
			player->SetCameraDistance(CameraDistance);
		}

		// Aggro enemies when the player enters the zone
		for (int32 i = 0; i < Actors.Num(); ++i)
		{
			if (Actors[i] != nullptr)
			{
				IAggroInterface* aggrotarget = Cast<IAggroInterface>(Actors[i]);
				aggrotarget->Aggro();
			}
		}

		// Cancel the respawn timer
		if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_RespawnTimer) > 0.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RespawnTimer);
		}

		// Call blueprint events
		OnEnter.Broadcast();

		Aggro = true;
	}
	else if (RespawnActors)
	{
		// Cancel respawns if the enemy returns to the zone after leaving
		for (int32 i = 0; i < Actors.Num(); ++i)
		{
			if (Actors[i] == OtherActor)
			{
				IAggroInterface* aggrotarget = Cast<IAggroInterface>(Actors[i]);
				aggrotarget->EnterAggro();
				return;
			}
		}
	}
}

void AAggroZone::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(OtherActor);
	if (player != nullptr)
	{
		// Ignore the player leaving if the player didn't activate the zone when it entered
		if (!Aggro)
		{
			return;
		}

		// Reset the player's camera distance
		if (SetCamera)
		{
			player->ResetCameraDistance();
		}

		// Lose aggro when the player leaves the zone
		for (int32 i = 0; i < Actors.Num(); ++i)
		{
			if (Actors[i] != nullptr)
			{
				IAggroInterface* aggrotarget = Cast<IAggroInterface>(Actors[i]);
				aggrotarget->EndAggro();
			}
		}

		// Start the respawn timer
		if (RespawnActors)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AAggroZone::Respawn, RespawnTime);
		}

		// Call blueprint events
		OnExit.Broadcast();

		Aggro = false;
	}
	else if (RespawnActors)
	{
		// Respawn enemies that leave the zone
		for (int32 i = 0; i < Actors.Num(); ++i)
		{
			if (Actors[i] == OtherActor)
			{
				IAggroInterface* aggrotarget = Cast<IAggroInterface>(Actors[i]);
				aggrotarget->ExitAggro();
				return;
			}
		}

		// Check to see if the actor is a minion belonging to a spawner in the zone
		AEnemyMinion* minion = Cast<AEnemyMinion>(OtherActor);
		if (minion != nullptr)
		{
			for (int32 i = 0; i < Actors.Num(); ++i)
			{
				if (minion->IsParent(Actors[i]))
				{
					minion->StartRespawn();
					return;
				}
			}
		}
	}
}

void AAggroZone::UpdateActorList()
{
	// Get all the enemies the aggro zone overlaps
	TArray<TEnumAsByte<EObjectTypeQuery>> types;
	TArray<AActor*> ignore;
	TArray<AActor*> actors;
	UKismetSystemLibrary::ComponentOverlapActors(CollisionBox, CollisionBox->GetComponentTransform(), types, AActor::StaticClass(), ignore, actors);

	// Add overlapped enemies and spawners to the zone
	Actors.Empty();
	for (int32 i = 0; i < actors.Num(); ++i)
	{
		IAggroInterface* actor = Cast<IAggroInterface>(actors[i]);
		if (actor != nullptr)
		{
			// Check the orientation of relevant object
			AEnemyBase* enemy = Cast<AEnemyBase>(actor);
			if (enemy != nullptr)
			{
				if (!RootComponent->GetUpVector().Equals(enemy->GetUpVector()))
				{
					continue;
				}
			}
			ADestructible* destructible = Cast<ADestructible>(actor);
			if (destructible != nullptr)
			{
				if (!RootComponent->GetUpVector().Equals(destructible->GetActorUpVector()))
				{
					continue;
				}
			}
			IOrientationInterface* object = Cast<IOrientationInterface>(actor);
			if (object != nullptr)
			{
				if (!object->CheckOrientation(RootComponent->GetUpVector()))
				{
					continue;
				}
			}

			// Add the actor
			Actors.Add(actors[i]);
		}
	}
}

void AAggroZone::NotifyRegister()
{
	TotalEnemies++;
}

void AAggroZone::NotifyDespawn()
{
	DespawnedEnemies++;

	// Clear the zone
	if (DespawnedEnemies >= TotalEnemies && !Cleared)
	{
		// Play the clear sound
		if (ClearSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ClearSound, GetActorLocation());
		}

		// Reset player camera
		if (SetCamera)
		{
			ACyberShooterPlayer* player = Cast<ACyberShooterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			if (player != nullptr)
			{
				player->ResetCameraDistance();
			}
		}

		// Call blueprint events
		OnClear.Broadcast();

		Cleared = true;

		FString msg("Cleared zone ");
		msg.Append(GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, msg);
	}
}

void AAggroZone::Respawn()
{
	// Reset pawns
	for (int32 i = 0; i < Actors.Num(); ++i)
	{
		IAggroInterface* aggrotarget = Cast<IAggroInterface>(Actors[i]);
		if (aggrotarget != nullptr)
		{
			aggrotarget->AggroReset();
		}
	}
	DespawnedEnemies = 0;

	// Call blueprint events
	OnRespawn.Broadcast();

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, "Zone respawn");
}

void AAggroZone::Disable()
{
	for (int32 i = 0; i < Actors.Num(); ++i)
	{
		IAggroInterface* object = Cast<IAggroInterface>(Actors[i]);
		if (object != nullptr)
		{
			object->AggroDisable();
		}
	}
}

void AAggroZone::Activate()
{
	Active = true;
	if (DisableActors)
	{
		Respawn();
	}

	// Update overlaps to activate the aggro zone if the player is inside it
	ClearComponentOverlaps();
	UpdateOverlaps();
}

void AAggroZone::Deactivate()
{
	Active = false;
	if (DisableActors)
	{
		Disable();
	}

	// Update overlaps to activate the aggro zone if the player is inside it
	ClearComponentOverlaps();
	UpdateOverlaps();
}