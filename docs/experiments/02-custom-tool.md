# 02 — Custom Toolset Extension (Issue #5, A4)

Proves the harness can extend the official UE 5.8 MCP surface with
**project-specific tools** — the main place reusable knowledge accumulates.

**Status: PASS.** 2026-06-22, official UE 5.8 MCP.

## What was added

A content-only plugin `Plugins/PlaygroundToolset/` that registers a Python
`unreal.ToolsetDefinition` with the Toolset Registry:

```
Plugins/PlaygroundToolset/
  PlaygroundToolset.uplugin                 # depends on ToolsetRegistry + PythonScriptPlugin
  Content/Python/
    init_unreal.py                          # auto-run on startup → registers the toolset
    playground_toolset/
      __init__.py
      screenshot_toolset.py                 # PlaygroundToolset.take_labeled_screenshot
```

### Authoring pattern (from the engine `demo_toolset.py`)

```python
@unreal.uclass()
class PlaygroundToolset(unreal.ToolsetDefinition):
    @toolset_registry.tool_call
    @staticmethod
    def take_labeled_screenshot(label: str) -> str:
        """...docstring becomes the MCP tool description; annotated params become the schema..."""
        ...
```

### Registration (`init_unreal.py`)

```python
from toolset_registry.registration import Registration
from playground_toolset.screenshot_toolset import PlaygroundToolset
Registration([PlaygroundToolset]).register()
```

## Enable steps

1. Plugin lives under `Plugins/PlaygroundToolset/` (content-only; no C++ build).
2. Enable it in `MCPPlayground.uproject` (`"Name": "PlaygroundToolset"`).
3. Restart the editor. `init_unreal.py` runs and registers the toolset; the log
   shows `LogToolsetRegistry: Registering Toolset
   playground_toolset.screenshot_toolset.PlaygroundToolset`.
4. Reconnect the MCP client (auto-reconnects once `:8000` is back).

## Verification (MCP, from Claude Code)

- `list_toolsets` now includes
  `playground_toolset.screenshot_toolset.PlaygroundToolset: MCPPlayground project tools.` ✓
- `call_tool(take_labeled_screenshot, {label})` →
  log `LogModelContextProtocol: Dispatching toolset tool: '…PlaygroundToolset.take_labeled_screenshot'`
  and returns a real path; the file is a 1280×720 PNG of the scene with the HUD
  legible (`screenshots/06-custom-tool.png`). Grounded (no hallucinated paths).

## Gotcha (documented for reuse)

- `unreal.AutomationLibrary.take_high_res_screenshot` is **asynchronous and only
  fulfils while a viewport is actively rendering**. In an idle (non-realtime)
  editor viewport the request queues but no file is written until something
  renders (e.g. PIE starts). Capture during PIE, or enable realtime, for the
  artifact to appear. (The first idle request was fulfilled the moment PIE began.)
- `unreal.Paths.project_dir()` returns a path **relative to the engine binaries**;
  it still resolves correctly when handed to the screenshot API, but normalize if
  an absolute path is needed elsewhere.

## Acceptance criteria — met

- [x] Custom tool appears in the MCP tool list from Claude Code (`list_toolsets`).
- [x] Invoking it produces a labeled screenshot artifact.
- [x] This doc records authoring + registration + enable steps.
