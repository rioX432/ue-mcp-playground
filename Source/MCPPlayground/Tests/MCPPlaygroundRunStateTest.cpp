// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "MCPPlaygroundGameState.h"
#include "UObject/Package.h"

#if WITH_DEV_AUTOMATION_TESTS

// Unit test for the wave-survival run state on the GameState. All mutators are pure
// logic, exercised directly in the transient package (headless, no PIE). The
// player-death -> GameOver chain through the character is verified live in V1.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundRunStateTest,
	"MCPPlayground.RunState.Mutators",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundRunStateTest::RunTest(const FString& Parameters)
{
	AMCPPlaygroundGameState* GS = NewObject<AMCPPlaygroundGameState>(GetTransientPackage());
	TestNotNull(TEXT("GameState constructed"), GS);
	if (!GS)
	{
		return false;
	}

	// Initial run state.
	TestEqual(TEXT("wave 0"), GS->Wave, 0);
	TestEqual(TEXT("no enemies"), GS->EnemiesAlive, 0);
	TestTrue(TEXT("player alive"), GS->bPlayerAlive);
	TestFalse(TEXT("not game over"), GS->bGameOver);

	// Wave begins.
	GS->StartNextWave(3);
	TestEqual(TEXT("wave 1"), GS->Wave, 1);
	TestEqual(TEXT("3 enemies"), GS->EnemiesAlive, 3);

	// Kills decrement + score, clamped at zero even with extra kills.
	GS->RegisterEnemyKilled();
	GS->RegisterEnemyKilled();
	GS->RegisterEnemyKilled();
	GS->RegisterEnemyKilled();
	TestEqual(TEXT("enemies clamp at 0"), GS->EnemiesAlive, 0);
	TestEqual(TEXT("score == 4 kills"), GS->Score, 4);

	// Next wave escalates the count and bumps the number.
	GS->StartNextWave(5);
	TestEqual(TEXT("wave 2"), GS->Wave, 2);
	TestEqual(TEXT("5 enemies"), GS->EnemiesAlive, 5);

	// Player death -> game over (idempotent).
	GS->SetPlayerDead();
	GS->SetPlayerDead();
	TestFalse(TEXT("player not alive"), GS->bPlayerAlive);
	TestTrue(TEXT("game over"), GS->bGameOver);

	// Reset clears the run.
	GS->ResetRun();
	TestEqual(TEXT("score reset"), GS->Score, 0);
	TestEqual(TEXT("wave reset"), GS->Wave, 0);
	TestTrue(TEXT("player alive after reset"), GS->bPlayerAlive);
	TestFalse(TEXT("not game over after reset"), GS->bGameOver);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
