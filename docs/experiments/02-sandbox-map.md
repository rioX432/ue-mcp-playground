# 02 — Sandbox Test Map (Issue #6, B0)

A fixed, clean level so later gameplay/level issues (B3/B4/B6) operate on a known
substrate. Created by duplicating the engine `Template_Default` (a minimal lit
level) and finalizing it via MCP.

**Status: PASS.** Verified 2026-06-22, official UE 5.8 MCP.

## Map

- Asset: `/Game/Maps/Sandbox` (file `Content/Maps/Sandbox.umap`, committed via Git LFS).
- Set as `EditorStartupMap` and `GameDefaultMap` in `Config/DefaultEngine.ini`.
- GameMode (`AMCPPlaygroundGameMode`, from #11) applies via `GlobalDefaultGameMode`.

## How it was built (MCP)

1. `AssetTools.duplicate /Engine/Maps/Templates/Template_Default → /Game/Maps/Sandbox`.
2. `SceneTools.load_level /Game/Maps/Sandbox` (after `save_assets []`).
3. Removed the #3 probe cube (`remove_from_scene`).
4. Organized lighting under a `Lighting` outliner folder (`set_actor_folder`).
5. `save_assets ["/Game/Maps/Sandbox"]`.

## Object paths (for reuse by B3 / B4 / B6)

Persistent-level actor paths follow
`/Game/Maps/Sandbox.Sandbox:PersistentLevel.<ActorName>`:

| Actor name | Role |
|---|---|
| `Floor_0` | Floor (collision ground; coins/character placed relative to this) |
| `StaticMeshActor_0` | Template_Default floor mesh |
| `PlayerStart_0` | Player spawn — character (B3) and PIE start here |
| `DirectionalLight_0` | Sun (folder `Lighting`) |
| `SkyLight_0` | Sky light (folder `Lighting`) |
| `SkyAtmosphere_0` | Sky atmosphere (folder `Lighting`) |
| `ExponentialHeightFog_0` | Fog (folder `Lighting`) |
| `VolumetricCloud_0` | Clouds (folder `Lighting`) |
| `WorldSettings_1` | World settings |

## Naming / layout conventions established

- Levels live under `/Game/Maps/`.
- Outliner folders group concerns (`Lighting`, later `Gameplay` for coins/character).
- Reference actors by **label** (`find_actors name=`) or **folder**
  (`get_actors_in_folder`); spawned-actor internal names are engine-assigned.
- Spawn assets by content path; spawn classes by class ref (SceneTools).

## Screenshot

`docs/experiments/screenshots/02-sandbox-map.png` — clean lit floor (probe removed).

## Acceptance criteria — met

- [x] `/Game/Maps/Sandbox.Sandbox` opens with floor, lighting, PlayerStart.
- [x] Object paths documented here for reuse by B4/B6.
- [x] Map committed via Git LFS (`.umap`).
