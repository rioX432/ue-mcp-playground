// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "MCPPlaygroundGameState.h"
#include "UObject/Package.h"

#if WITH_DEV_AUTOMATION_TESTS

// Unit test for the authoritative score state. AddScore is pure logic (no world
// dependency), so the GameState can be constructed in the transient package and
// exercised directly — fully headless, no PIE / PlayerController.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundScoreTest,
	"MCPPlayground.Score.AddScore",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundScoreTest::RunTest(const FString& Parameters)
{
	AMCPPlaygroundGameState* GameState = NewObject<AMCPPlaygroundGameState>(GetTransientPackage());
	TestNotNull(TEXT("GameState constructed"), GameState);
	if (!GameState)
	{
		return false;
	}

	TestEqual(TEXT("initial score is 0"), GameState->Score, 0);

	GameState->AddScore(1);
	TestEqual(TEXT("score after +1"), GameState->Score, 1);

	GameState->AddScore(5);
	TestEqual(TEXT("score after +5"), GameState->Score, 6);

	GameState->AddScore(-100);
	TestEqual(TEXT("score clamps at 0"), GameState->Score, 0);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
