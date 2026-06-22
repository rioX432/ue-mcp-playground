// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "MCPPlaygroundCharacter.h"
#include "MCPProjectile.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

static int32 CountProjectiles(UWorld* World)
{
	int32 Count = 0;
	for (TActorIterator<AMCPProjectile> It(World); It; ++It)
	{
		++Count;
	}
	return Count;
}

// Verifies firing spawns a projectile and respects the cooldown — headless, in a
// transient world, by calling FireWeapon() directly (no input device needed).
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundFiringTest,
	"MCPPlayground.Firing.SpawnsProjectile",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundFiringTest::RunTest(const FString& Parameters)
{
	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("FiringTestWorld"), GetTransientPackage());
	if (!TestNotNull(TEXT("test world"), World))
	{
		return false;
	}
	World->AddToRoot();
	WorldContext.SetCurrentWorld(World);
	World->InitializeActorsForPlay(FURL());

	AMCPPlaygroundCharacter* Character = World->SpawnActor<AMCPPlaygroundCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	bool bSuccess = TestNotNull(TEXT("character spawned"), Character);

	if (bSuccess)
	{
		bSuccess &= TestEqual(TEXT("no projectiles before firing"), CountProjectiles(World), 0);

		Character->FireWeapon();
		bSuccess &= TestEqual(TEXT("one projectile after first shot"), CountProjectiles(World), 1);

		// Immediate second shot is blocked by the cooldown (same world time).
		Character->FireWeapon();
		bSuccess &= TestEqual(TEXT("cooldown blocks the second shot"), CountProjectiles(World), 1);
	}

	World->DestroyWorld(false);
	GEngine->DestroyWorldContext(World);
	World->RemoveFromRoot();

	return bSuccess;
}

#endif // WITH_DEV_AUTOMATION_TESTS
