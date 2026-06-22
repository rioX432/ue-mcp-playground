// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "MCPProjectile.h"
#include "HealthComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#if WITH_DEV_AUTOMATION_TESTS

// Integration test for the projectile impact path: spawn a projectile + a target
// carrying a UHealthComponent in a transient world, call HandleImpact directly
// (no flight simulation), and assert the target took damage and the projectile was
// consumed. Headless, -nullrhi.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundProjectileTest,
	"MCPPlayground.Projectile.Impact",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundProjectileTest::RunTest(const FString& Parameters)
{
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("ProjectileTestWorld"), GetTransientPackage());
	if (!TestNotNull(TEXT("test world"), World))
	{
		return false;
	}
	World->AddToRoot();
	WorldContext.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());

	// Target actor with a health component.
	AActor* Target = World->SpawnActor<AActor>();
	USceneComponent* Root = NewObject<USceneComponent>(Target, TEXT("Root"));
	Root->RegisterComponent();
	Target->SetRootComponent(Root);
	UHealthComponent* TargetHealth = NewObject<UHealthComponent>(Target);
	TargetHealth->RegisterComponent();

	AMCPProjectile* Projectile = World->SpawnActor<AMCPProjectile>();

	bool bSuccess = TestNotNull(TEXT("target health"), TargetHealth) && TestNotNull(TEXT("projectile"), Projectile);
	if (bSuccess)
	{
		Projectile->Damage = 30.0f;
		bSuccess &= TestEqual(TEXT("target starts full"), TargetHealth->Health, 100.0f);

		Projectile->HandleImpact(Target);

		bSuccess &= TestEqual(TEXT("target took 30 damage"), TargetHealth->Health, 70.0f);
		bSuccess &= TestTrue(TEXT("projectile consumed on impact"),
			!IsValid(Projectile) || Projectile->IsActorBeingDestroyed());
	}

	World->DestroyWorld(false);
	GEngine->DestroyWorldContext(World);
	World->RemoveFromRoot();

	return bSuccess;
}

#endif // WITH_DEV_AUTOMATION_TESTS
