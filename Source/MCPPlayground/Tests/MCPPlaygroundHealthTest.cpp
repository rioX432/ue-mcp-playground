// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "HealthComponent.h"
#include "UObject/Package.h"

#if WITH_DEV_AUTOMATION_TESTS

// Unit test for the health/damage core. ApplyDamage is pure logic (no world), so
// the component is exercised directly in the transient package, fully headless.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundHealthTest,
	"MCPPlayground.Health.Damage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundHealthTest::RunTest(const FString& Parameters)
{
	UHealthComponent* Health = NewObject<UHealthComponent>(GetTransientPackage());
	TestNotNull(TEXT("HealthComponent constructed"), Health);
	if (!Health)
	{
		return false;
	}

	// Defaults: Health == MaxHealth == 100.
	TestEqual(TEXT("starts at full health"), Health->Health, 100.0f);
	TestFalse(TEXT("not dead at full"), Health->IsDead());

	Health->ApplyDamage(30.0f);
	TestEqual(TEXT("health after 30 damage"), Health->Health, 70.0f);

	// Heals / non-positive amounts are ignored by ApplyDamage.
	Health->ApplyDamage(0.0f);
	Health->ApplyDamage(-50.0f);
	TestEqual(TEXT("non-positive damage ignored"), Health->Health, 70.0f);

	// Overkill clamps to zero and marks dead.
	Health->ApplyDamage(999.0f);
	TestEqual(TEXT("health clamps at 0"), Health->Health, 0.0f);
	TestTrue(TEXT("dead at zero"), Health->IsDead());

	// Death state is stable — further damage does not go negative.
	Health->ApplyDamage(10.0f);
	TestEqual(TEXT("stays at 0 after death"), Health->Health, 0.0f);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
