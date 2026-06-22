// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "MCPWaveManager.h"
#include "MCPEnemyCharacter.h"
#include "MCPPlaygroundGameState.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

static int32 CountEnemies(UWorld* World)
{
	int32 Count = 0;
	for (TActorIterator<AMCPEnemyCharacter> It(World); It; ++It)
	{
		++Count;
	}
	return Count;
}

// Wave spawning + escalation, headless. Auto-advance on clear (Tick) is verified in V1.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundWaveTest,
	"MCPPlayground.Wave.SpawnAndEscalate",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundWaveTest::RunTest(const FString& Parameters)
{
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("WaveTestWorld"), GetTransientPackage());
	if (!TestNotNull(TEXT("test world"), World))
	{
		return false;
	}
	World->AddToRoot();
	WorldContext.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());

	AMCPPlaygroundGameState* GameState = World->SpawnActor<AMCPPlaygroundGameState>();
	World->SetGameState(GameState);
	AMCPWaveManager* Manager = World->SpawnActor<AMCPWaveManager>();

	bool bSuccess = TestNotNull(TEXT("gamestate"), GameState) && TestNotNull(TEXT("manager"), Manager);
	if (bSuccess)
	{
		// Escalation formula: 3, 5, 7 for waves 1..3 (base 3, +2/wave).
		bSuccess &= TestEqual(TEXT("wave1 count"), Manager->EnemyCountForWave(1), 3);
		bSuccess &= TestEqual(TEXT("wave2 count"), Manager->EnemyCountForWave(2), 5);
		bSuccess &= TestEqual(TEXT("wave3 count"), Manager->EnemyCountForWave(3), 7);

		// First wave: spawns 3 enemies, GameState reflects it.
		Manager->SpawnNextWave();
		bSuccess &= TestEqual(TEXT("wave is 1"), GameState->Wave, 1);
		bSuccess &= TestEqual(TEXT("enemies alive 3"), GameState->EnemiesAlive, 3);
		bSuccess &= TestEqual(TEXT("3 enemies in world"), CountEnemies(World), 3);

		// Second wave escalates to 5.
		Manager->SpawnNextWave();
		bSuccess &= TestEqual(TEXT("wave is 2"), GameState->Wave, 2);
		bSuccess &= TestEqual(TEXT("enemies alive 5"), GameState->EnemiesAlive, 5);
		bSuccess &= TestEqual(TEXT("8 enemies total in world"), CountEnemies(World), 8);
	}

	World->DestroyWorld(false);
	GEngine->DestroyWorldContext(World);
	World->RemoveFromRoot();

	return bSuccess;
}

#endif // WITH_DEV_AUTOMATION_TESTS
