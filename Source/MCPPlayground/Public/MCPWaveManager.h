// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MCPWaveManager.generated.h"

class AMCPEnemyCharacter;

/**
 * Drives the survival loop: spawns escalating waves of enemies in a ring and tracks
 * wave/enemy counts on the GameState. No placed spawn points or NavMesh — positions
 * are computed, so the spawn logic is headless-testable.
 */
UCLASS()
class MCPPLAYGROUND_API AMCPWaveManager : public AActor
{
	GENERATED_BODY()

public:
	AMCPWaveManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<AMCPEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 BaseEnemies = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 EnemiesPerWaveIncrement = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float SpawnRadius = 800.0f;

	/** Enemy count for a given (1-based) wave number. */
	UFUNCTION(BlueprintCallable, Category = "Wave")
	int32 EnemyCountForWave(int32 WaveNumber) const;

	/** Spawns the next wave (count from GameState->Wave+1) in a ring and registers it. */
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void SpawnNextWave();

	virtual void Tick(float DeltaSeconds) override;
};
