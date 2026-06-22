// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPWaveManager.h"

#include "Engine/World.h"
#include "MCPEnemyCharacter.h"
#include "MCPPlaygroundGameState.h"

AMCPWaveManager::AMCPWaveManager()
{
	PrimaryActorTick.bCanEverTick = true;
	EnemyClass = AMCPEnemyCharacter::StaticClass();
}

int32 AMCPWaveManager::EnemyCountForWave(int32 WaveNumber) const
{
	const int32 Steps = FMath::Max(0, WaveNumber - 1);
	return BaseEnemies + Steps * EnemiesPerWaveIncrement;
}

void AMCPWaveManager::SpawnNextWave()
{
	UWorld* World = GetWorld();
	if (!World || !EnemyClass)
	{
		return;
	}

	AMCPPlaygroundGameState* GameState = World->GetGameState<AMCPPlaygroundGameState>();
	if (!GameState)
	{
		return;
	}

	const int32 NextWave = GameState->Wave + 1;
	const int32 Count = EnemyCountForWave(NextWave);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < Count; ++i)
	{
		const float Angle = (2.0f * PI * i) / FMath::Max(1, Count);
		const FVector Location(FMath::Cos(Angle) * SpawnRadius, FMath::Sin(Angle) * SpawnRadius, 100.0f);
		World->SpawnActor<AMCPEnemyCharacter>(EnemyClass, Location, FRotator::ZeroRotator, SpawnParams);
	}

	GameState->StartNextWave(Count);
}

void AMCPWaveManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (const AMCPPlaygroundGameState* GameState = World->GetGameState<AMCPPlaygroundGameState>())
	{
		// Start the first wave, and each subsequent wave once the current one is cleared.
		if (GameState->bPlayerAlive && !GameState->bGameOver && GameState->EnemiesAlive <= 0)
		{
			SpawnNextWave();
		}
	}
}
