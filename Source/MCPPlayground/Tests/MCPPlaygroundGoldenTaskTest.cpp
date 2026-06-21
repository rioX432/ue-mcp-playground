// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "PickupComponent.h"
#include "MCPPlaygroundGameState.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/UObjectGlobals.h"

#if WITH_DEV_AUTOMATION_TESTS

// End-to-end headless proof of the collectible mechanic through the *authored*
// BP_Coin asset (not just the raw component, which 02/#8 covers): load the
// Blueprint class, spawn it into a world with a GameState, collect it, and assert
// the score went up by exactly one and the coin was consumed. Runs under -nullrhi,
// no PIE / PlayerController.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundGoldenTaskTest,
	"MCPPlayground.GoldenTask.Pickup",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundGoldenTaskTest::RunTest(const FString& Parameters)
{
	// --- world + GameState ---
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("GoldenTaskWorld"), GetTransientPackage());
	if (!TestNotNull(TEXT("test world created"), World))
	{
		return false;
	}
	World->AddToRoot();
	WorldContext.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());

	AMCPPlaygroundGameState* GameState = World->SpawnActor<AMCPPlaygroundGameState>();
	World->SetGameState(GameState);

	bool bSuccess = TestNotNull(TEXT("GameState spawned"), GameState);

	// --- load and spawn the authored BP_Coin ---
	UClass* CoinClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blueprints/BP_Coin.BP_Coin_C"));
	bSuccess &= TestNotNull(TEXT("BP_Coin class loads"), CoinClass);

	if (GameState && CoinClass)
	{
		AActor* Coin = World->SpawnActor<AActor>(CoinClass);
		bSuccess &= TestNotNull(TEXT("BP_Coin spawned"), Coin);

		UPickupComponent* Pickup = Coin ? Coin->FindComponentByClass<UPickupComponent>() : nullptr;
		bSuccess &= TestNotNull(TEXT("BP_Coin carries a UPickupComponent"), Pickup);

		AActor* Collector = World->SpawnActor<AActor>();

		if (Pickup && Collector)
		{
			bSuccess &= TestEqual(TEXT("score starts at 0"), GameState->Score, 0);

			const bool bConsumed = Pickup->Consume(Collector);
			bSuccess &= TestTrue(TEXT("coin is collected"), bConsumed);
			bSuccess &= TestEqual(TEXT("score is 1 after collecting one coin"), GameState->Score, 1);
			bSuccess &= TestTrue(TEXT("coin actor is consumed (being destroyed)"),
				!IsValid(Coin) || Coin->IsActorBeingDestroyed());

			// No double-count on a second overlap.
			const bool bSecond = Pickup->Consume(Collector);
			bSuccess &= TestFalse(TEXT("second collect is a no-op"), bSecond);
			bSuccess &= TestEqual(TEXT("score stays 1"), GameState->Score, 1);
		}
		else
		{
			bSuccess = false;
		}
	}

	// --- teardown ---
	World->DestroyWorld(false);
	GEngine->DestroyWorldContext(World);
	World->RemoveFromRoot();

	return bSuccess;
}

#endif // WITH_DEV_AUTOMATION_TESTS
