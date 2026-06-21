// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPPlaygroundGameState.h"

DEFINE_LOG_CATEGORY_STATIC(LogMCPPlayground, Display, All);

void AMCPPlaygroundGameState::AddScore(int32 Delta)
{
	// Score is a non-negative running total; clamp so negative deltas can never
	// drive it below zero.
	Score = FMath::Max(0, Score + Delta);
	OnScoreChanged.Broadcast(Score);

	// Observable signal for the autonomous loop: a coin collected during PIE prints
	// here, so the score increment can be verified headlessly via the editor log
	// (LogsToolset) without reading the live PIE GameState.
	UE_LOG(LogMCPPlayground, Display, TEXT("[MCPPlayground] Score -> %d (delta %d)"), Score, Delta);
}
