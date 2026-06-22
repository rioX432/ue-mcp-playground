# Experiments Log — the autonomous MCP loop, in order

Each golden task / probe is logged here: what was attempted, which MCP tools were
used, what worked, what failed, and a screenshot. This is how the harness
accumulates reusable knowledge — read top to bottom for the full story of an AI
agent building a playable UE 5.8 game through the Model Context Protocol.

The thread runs: **prove the editor can be driven at all → build a collectible
golden task → grow it into a wave-survival shooter → polish the look/feel** — every
step closing the Golden Loop (`status → lookup → act → build → test → PIE +
screenshot → self-correct`, see `../../.claude/rules/mcp-workflow.md`).

## Harness foundation (Epic A)

| # | Log | What it proves |
|---|---|---|
| 00 | [Bootstrap & connection](00-bootstrap.md) | Plugin + server + toolsets all on; the three things that must be true or nothing works. |
| 01 | [Minimal mutation loop](01-mutation-loop.md) | spawn → save → re-query → PIE → capture: editor state mutates and *persists*. |
| 02 | [Custom toolset extension](02-custom-tool.md) | A project Python `AICallable` (`PlaygroundToolset.take_labeled_screenshot`) registered into the MCP server. |

## Golden task: collectible pickup (Epic B)

| # | Log | What it proves |
|---|---|---|
| 02 | [Sandbox test map](02-sandbox-map.md) | A clean `/Game/Maps/Sandbox` built by duplicating an engine template (no "new level" MCP tool). |
| 03 | [Third-person character + Enhanced Input](03-character.md) | A content-free C++ character with Enhanced Input wired entirely in code. |
| 04 | [BP_Coin placed via MCP](04-coins.md) | `UPickupComponent` → `BP_Coin`, three instances placed via explicit object paths. |
| ★ | [Golden Task 01 — Collectible](golden-task-01-collectible.md) | The headline result for Epic B: pickup increments score, verified headless + live in PIE. |

## Wave-survival shooter (Epics C–G)

| # | Log | What it proves |
|---|---|---|
| 05 | [Wave-survival loop](05-shooter-loop.md) | Spawn → chase → contact damage → death → GAME OVER → restart, verified live in PIE. |
| 06 | [Art & audio integration](06-art-audio-integration.md) | Asset-pluggable SFX hooks + asset-free muzzle flash; what Fab integration is/isn't automatable. |
| 07 | [Humanoid + revolver + aim/fire](07-humanoid-aim.md) | Manny humanoid holding a revolver, over-the-shoulder camera, muzzle-origin crosshair aim. |

## Reading the logs

- **Reusable findings** in each log are distilled into
  [`../claude/review_points.md`](../claude/review_points.md) — check that before any PR.
- Screenshots live in [`screenshots/`](screenshots/).
- The operating discipline these logs follow is
  [`../../.claude/rules/mcp-workflow.md`](../../.claude/rules/mcp-workflow.md).
</content>
