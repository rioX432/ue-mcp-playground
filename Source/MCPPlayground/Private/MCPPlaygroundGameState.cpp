// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPPlaygroundGameState.h"

void AMCPPlaygroundGameState::AddScore(int32 Delta)
{
	// Score is a non-negative running total; clamp so negative deltas can never
	// drive it below zero.
	Score = FMath::Max(0, Score + Delta);
	OnScoreChanged.Broadcast(Score);
}
