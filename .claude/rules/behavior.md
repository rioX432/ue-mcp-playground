# Behavior Rules

## No Guessing
- **Do not make assumptions.** Verify UE API specs, class hierarchies, and asset
  paths before acting. In MCP workflows this means `lookup_class` / `lookup_docs` /
  `get_level_actors` / `search_assets` **before** writing code or issuing editor ops.
- **Fact-checking priority**: official UE docs + `lookup_*` MCP tools first, then
  WebSearch. Cite source URLs for non-obvious API claims.
- Code or editor operations based on guesses will always fail. Ground first.

## Verify Before Implementing (Codex MCP)
Use **Codex MCP** to validate design decisions before implementation — it is for
**design verification**, not fact lookup.

1. Load: `ToolSearch("select:mcp__codex__codex")`
2. Call `mcp__codex__codex` with one focused design question.
3. **Mandatory**: new architecture (e.g. component vs. subsystem vs. actor),
   gameplay framework choices (GAS vs. plain components), Blueprint/C++ boundary
   decisions, MCP toolset extension design.
4. **Skip**: naming, test strategy, anything decidable from existing patterns.

### Fallback (Codex unavailable)
WebSearch official UE docs → check existing project precedent → document the
decision rationale in the PR description.

## Think Twice
After a change, re-verify per the Golden Loop in `.claude/rules/mcp-workflow.md`:
build passes, tests pass, screenshot confirms intent, edge cases considered
(empty level, missing asset, PIE not running). Never call a task done without
build + test (+ screenshot for visual changes) confirmation.
