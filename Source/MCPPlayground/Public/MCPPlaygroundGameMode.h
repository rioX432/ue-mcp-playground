// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MCPPlaygroundGameMode.generated.h"

/**
 * Default game mode for the playground. Wires together the score GameState and the
 * debug HUD so the collectible mechanic works in PIE out of the box. The default
 * pawn is added with the character work (#9).
 */
UCLASS()
class MCPPLAYGROUND_API AMCPPlaygroundGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMCPPlaygroundGameMode();

	/** Resets the run after game over: clears run state, heals the player, removes remaining enemies. */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void RestartGame();
};
