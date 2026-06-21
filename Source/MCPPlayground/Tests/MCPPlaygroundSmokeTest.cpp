// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

// Trivial smoke test: proves the headless build + automation-test quality gate is
// wired up end to end (BuildEditor compiles it, run-tests.sh discovers and runs it,
// and a failing assertion would make the wrapper exit non-zero). This is the
// foundational gate every later gameplay test depends on.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMCPPlaygroundSmokeTest,
	"MCPPlayground.Smoke.Trivial",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMCPPlaygroundSmokeTest::RunTest(const FString& Parameters)
{
	TestTrue(TEXT("Smoke gate sanity: true is true"), true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
