---
name: dev-all
description: "Process UE issues sequentially: /dev per issue in isolated sub-agent → CI wait → merge → next"
argument-hint: "[issue numbers, e.g. #42 #43, or empty for all open issues]"
user-invocable: true
disable-model-invocation: true
allowed-tools:
  - Bash(git checkout:*)
  - Bash(git pull:*)
  - Bash(git log:*)
  - Bash(git status)
  - Bash(git branch:*)
  - Bash(gh pr create:*)
  - Bash(gh pr merge:*)
  - Bash(gh pr view:*)
  - Bash(gh pr checks:*)
  - Bash(gh issue view:*)
  - Bash(gh issue list:*)
  - Glob
  - Grep
  - Read
  - Agent
  - Skill
  - TaskCreate
  - TaskUpdate
  - TaskList
  - TaskGet
  - AskUserQuestion
---

# /dev-all — Sequential Issue Processing (Unreal Engine)

Process multiple GitHub Issues sequentially. Each issue runs `/dev` in an isolated
sub-agent, then waits for CI and merges before the next. Per-issue branches keep
git history atomic and make rollback a single-PR revert.

**Arguments:** $ARGUMENTS

## ⚠️ UE-specific caveat: the editor is a shared, stateful resource
The MCP Golden Loop needs a **running Unreal Editor**, and only one editor holds
the project at a time. Unlike pure-code projects, sub-agents **cannot each run an
independent live editor in a worktree**. Therefore:
- Issues that require live MCP editor ops (Blueprint/asset/level work) run
  **serially against the single open editor** — do not parallelize them.
- Issues that are **C++/headless only** (core types, headless automation tests,
  build fixes) may use `isolation: "worktree"` safely.
- Step 2 investigation tags each issue `editor-bound` or `headless` so ordering
  respects this. Editor-bound issues never overlap.

## Step 0: Core Value Check (GATE)
Read `CLAUDE.md → ## Core Values`. If missing, warn and ask to define them or
proceed without the filter (logged as a warning).

## Step 1: Resolve Target Issues
If `$ARGUMENTS` given, extract numbers. Else `gh issue list --state open --json
number,title,labels,body --limit 100`. Skip issues labeled `won't` or listed in
`CLAUDE.md → ## Won't Do`.

## Step 2: Parallel Investigation (read-only)
Launch parallel Explore agents (one per issue). Each returns: summary, affected
files/Blueprints/assets, estimated scope, dependencies, and an **`editor-bound` vs
`headless`** classification.

## Step 3: Dependency & Order
Detect `blocked by #N` / `depends on #N`. Topological sort; independent issues
first (ascending). **Then enforce the editor-bound constraint**: never schedule two
editor-bound issues to overlap. Circular deps → skip, report.

## ── AskUserQuestion: Execution Plan ──
Present ordered issues, dependencies, skipped issues (with reasons),
editor-bound/headless tags, scope, and Core Value alignment. Confirm.

## Step 4: Sequential Issue Loop
`TaskCreate` per issue. For each (in order):
1. `git checkout main && git pull origin main`.
2. Run `/dev` in a sub-agent:
   - `headless` issue → `Agent(prompt: "/goal '...resolved...' /dev #N", isolation: "worktree")`.
   - `editor-bound` issue → run against the live editor **without** worktree
     isolation; confirm `status` shows the editor connected first.
3. Validate result: read `workspace/{issue}/review.json`.
   - `critical` → skip, report. `warnings` → AskUserQuestion (merge anyway?).
     `clean` → auto-merge. Sub-agent `failed` → skip, report.
4. `gh pr merge {PR} --auto --merge --delete-branch`.
5. Poll until merged (30s interval, 15min timeout). CI fails → ask skip/stop.
6. Mark task completed, proceed.

## Step 5: Final Report
Table of `# | Issue | PR | Status`, plus completed N/M.

## Autonomous Mode (/goal)
`/goal "All issues in $ARGUMENTS are resolved: each has a merged PR or a documented
skip reason"`. Skip confirmations; on CI failure skip and continue; stop only on 3
consecutive failures.

## Error Handling
| Situation | Action |
|---|---|
| Editor not connected for an editor-bound issue | Pause, ask user to open editor |
| Issue not found / circular dep | Skip, report |
| Sub-agent /dev fails / CI fails / merge conflict | Ask skip or stop |
| 3 consecutive failures | Stop, report |
| Auto-merge timeout (15min) | Report, ask user |
