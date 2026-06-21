# MCPPlayground — Unreal Engine 5.8 MCP Playground

A playground for AI-assisted game development in Unreal Engine 5.8 driven through
the **Model Context Protocol (MCP)**. The goal is to build a reusable AI
development harness (`.claude/` skills, rules, docs) while actually shipping a
small playable game — and to prove that an AI agent can autonomously implement a
gameplay mechanic in UE via MCP.

## Core Values

Every feature/issue must directly strengthen one of these (one-step test). If it
does not, it goes to `## Won't Do`.

1. **Autonomous build-verify loop** — the agent can change the editor/code, run
   it (PIE), observe the result (screenshot + automation test), and self-correct
   without a human in the loop.
2. **Grounded, no-hallucination workflow** — the agent queries real engine APIs
   and editor state before acting; it never guesses class/asset/actor names.
3. **Reusable harness** — what we learn becomes `.claude/` rules, skills, and
   custom MCP toolset extensions that transfer to other UE projects.
4. **Playable result** — a real, runnable game mechanic, not just a tech demo.

## Won't Do

- Shipping/packaged builds, store distribution, monetization (out of scope for a
  playground).
- Multiplayer/networking until single-player golden tasks pass.
- Photoreal art production — use Fab/engine primitives; art is not the point.

## Architecture Layers (for /decompose ordering)

Implement inner-to-outer. UE-specific layering:

1. **C++ core types** — `USTRUCT`/`UENUM`, gameplay data, interfaces (`UINTERFACE`).
2. **Gameplay logic (C++)** — `AActor`/`UActorComponent`/`USubsystem` classes,
   `UFUNCTION(BlueprintCallable)` surface.
3. **Blueprint layer** — Blueprint subclasses, node graphs, designer-facing config
   (edited via MCP, not by hand).
4. **Assets & level** — meshes, materials, input mappings, level actors
   (placed/configured via MCP using explicit object paths).
5. **Automation tests** — `IMPLEMENT_SIMPLE_AUTOMATION_TEST` / functional tests
   that prove the mechanic works headlessly.
6. **Cross-cutting** — build passes, lint/format, CI.

## Commands (Quality Gate)

Adjust paths to your local UE 5.8 install. `UE_ROOT` is the engine root,
`PROJECT` is the absolute path to `MCPPlayground.uproject`.

| Purpose | Command |
|---|---|
| Generate project files | `"$UE_ROOT/Engine/Build/BatchFiles/<Platform>/GenerateProjectFiles.sh" -project="$PROJECT" -game` |
| Build (Development Editor) | `"$UE_ROOT/Engine/Build/BatchFiles/RunUAT.sh" BuildEditor -project="$PROJECT"` |
| Run automation tests (headless) | `"$UE_ROOT/Engine/Binaries/<Platform>/UnrealEditor-Cmd" "$PROJECT" -nullrhi -unattended -nopause -nosplash -ExecCmds="Automation RunTests MCPPlayground;quit" -log` |
| Cook (sanity) | `"$UE_ROOT/Engine/Build/BatchFiles/RunUAT.sh" BuildCookRun -project="$PROJECT" -cook -platform=Win64 -nop4 -build` |

In the AI-assisted MCP loop, prefer the MCP tools (`build_project`, `run_tests`,
`pie_control`, `capture_viewport`) over raw command lines — see
`.claude/rules/mcp-workflow.md`.

## MCP Servers

- **Official Unreal MCP** (UE 5.8 built-in, experimental) — primary. Editor must
  be open; binds `http://127.0.0.1:8000/mcp`. Enable in Edit > Plugins, enable
  Toolset Registry, restart.
- See `.mcp.json` for client config and `.claude/rules/mcp-workflow.md` for the
  operating discipline.

## Rules

- `.claude/rules/mcp-workflow.md` — **read first.** How to drive UE via MCP
  (status → lookup → act → build → test → PIE+screenshot → self-correct).
- `.claude/rules/ue-conventions.md` — Epic C++ coding standard, asset naming, LFS.
- `.claude/rules/behavior.md` — no guessing, Codex for design verification.
- `.claude/rules/ai-ops.md` — issue-driven flow, Core Value guard.

## Skills

| Command | Purpose |
|---|---|
| `/dev #N` | E2E: investigate → Codex design → decompose → implement (via MCP) → test → review → PR |
| `/dev-all` | Process issues sequentially, each in an isolated sub-agent, CI-gated merge |
| `/decompose` | Break an issue into UE-layered subtasks |

## Workflow

All work is issue-driven. Pick the lowest-numbered unblocked issue, run `/dev #N`,
or `/dev-all` for a batch. Research never goes straight to implementation — it
becomes an issue first (see `.claude/rules/ai-ops.md`).
