// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPPlaygroundGameMode.h"

#include "MCPPlaygroundCharacter.h"
#include "MCPPlaygroundGameState.h"
#include "MCPPlaygroundHUD.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MCPEnemyCharacter.h"

AMCPPlaygroundGameMode::AMCPPlaygroundGameMode()
{
	GameStateClass = AMCPPlaygroundGameState::StaticClass();
	HUDClass = AMCPPlaygroundHUD::StaticClass();
	DefaultPawnClass = AMCPPlaygroundCharacter::StaticClass();
}

void AMCPPlaygroundGameMode::RestartGame()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (AMCPPlaygroundGameState* GameState = World->GetGameState<AMCPPlaygroundGameState>())
	{
		GameState->ResetRun();
	}

	if (APawn* Player = UGameplayStatics::GetPlayerPawn(World, 0))
	{
		if (UHealthComponent* PlayerHealth = Player->FindComponentByClass<UHealthComponent>())
		{
			PlayerHealth->ResetHealth();
		}
	}

	// Remove any leftover enemies; the WaveManager restarts at wave 1 once EnemiesAlive is 0.
	TArray<AActor*> Enemies;
	for (TActorIterator<AMCPEnemyCharacter> It(World); It; ++It)
	{
		Enemies.Add(*It);
	}
	for (AActor* Enemy : Enemies)
	{
		Enemy->Destroy();
	}
}
