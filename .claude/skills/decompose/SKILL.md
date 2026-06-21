---
name: decompose
description: "Break a UE task into ordered subtasks by engine layer with dependencies"
user-invocable: true
allowed-tools:
  - ToolSearch
---

# Task Decomposition (Unreal Engine)

Break a development task into small, executable subtasks ordered by UE engine
layer, with explicit dependencies and MCP-aware verification steps.

## When to Use
After investigation and design, before the implementation loop.

## Input Requirements
1. Investigation results (affected C++ classes, Blueprints, assets, level actors).
2. Resolved design decisions (incl. Codex validation where required).
3. Confirmed approach.

## Decomposition Rules

### Size
Each task should take 5–30 minutes. Split anything larger.

### Structure per Task
- **What**: what to implement/change.
- **Where**: target file(s) and/or **object paths** (e.g.
  `/Game/Maps/Main.Main:PersistentLevel.Coin_1`).
- **How**: implementation approach — note whether it is **C++ text editing** or a
  **MCP editor operation** (spawn/transform actor, edit Blueprint node/pin,
  create material).
- **Why**: reason the change is needed.
- **Verify**: how to confirm it works — `build_project`, `run_tests <name>`, and/or
  `pie_control` + `capture_viewport`. Every gameplay change pairs with a test.

### UE Layer Ordering (inner → outer)
```
1. C++ core types     — USTRUCT/UENUM, interfaces, gameplay data
2. Gameplay logic     — AActor/UActorComponent/USubsystem (C++), BlueprintCallable surface
3. Blueprint layer    — BP subclasses, node graphs (via MCP)
4. Assets & level     — meshes/materials/input mappings/level actors (via MCP, explicit paths)
5. Automation tests   — IMPLEMENT_SIMPLE_AUTOMATION_TEST / functional tests (headless)
6. Cross-cutting      — build passes, format, CI
```

### Pairing Rules
- Type + its mapper/config = 1 task.
- Implementation + its test = 1 task (never separate).
- Cross-cutting checks = last task.

### Dependency Order
Core types block gameplay logic; logic blocks Blueprint/asset wiring; everything
blocks automation tests; tests block cross-cutting.

## TaskCreate Format
```
subject: "Implement {What} in {Where}"
description: |
  **What**: {description}
  **Where**: {file path(s) and/or object path(s)}
  **How**: {C++ edit | MCP op} — {approach}
  **Why**: {reason}
  **Verify**: {build_project | run_tests <name> | pie_control+capture_viewport}
activeForm: "Implementing {What}"
```

## Output
Show the task list:
```
| # | Layer | Description | Verify | Blocked By |
|---|-------|-------------|--------|------------|
| 1 | Core  | FCoinData struct | build | — |
| 2 | Logic | UPickupComponent | run_tests Pickup | #1 |
| 3 | BP    | BP_Coin from Pickup (MCP) | PIE+screenshot | #2 |
| 4 | Level | Place 3 coins (MCP, paths) | screenshot | #3 |
| 5 | Test  | Automation: pickup increments score | run_tests | #2 |
| 6 | Cross | build + format | build | #3,#4,#5 |
```

## Codex Architecture Validation (Optional)
After generating the list, validate with Codex:
`ToolSearch("select:mcp__codex__codex")` then ask it to check layer ordering,
missing tasks, dependency correctness, parallelizable groups, and highest-risk
tasks. Apply corrections and note what changed. If Codex is unavailable, output
as-is.
