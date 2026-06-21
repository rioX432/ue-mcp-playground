# 01 — Minimal End-to-End Editor Mutation Loop (Issue #3, A2)

The canonical de-risk: prove the full autonomous Golden Loop on the smallest
inspectable change before building gameplay. Codex flagged the biggest risk as
*stateful editor mutation that looks applied but is subtly wrong* — so this round
trips through **save + re-query** to prove the mutation actually persisted.

**Status: PASS.** Verified 2026-06-22 on macOS (Apple Silicon), official UE 5.8 MCP.

## The loop, as executed (real tool calls)

| Step | Tool | Result |
|---|---|---|
| status | `list_toolsets` | 20 toolsets incl. EditorAppToolset / SceneTools |
| ground | `SceneTools.get_current_level` | `/Temp/Untitled_1` (transient) |
| create map | `AssetTools.duplicate` `/Engine/Maps/Templates/Template_Default` → `/Game/Maps/Sandbox` | true |
| load | `SceneTools.load_level /Game/Maps/Sandbox` | ok (after `save_assets []` to clear the dirty transient level) |
| ground | `SceneTools.find_actors` | Floor_0, PlayerStart_0, DirectionalLight_0, SkyLight_0, SkyAtmosphere_0, … |
| act (spawn) | `SceneTools.add_to_scene_from_asset /Engine/BasicShapes/Cube` "MutationProbeCube" @ (0,0,200) | `…:PersistentLevel.StaticMeshActor_1` |
| act (property) | `SceneTools.set_actor_folder` → "MutationProbe" | ok |
| save | `AssetTools.save_assets ["/Game/Maps/Sandbox"]` | true |
| **re-query** | `SceneTools.find_actors name="MutationProbeCube"` | `StaticMeshActor_1` ✓ |
| **re-query** | `SceneTools.get_actors_in_folder "MutationProbe"` | `StaticMeshActor_1` ✓ (folder property persisted) |
| screenshot | `EditorAppToolset.FocusOnActors` → `GetCameraTransform` → `CaptureViewport` | `screenshots/01-mutation-loop-cube.png` |
| PIE | `StartPIE` → `IsPIERunning` (true) → `StopPIE` | ok |

Object path of the probe actor:
`/Game/Maps/Sandbox.Sandbox:PersistentLevel.StaticMeshActor_1` (label `MutationProbeCube`).

## What this de-risked / learned

- **There is no "new level" or "save-level-as" MCP tool.** Create a `/Game` map by
  `AssetTools.duplicate` of an engine template (`Template_Default` is a clean
  basic level: floor, PlayerStart, directional/sky light, sky atmosphere, fog).
- **`load_level` refuses if the current (transient) level is dirty.** Call
  `AssetTools.save_assets([])` first to flush dirty state, then load.
- **Spawned actor names are engine-assigned** (`StaticMeshActor_1`); the `name`
  argument becomes the *label*. Re-query by label (`find_actors name=`) or folder.
- **`CaptureViewport` requires `captureTransform` + `annotations` + `bShowUI`** all
  present, and returns a multi-MB base64 PNG → decode via `scripts/decode-capture.py`
  then downscale (`sips -Z 768`) before viewing.
- The mutation **persisted across save** (verified by re-query, the core risk).

## Screenshot

`docs/experiments/screenshots/01-mutation-loop-cube.png` — the probe cube hovering
over the Template_Default floor under sky lighting, proving the render + spawn path.

## Acceptance criteria — met

- [x] Actor created at explicit path `/Game/Maps/Sandbox.Sandbox:PersistentLevel.StaticMeshActor_1`.
- [x] Post-save re-query confirms the actor and its folder property persist.
- [x] `CaptureViewport` screenshot saved to `docs/experiments/screenshots/`.
- [x] Loop documented here as the canonical example.

> The probe cube is removed when the Sandbox map is finalized in #6 (B0); this map
> file is committed there.
