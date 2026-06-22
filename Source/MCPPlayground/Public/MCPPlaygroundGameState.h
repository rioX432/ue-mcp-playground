// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MCPPlaygroundGameState.generated.h"

/** Fired whenever the score changes; carries the new total. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);

/**
 * Authoritative, game-wide score state for the collectible mechanic.
 *
 * Lives on GameState (not GameMode/PlayerState) so the HUD can read it via
 * GetWorld()->GetGameState<AMCPPlaygroundGameState>(), gameplay (UPickupComponent)
 * can write it, and automation tests can construct it without a PlayerController or
 * a live PIE session.
 */
UCLASS()
class MCPPLAYGROUND_API AMCPPlaygroundGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	/** Current collectible score for the session. */
	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 Score = 0;

	/** Adds Delta to the score (clamped at >= 0) and broadcasts OnScoreChanged. */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Delta);

	/** Broadcast after the score changes. */
	UPROPERTY(BlueprintAssignable, Category = "Score")
	FOnScoreChanged OnScoreChanged;

	// --- Wave-survival run state (authoritative; read by HUD, written by gameplay) ---

	/** Current wave number (1-based once started). */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	int32 Wave = 0;

	/** Enemies remaining in the current wave. */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	int32 EnemiesAlive = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Run")
	bool bPlayerAlive = true;

	UPROPERTY(BlueprintReadOnly, Category = "Run")
	bool bGameOver = false;

	/** Begins a wave: increments Wave and sets EnemiesAlive to EnemyCount. */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void StartNextWave(int32 EnemyCount);

	/** One enemy died: decrements EnemiesAlive (clamped) and awards a point. */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void RegisterEnemyKilled();

	/** Player died: flips bPlayerAlive false + bGameOver true (idempotent). */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void SetPlayerDead();

	/** Resets the whole run (for restart). */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void ResetRun();
};
