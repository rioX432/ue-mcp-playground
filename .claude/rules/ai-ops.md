# AI-Driven Development & Operations

## Core Value Guard
Before any feature work, check `CLAUDE.md → ## Core Values`. Every issue must pass
the one-step test against a Core Value. If it does not, it goes to `## Won't Do`,
not the backlog.

## Research → Implementation Gate
**Research must NOT be implemented directly.** Findings (in `docs/experiments/` or
`docs/research/`) become a GitHub Issue first — with Core Value alignment and a
complexity note — and only then enter the development flow. No shortcut from
"interesting MCP capability" to "let's build it".

## Development Flow (Issue-Driven)
All work is driven by GitHub Issues. **WIP limit: 1 issue at a time.**

1. `gh issue list` → pick the lowest-numbered unblocked issue (skip `won't`).
2. Verify Core Value alignment (ask the user if the issue lacks it).
3. Confirm design with **Codex MCP** for architecture/new-pattern/MCP-extension
   work (see `.claude/rules/behavior.md`).
4. Implement via the MCP Golden Loop (`.claude/rules/mcp-workflow.md`).
5. Quality gate: `build_project` + `run_tests` (+ `capture_viewport` for visual).
6. `/review` self-review; fix findings; extract reusable insights into
   `docs/claude/review_points.md`.
7. Create PR (`Closes #N`).

## Effort Level
| Level | Use when |
|---|---|
| `high` (default) | Standard work, small mechanics |
| `xhigh` | New gameplay architecture, MCP toolset extensions |
| `max` | Debugging editor/engine crashes, unfamiliar UE subsystems |

## /goal for Autonomous Execution
Use `/goal` with a measurable end state for unattended runs, e.g.
`/goal "Issue #N is resolved: build passes, automation test passes, screenshot
confirms the mechanic, and a PR is created"`. A separate evaluator checks the
condition after each turn.
