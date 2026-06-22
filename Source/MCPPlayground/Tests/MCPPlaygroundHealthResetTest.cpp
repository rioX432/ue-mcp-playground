// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "HealthComponent.h"
#include "UObject/Package.h"

#if WITH_DEV_AUTOMATION_TESTS

// ResetHealth (used by GameMode::RestartGame) restores full health and re-arms death.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundHealthResetTest,
	"MCPPlayground.Health.Reset",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundHealthResetTest::RunTest(const FString& Parameters)
{
	UHealthComponent* Health = NewObject<UHealthComponent>(GetTransientPackage());
	if (!TestNotNull(TEXT("health"), Health))
	{
		return false;
	}

	Health->ApplyDamage(999.0f);
	TestTrue(TEXT("dead after overkill"), Health->IsDead());

	Health->ResetHealth();
	TestEqual(TEXT("full health after reset"), Health->Health, 100.0f);
	TestFalse(TEXT("alive after reset"), Health->IsDead());

	// Death can fire again after a reset (re-armed).
	Health->ApplyDamage(150.0f);
	TestTrue(TEXT("dead again after reset+damage"), Health->IsDead());

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
