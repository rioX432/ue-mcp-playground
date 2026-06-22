// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "MCPEnemyCharacter.h"
#include "MCPPlaygroundGameState.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#if WITH_DEV_AUTOMATION_TESTS

// Enemy logic, headless: (a) chase direction points at the target, (b) death
// reports a kill to the GameState (score +1, EnemiesAlive--). Movement integration
// (distance actually decreasing) is verified live in V1.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundEnemyTest,
	"MCPPlayground.Enemy.ChaseAndKill",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundEnemyTest::RunTest(const FString& Parameters)
{
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("EnemyTestWorld"), GetTransientPackage());
	if (!TestNotNull(TEXT("test world"), World))
	{
		return false;
	}
	World->AddToRoot();
	WorldContext.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());

	AMCPPlaygroundGameState* GameState = World->SpawnActor<AMCPPlaygroundGameState>();
	World->SetGameState(GameState);

	AMCPEnemyCharacter* Enemy = World->SpawnActor<AMCPEnemyCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);

	bool bSuccess = TestNotNull(TEXT("gamestate"), GameState) && TestNotNull(TEXT("enemy"), Enemy);
	if (bSuccess)
	{
		// (a) Chase direction toward a target 1000 units along +X should point ~+X.
		// A bare AActor has no root, so SetActorLocation is a no-op — give it one.
		AActor* Target = World->SpawnActor<AActor>();
		USceneComponent* TargetRoot = NewObject<USceneComponent>(Target, TEXT("Root"));
		TargetRoot->RegisterComponent();
		Target->SetRootComponent(TargetRoot);
		Target->SetActorLocation(FVector(1000.0f, 0.0f, 0.0f));
		const FVector Dir = Enemy->ComputeMoveDirectionToTarget(Target);
		bSuccess &= TestTrue(TEXT("chase direction points toward target"),
			FVector::DotProduct(Dir, FVector(1, 0, 0)) > 0.9f);

		// (b) Death reports a kill: set up a wave of 1, then kill.
		GameState->StartNextWave(1);
		Enemy->HandleEnemyDeath();
		bSuccess &= TestEqual(TEXT("enemies alive 0 after kill"), GameState->EnemiesAlive, 0);
		bSuccess &= TestEqual(TEXT("score +1 on kill"), GameState->Score, 1);
	}

	World->DestroyWorld(false);
	GEngine->DestroyWorldContext(World);
	World->RemoveFromRoot();

	return bSuccess;
}

#endif // WITH_DEV_AUTOMATION_TESTS
