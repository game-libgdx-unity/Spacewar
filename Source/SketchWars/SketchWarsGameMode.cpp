// Fill out your copyright notice in the Description page of Project Settings.

#include "SketchWars.h"
#include "SketchWarsGameMode.h"

#include "Asteroid.h"
#include "TieFighter.h"

void ASketchWarsGameMode::BeginPlay() {
	Super::BeginPlay();
	RemainingEnemySpawnTime = FMath::RandRange(1000, 2000);
	GetWorldTimerManager().SetTimer(
		SpawnEnemyTimerHandle,
		this,
		&ASketchWarsGameMode::AdvanceRemainingEnemySpawnTime,
		0.01f,
		true
	);
}

void ASketchWarsGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	UWorld* World = GetWorld();
	
	if (AsteroidClass && World) {
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, AAsteroid::StaticClass(), FoundActors);

		TArray<AActor*> FoundSpawns;
		UGameplayStatics::GetAllActorsOfClass(World, ATargetPoint::StaticClass(), FoundSpawns);

		if (FoundActors.Num() <= 5) {
			int32 index = FMath::RandRange(0, FoundSpawns.Num() - 1);
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			AActor* Spawn = FoundSpawns[index];
			World->SpawnActor<AAsteroid>(
				AsteroidClass, 
				Spawn->GetActorLocation(),
				Spawn->GetActorRotation(),
				SpawnParams
			);
		}
	}
}

void ASketchWarsGameMode::SpawnTieFighter() {
	UWorld* World = GetWorld();

	if (EnemyClass && World) {
		float SpawnZ = FMath::FRandRange(100.0f, 700.0f);
		bool GoRight = FMath::RandBool();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		World->SpawnActor<ATieFighter>(
			EnemyClass,
			FVector((GoRight) ? 0.0f : 1024.0f, 0.0f, SpawnZ),
			FRotator((GoRight) ?  0.0f : 180.0f, 0.0f, 0.0f),
			SpawnParams
		);
	}
}

void ASketchWarsGameMode::AdvanceRemainingEnemySpawnTime() {
	RemainingEnemySpawnTime--;
	if (RemainingEnemySpawnTime <= 0) {
		SpawnTieFighter();
		RemainingEnemySpawnTime = FMath::RandRange(1000, 2000);
		GetWorldTimerManager().ClearTimer(SpawnEnemyTimerHandle);
		GetWorldTimerManager().SetTimer(
			SpawnEnemyTimerHandle,
			this,
			&ASketchWarsGameMode::AdvanceRemainingEnemySpawnTime,
			0.01f,
			true
		);
	}
}