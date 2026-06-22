# UE Headless Automation Testing

How to write headless (`-nullrhi`) automation tests for this project. These
patterns were proven across the score/pickup/health/projectile/firing/enemy/wave
suites (12+ tests). **Every gameplay change ships with a test that runs without
PIE.** Run with `scripts/run-tests.sh <Filter>`; a green wrapper exit is not enough
— confirm the editor log shows `Found N>0 automation tests` (a 0-match filter also
exits 0). A failing test makes the wrapper exit non-zero (CI-usable).

## Boilerplate (UE 5.8)

```cpp
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMyTest, "MCPPlayground.Area.Case",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FMyTest::RunTest(const FString& Parameters) { /* ... */ return bOk; }
#endif
```

- `EAutomationTestFlags` is an **enum class**; the mask is the free constant
  `EAutomationTestFlags_ApplicationContextMask` (NOT `EAutomationTestFlags::...Mask`).
- Test path = `MCPPlayground.<Area>.<Case>`; the wrapper's default filter is
  `MCPPlayground`.

## Prefer pure logic, then a thin integration test

Cheapest and most deterministic: test **pure methods with no world**.
- `NewObject<UMyComponent>(GetTransientPackage())` then call methods directly
  (e.g. `AddScore`, `ApplyDamage`, `ComputeMoveDirectionToTarget`,
  `EnemyCountForWave`). UPROPERTY defaults apply even without `BeginPlay`.
- Design gameplay so the testable core is a separable method: e.g. projectile
  `HandleImpact(AActor*)` instead of relying on simulated flight; firing via a
  public `FireWeapon()` instead of an input event; enemy death via a callable
  `HandleEnemyDeath()`.

## When you need a world

```cpp
FWorldContext& Ctx = GEngine->CreateNewWorldContext(EWorldType::Game);
UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("MyTestWorld"), GetTransientPackage());
World->AddToRoot();
Ctx.SetCurrentWorld(World);
World->InitializeActorsForPlay(FURL());
// ... spawn actors, SetGameState, exercise ...
World->DestroyWorld(false);
GEngine->DestroyWorldContext(World);
World->RemoveFromRoot();
```

- GameState is **not** auto-created in a hand-built world: `SpawnActor<AMy...GameState>()`
  then `World->SetGameState(GS)`.
- Count spawned actors with `TActorIterator<AType>(World)` (`#include "EngineUtils.h"`).
- Avoid depending on `BeginPlay`/Tick firing: call the methods you want to test
  directly. (We do not rely on delegate auto-binding done in BeginPlay.)

## Gotchas that cost a red test here

- **A bare `AActor` has no root component**, so `SetActorLocation` is a silent
  no-op and `GetActorLocation` stays at origin. Give test actors a root:
  `NewObject<USceneComponent>(A); ->RegisterComponent(); A->SetRootComponent(...)`.
  (`ACharacter`/`AMCPProjectile` have their own roots and are fine.)
- **Dynamic multicast delegates have no `AddLambda`.** `DECLARE_DYNAMIC_MULTICAST_DELEGATE`
  can only bind `UFUNCTION`s (`AddDynamic`). In tests, assert the *effect*
  (state change) rather than binding the delegate, or bind from a real `UObject`.
- **Don't shadow engine member functions.** A free helper named `DrawLine` inside
  an `AHUD` method resolves to `AHUD::DrawLine(float,...)` and mis-compiles —
  name project helpers distinctly (we used `DrawTextLine`).
- Tests live in `Source/MCPPlayground/Tests/` and are guarded by
  `#if WITH_DEV_AUTOMATION_TESTS`.

## What can't be unit-tested headlessly

Live input, rendering, and PIE-only behavior. For those, verify in the editor
(PIE + `CaptureEditorImage` + the score/run-state `UE_LOG`s), and drive runtime by
**teleporting PIE-world actors** (`UEDPIE_0_<Map>` package) since stock MCP has no
input injection. See `.claude/rules/mcp-workflow.md` (Editor Lifecycle) and
`docs/experiments/05-shooter-loop.md`.
