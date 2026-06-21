// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "MCPPlaygroundGameMode.h"
#include "MCPPlaygroundGameState.h"
#include "MCPPlaygroundHUD.h"
#include "UObject/Package.h"

#if WITH_DEV_AUTOMATION_TESTS

// Headless guard that the GameMode wires the score GameState and debug HUD. The
// on-screen render is verified visually in the editor phase (PIE + capture_viewport);
// this test ensures the wiring that makes that possible never silently regresses.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundGameModeWiringTest,
	"MCPPlayground.GameMode.Wiring",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundGameModeWiringTest::RunTest(const FString& Parameters)
{
	const AMCPPlaygroundGameMode* GameMode = NewObject<AMCPPlaygroundGameMode>(GetTransientPackage());
	TestNotNull(TEXT("GameMode constructed"), GameMode);
	if (!GameMode)
	{
		return false;
	}

	TestEqual(TEXT("GameStateClass is AMCPPlaygroundGameState"),
		GameMode->GameStateClass.Get(), AMCPPlaygroundGameState::StaticClass());
	TestEqual(TEXT("HUDClass is AMCPPlaygroundHUD"),
		GameMode->HUDClass.Get(), AMCPPlaygroundHUD::StaticClass());

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
