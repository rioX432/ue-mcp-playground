// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "PickupComponent.h"
#include "MCPPlaygroundGameState.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#if WITH_DEV_AUTOMATION_TESTS

// Integration test for the pickup -> score path. Builds a minimal game world,
// installs a GameState, spawns a coin actor carrying a UPickupComponent, and
// verifies that collecting it increments the score by exactly one — no PIE,
// no PlayerController, no physics ticking (Consume() is the same entry point the
// overlap handler calls, so calling it twice exercises the exactly-once gate).
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundPickupTest,
	"MCPPlayground.Pickup.OverlapScoresOnce",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundPickupTest::RunTest(const FString& Parameters)
{
	// --- Build a transient game world ---
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("PickupTestWorld"), GetTransientPackage());
	if (!TestNotNull(TEXT("test world created"), World))
	{
		return false;
	}
	World->AddToRoot();
	WorldContext.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());

	// --- Install GameState ---
	AMCPPlaygroundGameState* GameState = World->SpawnActor<AMCPPlaygroundGameState>();
	TestNotNull(TEXT("GameState spawned"), GameState);
	World->SetGameState(GameState);

	// --- Spawn a coin actor with a root + pickup component ---
	AActor* CoinActor = World->SpawnActor<AActor>();
	USceneComponent* Root = NewObject<USceneComponent>(CoinActor, TEXT("Root"));
	Root->RegisterComponent();
	CoinActor->SetRootComponent(Root);

	UPickupComponent* Pickup = NewObject<UPickupComponent>(CoinActor);
	Pickup->RegisterComponent();

	// --- A dummy collector ---
	AActor* Collector = World->SpawnActor<AActor>();

	bool bSuccess = true;
	if (GameState && Pickup && Collector)
	{
		bSuccess &= TestEqual(TEXT("score starts at 0"), GameState->Score, 0);

		const bool bFirst = Pickup->Consume(Collector);
		bSuccess &= TestTrue(TEXT("first collect consumes"), bFirst);
		bSuccess &= TestEqual(TEXT("score is 1 after collect"), GameState->Score, 1);

		// Second collect must not double-count.
		const bool bSecond = Pickup->Consume(Collector);
		bSuccess &= TestFalse(TEXT("second collect is a no-op"), bSecond);
		bSuccess &= TestEqual(TEXT("score stays 1 (no double count)"), GameState->Score, 1);
	}
	else
	{
		bSuccess = false;
	}

	// --- Teardown ---
	World->DestroyWorld(false);
	GEngine->DestroyWorldContext(World);
	World->RemoveFromRoot();

	return bSuccess;
}

#endif // WITH_DEV_AUTOMATION_TESTS
