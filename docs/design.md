# Harness Design & Backlog (Codex cross-validated)

This document records the harness architecture and the seeded backlog, including
the Claude-vs-Codex cross-validation (Phase D). Sources for UE 5.8 MCP facts are in
`.claude/rules/mcp-workflow.md` and `README.md`.

## Harness architecture

`.claude/` adapts [ai-dev-templates](https://github.com/rioX432/ai-dev-templates)
for Unreal Engine 5.8 + MCP:

- **rules/mcp-workflow.md** — the Golden Loop: `status → lookup → act → build →
  test → PIE + screenshot → self-correct`. Object-path discipline, anti-hallucination.
- **rules/ue-conventions.md** — Epic C++ standard, asset naming, Git LFS.
- **rules/behavior.md / ai-ops.md** — no-guessing, Codex design verification,
  issue-driven flow, Core Value guard.
- **skills/dev, dev-all, decompose** — MCP-aware E2E workflow, lane-aware batch
  processing, UE-layered decomposition.

## Key design decision: editor lanes (Codex-confirmed)

`/dev-all` normally isolates each sub-agent in a git worktree. The MCP Golden Loop
needs ONE running editor holding the project, so **editor-bound issues cannot each
run an independent live editor in parallel worktrees**. Issues are classified:

- `headless` — C++ / automation-test only. Worktree-safe, parallel, CI-gated.
- `editor-bound` — needs the live editor. Single serialized "editor lane", explicit
  lock, no overlap.
- `mixed` — headless PR first, then a separate editor-verification issue.

Every `editor-bound` issue must define: target map · explicit object paths ·
pre-state query · post-state query · build/test command · PIE/screenshot artifact ·
rollback/cleanup plan.

## Codex cross-validation summary (Phase D)

Codex (gpt-5.2) reviewed the initial backlog and recommended:

1. Add **A0 editor/MCP bootstrap** before anything; reorder so headless quality gate
   precedes editor mutation.
2. Add an early **minimal end-to-end editor mutation loop** to de-risk autonomy
   *before* the collectible mechanic — the biggest risk is stateful editor mutation
   that *looks* applied but is subtly wrong; prove persistence by re-querying object
   paths/properties after save + PIE.
3. Make `/dev-all` lane-aware with an explicit editor lock (own issue).
4. Add **B0 sandbox test map** so later level/gameplay issues aren't noisy.
5. **HUD as C++ debug text/AHUD**, not UMG (authoring via stock MCP is the risky part).
6. **Third-person + Enhanced Input from a UE template / minimal C++ seed**, not
   MCP-authored — stock UE 5.8 MCP is reliable for spawn/transform/material/light,
   PIE, screenshots, test runs, and some Blueprint node editing, but not for
   unattended authoring of full character/input/UMG/functional-test *assets*.
7. Split B6 into headless gameplay test + editor golden verification.
8. Keep to the **official** MCP to preserve the "official UE 5.8 MCP" claim;
   third-party servers (StraySpark/mcp-unreal) only as a logged fallback.

## Final backlog

### Epic A — Harness foundation
| ID | Title | Lane | Depends |
|----|-------|------|---------|
| A0 | Editor/MCP bootstrap & connection verification | editor-bound | — |
| A1 | Headless build + automation-test quality gate | headless | — |
| A2 | Minimal end-to-end editor mutation loop (de-risk autonomy) | editor-bound | A0, A1 |
| A3 | Lane-aware /dev-all + editor lock + editor-bound issue template | headless | A1 |
| A4 | Custom Toolset extension scaffold (Python AICallable) | editor-bound | A0 |

### Epic B — Golden task: collectible pickup
| ID | Title | Lane | Depends |
|----|-------|------|---------|
| B0 | Sandbox test map (floor, light, player start, naming) | editor-bound | A2 |
| B1 | C++ score state (GameMode/PlayerState) + test | headless | A1 |
| B2 | C++ UPickupComponent: overlap → score+1 + test | headless | A1, B1 |
| B3 | Third-person character + Enhanced Input (template/C++ seed) | editor-bound | B0 |
| B4 | BP_Coin from UPickupComponent, placed via MCP (3 coins, paths) | editor-bound | B2, B3 |
| B5 | C++ HUD score display (AHUD debug text) | headless | B1 |
| B6a | Headless gameplay test: pickup increments score | headless | B2, B4 |
| B6b | Editor golden verification: PIE + capture_viewport + log | editor-bound | B4, B5, B6a |
