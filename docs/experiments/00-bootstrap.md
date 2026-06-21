# 00 — Editor / MCP Bootstrap & Connection Verification (Issue #1, A0)

The foundation experiment: bring up the official UE 5.8 Unreal MCP plugin, connect
an MCP client (Claude Code), and prove the Golden Loop primitives work
(`status` ≈ connection + `list_toolsets`, and `CaptureViewport` returns an image).

**Status: PASS.** Verified 2026-06-22 on macOS (Apple Silicon).

## Environment (grounded)

| Fact | Value |
|---|---|
| Engine | UE 5.8, `++UE5+Release-5.8-CL-55116800` |
| Engine root (`UE_ROOT`) | `/Users/Shared/Epic Games/UE_5.8` |
| Platform | macOS arm64 (native universal binary) |
| Project | `/Users/rio/workspace/projects/ue-mcp-playground/MCPPlayground.uproject` |
| MCP plugin | "Unreal MCP" (`ModelContextProtocol`) VersionName 1.0 |
| Editor toolset plugin | `EditorToolset` VersionName 1.0 |
| Server endpoint | `http://127.0.0.1:8000/mcp` (HTTP + SSE, loopback, no auth) |
| MCP protocol version | `2025-06-18` |

## Launch command

```
"/Users/Shared/Epic Games/UE_5.8/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor" \
  "/Users/rio/workspace/projects/ue-mcp-playground/MCPPlayground.uproject"
```

## The non-obvious bootstrap chain (this is the whole experiment)

Enabling the plugin in `Edit > Plugins` is **necessary but not sufficient**. Three
separate things must be true before the agent can drive the editor:

1. **Plugin enabled** — `ModelContextProtocol` in `MCPPlayground.uproject`
   (`Edit > Plugins` → "Unreal MCP" → restart). Loads the plugin and registers
   the MCP meta-tools.
2. **Server started** — the HTTP server does **NOT** auto-start by default.
   `ModelContextProtocol.GenerateClientConfig ClaudeCode` only *writes the client
   config* (`.mcp.json`); it does not bind the port. The server is started by the
   console command **`ModelContextProtocol.StartServer`** (stop with
   `ModelContextProtocol.StopServer`). Confirmed in the log:
   `LogHttpListener: Created new HttpListener on 127.0.0.1:8000`.
   - **Fix for autonomy:** `Config/DefaultEditorPerProjectUserSettings.ini` sets
     `bAutoStartServer=True` on `UModelContextProtocolSettings` (module
     `ModelContextProtocolEngine`), so the server starts on every editor launch
     with no manual console call. This is what lets the loop survive restarts
     unattended.
3. **Editor-control toolsets enabled** — out of the box only
   `ToolsetRegistry.AgentSkillToolset` (skill authoring) is discoverable, which
   does **nothing** for editor manipulation. The actual editor tools live in
   separate Toolset plugins (all `EnabledByDefault: false`). We enable in
   `MCPPlayground.uproject`:
   - `EditorToolset` — viewport capture, PIE control, actor/asset/level/object/
     blueprint/mesh/material tools.
   - `AutomationTestToolset` — headless test discovery/run/results.
   After enabling + restart, `list_toolsets` reports 20 toolsets.

## Confirmed tool surface (what the project's invented names map to)

The official MCP runs in **tool-search mode**: clients see only 3 meta-tools
(`list_toolsets`, `describe_toolset`, `call_tool`) and dispatch real tools through
`call_tool(toolset_name, tool_name, arguments)`. Mapping from the aspirational
names in `mcp-workflow.md` to the real toolset tools:

| Golden-Loop concept | Real toolset / tool |
|---|---|
| `status` / health | MCP `initialize` + `list_toolsets` succeed; `EditorAppToolset.IsPIERunning` for PIE state |
| `capture_viewport` | `EditorToolset.EditorAppToolset.CaptureViewport` (PNG base64 + labeled actors) |
| editor screenshot | `EditorToolset.EditorAppToolset.CaptureEditorImage` |
| `pie_control start/stop` | `EditorAppToolset.StartPIE` / `StopPIE` / `IsPIERunning` |
| `get_level_actors` / spawn / place | `editor_toolset.toolsets.scene.SceneTools` (+ `EditorAppToolset.GetVisibleActors`/`GetSelectedActors`) |
| actor transform/label/components | `editor_toolset.toolsets.actor.ActorTools` |
| `lookup_class` / properties | `editor_toolset.toolsets.object.ObjectTools` |
| `search_assets` / asset I/O | `editor_toolset.toolsets.asset.AssetTools` |
| Blueprint authoring | `editor_toolset.toolsets.blueprint.BlueprintTools` |
| `run_tests` | `AutomationTestToolset.AutomationTestToolset` (Discover/List/Run/GetTestStatus/GetTestResults) |
| read editor log | `EditorToolset.LogsToolset` |

`build_project` has **no MCP tool** — build C++ via `RunUAT BuildEditor` /
`scripts/run-tests.sh` on the host; Live Coding for in-editor recompiles (cannot
add new functions → full restart for new C++ symbols).

## CaptureViewport gotchas (important for the loop)

- `CaptureViewport` requires `captureTransform`, `annotations`, and `bShowUI` to
  all be present (the MCP wrapper rejects "optional" params left unset with
  `input param "X" needs a default value`). Disable overlays with
  `annotations:{gridSpacing:0,gridExtent:0,gridHeight:0,maxLabelDistance:0,maxLabels:0,classFilter:{refPath:""}}`.
  Get a sane pose from `EditorAppToolset.GetCameraTransform` first.
- The result is a **base64 PNG ~3.6M characters** that overflows the agent
  context. Handling: the harness saves oversized tool results to a file; decode
  `returnValue.image.data` to a PNG, then downscale (`sips -Z 768`) before viewing.
  Screenshots are written to `docs/experiments/screenshots/`.
  First capture: `docs/experiments/screenshots/a0-bootstrap-capture.png`
  (1419×1548; landscape `Untitled_1` map, lit terrain — confirms render path).

## Failure modes observed + fixes

1. **Port 8000 not listening / `curl` connection refused / `/mcp` reconnect
   "ConnectionRefused"** — plugin enabled but server never started. *Fix:* run
   `ModelContextProtocol.StartServer` (or set `bAutoStartServer=True`).
2. **Connected but only `AgentSkillToolset` available — no editor tools.** Editor
   toolset plugins not enabled. *Fix:* enable `EditorToolset` +
   `AutomationTestToolset` in the `.uproject`, restart.
3. **`/mcp` reconnect fails right after restart** — server start ordering: the
   client reconnected before `StartServer` ran. *Fix:* StartServer first, then
   reconnect (moot once `bAutoStartServer=True`).
4. **`GenerateClientConfig` pollutes `.mcp.json`** — it appends a redundant
   `unreal-mcp` entry pointing at the same URL. *Fix:* keep the single canonical
   `unreal` entry; prune the duplicate.
5. **`CaptureViewport` "input param needs a default value"** — pass all three
   top-level params explicitly (see gotchas above).

## Acceptance criteria — met

- [x] Editor launches with the plugin enabled; `.mcp.json` validated against the
      generated config (pruned to one `unreal` entry).
- [x] MCP connection healthy from Claude Code (`initialize` 200, `list_toolsets`
      returns 20 toolsets).
- [x] `CaptureViewport` returns a real rendered image.
- [x] This bootstrap doc records launch command, port, plugin versions, and the
      failure modes + fixes above.
- [x] `.claude/rules/mcp-workflow.md` Golden Loop references the confirmed tool
      names (see the mapping table there).
