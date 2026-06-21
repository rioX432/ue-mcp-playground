# ue-mcp-playground

A playground for AI-assisted game development in **Unreal Engine 5.8** driven
through the **Model Context Protocol (MCP)**.

Two goals at once:

1. **Build a reusable AI dev harness** — `.claude/` skills, rules, and docs,
   adapted from [ai-dev-templates](https://github.com/rioX432/ai-dev-templates)
   for Unreal Engine. After it proves out here, it is upstreamed as a
   `layers/game-ue` layer.
2. **Actually make a game** — ship a small, playable mechanic and prove an AI
   agent can autonomously implement it in UE via MCP (build → run → screenshot →
   self-correct).

## Status

Experimental. UE 5.8 shipped an official (experimental) Unreal MCP plugin on
2026-06-17 ([Epic docs](https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor)).
APIs may change.

## Setup

Works on **macOS (Apple Silicon)** and **Windows** — the same repo, harness, and
MCP loop on either machine. Develop wherever Claude Code and the editor share a box
(tighter loop). UE 5.8 macOS caveats (software-only Lumen, M1 no Nanite) don't
affect this playground.

1. Install **Unreal Engine 5.8** (Epic Games Launcher).
2. `git lfs install` (once per machine).
3. Open `MCPPlayground.uproject` in the editor (let it compile / regenerate
   project files if prompted).
4. **Edit > Plugins** → enable **Unreal MCP** → enable the **Toolset Registry** →
   restart the editor. The server binds `http://127.0.0.1:8000/mcp`.
5. In the editor console: `ModelContextProtocol.GenerateClientConfig ClaudeCode`
   to (re)generate `.mcp.json` if needed.
6. Set engine paths for headless commands:
   - macOS/Linux: `export UE_ROOT="/Users/Shared/Epic Games/UE_5.8"` and
     `export PROJECT="$PWD/MCPPlayground.uproject"`
   - Windows: `set UE_ROOT=C:\Program Files\Epic Games\UE_5.8` and
     `set PROJECT=%CD%\MCPPlayground.uproject`
   Then run tests with `scripts/run-tests.sh` (macOS/Linux) or
   `scripts\run-tests.bat` (Windows).
7. Run Claude Code from the project root. Read `.claude/rules/mcp-workflow.md`.

## Harness

| File | Purpose |
|---|---|
| `CLAUDE.md` | Project charter: Core Values, architecture layers, commands |
| `.claude/rules/mcp-workflow.md` | **How to drive UE via MCP** (the core discipline) |
| `.claude/rules/ue-conventions.md` | Epic C++ standard, asset naming, LFS |
| `.claude/rules/behavior.md` | No-guessing + Codex design verification |
| `.claude/rules/ai-ops.md` | Issue-driven flow, Core Value guard |
| `.claude/skills/dev` | E2E single-issue workflow (MCP-aware) |
| `.claude/skills/dev-all` | Sequential batch issue processing |
| `.claude/skills/decompose` | UE-layered task decomposition |

## Workflow

Issue-driven. `/dev #N` for one issue, `/dev-all` for a batch. See `docs/issues/`
for the seeded backlog and `docs/experiments/` for the golden-task log.
