# ue-mcp-playground

**An AI agent autonomously built a playable wave-survival shooter in Unreal Engine
5.8 — driven entirely through the Model Context Protocol (MCP).** No manual editor
authoring beyond what the agent drove via tool calls: it edited C++, built, ran the
game in PIE, took screenshots, read the result, and self-corrected in a loop.

This repo is two things at once:

1. **A reusable AI dev harness** — `.claude/` skills, rules, and docs, adapted from
   [ai-dev-templates](https://github.com/rioX432/ai-dev-templates) for Unreal Engine.
2. **An actual game** — a small, playable mechanic, shipped to prove the agent can
   close the build → run → screenshot → self-correct loop unattended.

> UE 5.8 shipped its official (experimental) Unreal MCP plugin on 2026-06-17
> ([Epic docs](https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor)).
> This is a playground exploring what an agent can build through it. APIs may change.

## What the agent built

A third-person **wave-survival shooter**: a humanoid player holding a revolver
fends off escalating waves of chasing enemies, with a center-crosshair aim, HUD
(Score / Wave / HP), game-over, and restart — plus the original collectible-coin
golden task, coexisting in the same map.

| Humanoid player in the Sandbox | Over-the-shoulder aim |
|---|---|
| ![Humanoid player](docs/experiments/screenshots/07-shooter-humanoid.png) | ![Over-the-shoulder aim](docs/experiments/screenshots/08-shooter-aim.png) |

**Built end-to-end through MCP, verified at every step:**

- **6 epics (A → G), every backlog issue shipped, 0 open.** Bootstrap → editor
  mutation loop → custom toolset → collectible golden task → wave-survival shooter
  → art/audio polish → humanoid + aim/fire.
- **12 headless automation tests, all green** — every gameplay change ships with a
  test that runs without PIE (`-nullrhi`), CI-usable.
- **The agent drove the full close → build → relaunch cycle unattended** — C++
  edits, Blueprint property edits, PIE, screenshots, test runs, all via MCP.

The full story, in order, is the [experiments log](docs/experiments/README.md) —
each step records the MCP tools used, what worked, what failed, and a screenshot.

## How it works — the Golden Loop

Every change follows one discipline (full version:
[`.claude/rules/mcp-workflow.md`](.claude/rules/mcp-workflow.md)):

```
status → lookup → act → build → test → PIE + screenshot → observe → self-correct
```

- **status** — confirm the editor + plugin are connected before any op.
- **lookup (anti-hallucination)** — query real engine APIs / object paths before
  writing code. Never guess class, asset, or actor names.
- **act → build → test** — edit C++ or drive the editor, then `Build.sh` + headless
  automation tests are the authority on "does it work".
- **PIE + screenshot → self-correct** — `CaptureEditorImage` the live view, read the
  image, and if it's wrong, loop back. The single highest-value habit.

Two hard constraints shaped the whole approach (both verified the hard way):

- **The editor must be CLOSED to compile new C++ symbols** (open = hot-reload dylib
  trap → stale code runs silently). The agent drives kill → build → relaunch, and
  MCP auto-reconnects.
- **Stock MCP has no input injection.** PIE is driven by teleporting actors in the
  `UEDPIE_0_<Map>` package; live-fire arcs are proven by headless tests instead.

## Setup

Works on **macOS (Apple Silicon)** and **Windows** — same repo, harness, and MCP
loop on either machine. Run Claude Code and the editor on the same box for a tight
loop. UE 5.8 macOS caveats (software-only Lumen, M1 no Nanite) don't affect this
primitives-and-logic playground.

1. Install **Unreal Engine 5.8** (Epic Games Launcher).
2. `git lfs install` (once per machine), then clone.
3. Open `MCPPlayground.uproject` (let it compile / regenerate project files if
   prompted). **The MCP plugins and auto-start are already committed** — all
   toolsets (`EditorToolset`, `AutomationTestToolset`, the project
   `PlaygroundToolset`) are enabled in the `.uproject`, and
   `Config/DefaultEditorPerProjectUserSettings.ini` sets `bAutoStartServer=True`, so
   the server binds `http://127.0.0.1:8000/mcp` on every launch — no manual plugin
   toggling or `ModelContextProtocol.StartServer`. (Re)generate the client config
   only if `.mcp.json` is missing: console `ModelContextProtocol.GenerateClientConfig ClaudeCode`.
4. Set engine paths for headless build/test commands:
   - macOS/Linux: `export UE_ROOT="/Users/Shared/Epic Games/UE_5.8"` and
     `export PROJECT="$PWD/MCPPlayground.uproject"`
   - Windows: `set UE_ROOT=C:\Program Files\Epic Games\UE_5.8` and
     `set PROJECT=%CD%\MCPPlayground.uproject`
5. Run tests: `scripts/run-tests.sh` (macOS/Linux) or `scripts\run-tests.bat`
   (Windows). Confirm the log shows `Found N>0 automation tests` (a 0-match filter
   also exits 0).
6. Run Claude Code from the project root. Start at
   [`.claude/rules/mcp-workflow.md`](.claude/rules/mcp-workflow.md).

## The harness

| File | Purpose |
|---|---|
| [`CLAUDE.md`](CLAUDE.md) | Project charter: Core Values, architecture layers, commands |
| [`.claude/rules/mcp-workflow.md`](.claude/rules/mcp-workflow.md) | **How to drive UE via MCP** (the core discipline) |
| [`.claude/rules/ue-conventions.md`](.claude/rules/ue-conventions.md) | Epic C++ standard, asset naming, LFS |
| [`.claude/rules/ue-testing.md`](.claude/rules/ue-testing.md) | Headless automation-test patterns + gotchas |
| [`.claude/rules/behavior.md`](.claude/rules/behavior.md) | No-guessing + Codex design verification |
| [`.claude/rules/ai-ops.md`](.claude/rules/ai-ops.md) | Issue-driven flow, Core Value guard |
| [`.claude/skills/dev`](.claude/skills/dev/SKILL.md) | E2E single-issue workflow (MCP-aware) |
| [`.claude/skills/dev-all`](.claude/skills/dev-all/SKILL.md) | Sequential batch issue processing |
| [`.claude/skills/decompose`](.claude/skills/decompose/SKILL.md) | UE-layered task decomposition |
| [`docs/claude/review_points.md`](docs/claude/review_points.md) | Distilled reusable insights — check before PR |

## Docs

- [`docs/experiments/`](docs/experiments/README.md) — the golden-task log, in order
  (the build story). Start here.
- [`docs/design.md`](docs/design.md) — harness architecture + the Codex-validated
  backlog (now fully shipped).
- [`docs/claude/review_points.md`](docs/claude/review_points.md) — reusable UE/MCP
  insights extracted across the build.

## Workflow

All work is issue-driven. `/dev #N` for one issue, `/dev-all` for a batch. Research
never goes straight to implementation — it becomes a GitHub Issue first (see
[`.claude/rules/ai-ops.md`](.claude/rules/ai-ops.md)).
</content>
