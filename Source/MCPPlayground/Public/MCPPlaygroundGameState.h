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
};
