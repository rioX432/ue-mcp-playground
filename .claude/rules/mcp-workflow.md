# MCP Workflow — Driving Unreal Engine via the Model Context Protocol

This is the most important rule file in this project. UE development through MCP
is **not** plain text editing. Implementation happens through tool calls against a
live editor process. The agent must keep itself grounded in real editor state and
close the build–run–observe–correct loop on every change.

Sources: [Epic Unreal MCP docs](https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor),
[StraySpark setup guide](https://www.strayspark.studio/blog/unreal-mcp-setup-claude-code-2026),
[mcp-unreal](https://github.com/remiphilippe/mcp-unreal).

## The Golden Loop

Every change to the editor or C++ code MUST follow this loop:

```
status → lookup → act → build → test → PIE + screenshot → observe → self-correct
```

1. **status** — Call the MCP `status`/health tool first to confirm the editor and
   plugin are connected. Never issue editor operations against an unknown state.
2. **lookup (anti-hallucination)** — Before writing C++ or Blueprint logic, query
   the real API: `lookup_class` for inheritance/properties/functions,
   `lookup_docs` for API patterns. **Never guess class, function, asset, or actor
   names.** "AI doesn't hallucinate — your missing context does."
3. **act** — Make the change: edit C++ source (text), or drive the editor via MCP
   (spawn/transform actors, create materials, edit Blueprint nodes/pins).
4. **build** — After editing C++, call `build_project` (or the UE_ROOT build
   command). Read build errors carefully; use `lookup_docs` to resolve UE API
   errors. Live Coding cannot add new functions — a full restart may be required.
5. **test** — Call `run_tests` (or headless `Automation RunTests`). Tests are the
   authority on "does the mechanic work", especially in autonomous mode.
6. **PIE + screenshot** — For runtime/visual behavior: `pie_control start` →
   `status` to confirm it started → `capture_viewport` (with `include_ui=true`
   for HUD) → review the image → `pie_control stop`. **The "take a screenshot and
   review yourself" loop is the single highest-value habit.**
7. **observe & self-correct** — Read the screenshot and test output. If the result
   is wrong, go back to step 2. Do not declare a task done without visual and/or
   test confirmation.

## Object Path Discipline

Use specific, real paths — never invented ones:

- **Actor path**: `/Game/Maps/MapName.MapName:PersistentLevel.ActorName`
- **Asset path**: `/Game/Folder/AssetName`
- **Engine asset**: `/Engine/BasicShapes/Cube`

Discover them at runtime — do not assume:

- `get_level_actors` → discover actor paths in the current level.
- `search_assets` → find asset paths before referencing them.

## Execution Model Constraints

- MCP tools execute **serially on the game thread**. Do not assume parallelism;
  sequence dependent operations explicitly.
- Transport is **HTTP + SSE only** (no stdio/WebSocket). **Loopback only, no auth**
  — never expose the port beyond localhost.
- The official plugin is **experimental**: features may be incomplete and the API
  may change. When a tool is missing or unstable, fall back to a supplementary
  server (StraySpark / mcp-unreal) and log it in `docs/experiments/`.

## Definition of Done (per change)

A change is only "done" when:

1. `build_project` succeeds (zero errors).
2. `run_tests` for the relevant automation test(s) passes.
3. For visual/gameplay changes: a `capture_viewport` screenshot confirms the
   intended result, and the screenshot/path is recorded in the experiment log.

## Logging Experiments

Every golden task / probe gets an entry in `docs/experiments/`: what was
attempted, which MCP tools were used, what worked, what failed, and screenshots.
This is how the harness accumulates reusable knowledge.
