---
name: dev
description: "E2E UE development: investigate → Codex design → decompose → implement via MCP → test → review → PR"
argument-hint: "[issue number, e.g. #42]"
user-invocable: true
disable-model-invocation: true
allowed-tools:
  - Bash(git checkout:*)
  - Bash(git add:*)
  - Bash(git commit:*)
  - Bash(git push:*)
  - Bash(git diff:*)
  - Bash(git log:*)
  - Bash(git status)
  - Bash(git branch:*)
  - Bash(gh pr create:*)
  - Bash(gh issue view:*)
  - Glob
  - Grep
  - Read
  - Edit
  - Write
  - Agent
  - Skill
  - TaskCreate
  - TaskUpdate
  - TaskList
  - TaskGet
  - ToolSearch
  - AskUserQuestion
  - mcp__codex__codex
---

# /dev — E2E Development Workflow (Unreal Engine via MCP)

Resolve Issue $ARGUMENTS from investigation to PR. Implementation runs through the
**MCP Golden Loop** (`.claude/rules/mcp-workflow.md`), not plain text editing.

**Target:** $ARGUMENTS

## Setup: Task Tracker
`TaskCreate` one task per phase: gather context · investigate · Codex design ·
resolve ambiguities · decompose · implement · quality gate · review · commit/PR.
Mark `in_progress`/`completed` as you go.

## Phase 1: Issue Understanding
`gh issue view <number> --json number,title,body,labels,comments`. Extract title,
description, acceptance criteria, Core Value alignment. Read `CLAUDE.md` and
`.claude/rules/*`. Branch: bug → `fix/{ref}-{slug}`, else `feat/{ref}-{slug}`.

## Phase 2: Investigation
Find affected C++ classes, Blueprints, assets, and level actors. **Ground via
MCP**: `status` → `lookup_class` / `search_assets` / `get_level_actors` to confirm
what actually exists. Never assume names. If anything is ambiguous, `AskUserQuestion`.

## Phase 2.5: Technical Design (Codex — required for new architecture)
`ToolSearch("select:mcp__codex__codex")`, then send issue + investigation and ask
for: implementation approach (component vs subsystem vs actor; C++/Blueprint
boundary), interface changes, edge cases, risks. Mandatory for new gameplay
architecture or MCP toolset extensions. **Fallback**: if Codex is unavailable,
WebSearch official UE docs and document rationale; log "Codex unavailable".

## Phase 3: Ambiguity Resolution
Resolve decision points (use `/dig` if present) using investigation + Codex design.

## Phase 4: Decomposition
`Skill("decompose", ...)` → UE-layered subtasks (core → logic → BP → assets →
tests → cross-cutting). Each task records its MCP/C++ implementation mode and a
concrete Verify step.

## ── AskUserQuestion: Approach Confirmation ──
Present decision matrix + task list + investigation summary. Confirm before
implementing. (Skipped in autonomous `/goal` mode.)

## Phase 5: Branch & Implement (MCP Golden Loop)
`git checkout -b {branch}`. Then, per subtask, follow the Golden Loop from
`.claude/rules/mcp-workflow.md`:
```
status → lookup → act (C++ edit OR MCP editor op) → build_project → run_tests
       → pie_control + capture_viewport → observe → self-correct
```
Guidelines: read surrounding code/Blueprint before editing; use explicit object
paths; never declare a subtask done without build + test (+ screenshot for visual
changes). INTERRUPT on unexpected problems or 3 consecutive failures.

## Phase 6: Quality Gate
Run the project's UE quality gate (CLAUDE.md → Commands), preferring MCP tools:
1. `build_project` (or `RunUAT BuildEditor`) — zero errors.
2. `run_tests` (or headless `UnrealEditor-Cmd ... -nullrhi -ExecCmds="Automation
   RunTests MCPPlayground;quit"`) — all pass.
3. For visual changes: `capture_viewport` confirms intent.
Max 3 fix attempts per failure, then report and stop.

## Phase 7: Review
`Skill("review", ...)` (or self-review). Write `workspace/{issue}/review.json` with
status (`clean|warnings|critical`) and counts. **Critical → stop.** Warning → fix,
re-run quality gate. Extract reusable insights into `docs/claude/review_points.md`.

## ── AskUserQuestion: Commit + PR Confirmation ──
Show changes, quality-gate results, review findings, and the proposed single-line
commit message (no AI stamps). (Skipped in autonomous mode if review is clean/warnings.)

## Phase 8: Commit & PR
Explicit file staging (no `git add .`). Single-line English commit. `git push -u
origin {branch}`. `gh pr create` with `Closes #{issue}`; attach the confirming
screenshot path in the PR body. Report the PR URL.

## Autonomous Mode (/goal)
`/goal "Issue $ARGUMENTS is resolved: build passes, automation test passes,
screenshot confirms the mechanic, and a PR is created"`. Skip AskUserQuestion
confirmations; still stop on Critical review findings or 3 consecutive failures.
Return a structured result:
```json
{ "issue": "#N", "status": "success|failed|blocked",
  "pr_url": "...", "review": { "status": "clean|warnings|critical",
  "critical_count": 0, "warning_count": 0 },
  "screenshot": "docs/experiments/...png", "failure_reason": null }
```

## Error Handling
| Situation | Action |
|---|---|
| Editor/MCP not connected (`status` fails) | Stop, ask user to open editor + enable plugin |
| Build fails (≤3 attempts) | Read errors, `lookup_docs`, fix, rebuild |
| Build fails (>3) | Report, stop |
| Automation test fails | Fix, re-run; if logic gap, revisit Phase 5 |
| Critical review finding | Report, stop |
| Live Coding can't add function | Full editor restart, then rebuild |
