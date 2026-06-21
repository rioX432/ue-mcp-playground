# 04 — BP_Coin from UPickupComponent, Placed via MCP (Issue #10, B4)

The core autonomous editor-mutation demonstration: build a placeable collectible
Blueprint from the C++ `UPickupComponent` and populate the level — entirely through
MCP tool calls, grounded at every step.

**Status: PASS.** 2026-06-22, official UE 5.8 MCP.

## Built via MCP (all real tool calls)

| Step | Tool | Result |
|---|---|---|
| create BP | `BlueprintTools.create /Game/Blueprints BP_Coin` (parent `/Script/Engine.Actor`) | `/Game/Blueprints/BP_Coin.BP_Coin` |
| add mesh | `ActorTools.add_component StaticMeshComponent "CoinMesh"` | `…BP_Coin_C:CoinMesh_GEN_VARIABLE` |
| add pickup | `ActorTools.add_component /Script/MCPPlayground.PickupComponent "Pickup"` | `…BP_Coin_C:Pickup_GEN_VARIABLE` |
| set mesh | `ObjectTools.set_properties CoinMesh` → `StaticMesh=/Engine/BasicShapes/Cylinder`, `RelativeScale3D=(1,1,0.2)` | true |
| compile | `BlueprintTools.compile_blueprint` | ok |
| **verify ref** | `ObjectTools.get_class Pickup_GEN_VARIABLE` | `/Script/MCPPlayground.PickupComponent` ✓ |
| save BP | `AssetTools.save_assets ["/Game/Blueprints/BP_Coin"]` | true |
| place ×3 | `SceneTools.add_to_scene_from_asset /Game/Blueprints/BP_Coin` @ (300,-200,50)/(300,0,50)/(300,200,50) | `BP_Coin_C_0/1/2` |
| organize | `set_actor_folder "Gameplay/Coins"` ×3 | ok |
| save level | `AssetTools.save_assets ["/Game/Maps/Sandbox"]` | true |
| **re-query** | `SceneTools.get_actors_in_folder "Gameplay/Coins"` | 3 actors ✓ |
| screenshot | `CaptureViewport` | `screenshots/04-coins-placed.png` (3 discs on the floor) |

## Object paths (placed coins)

- `/Game/Maps/Sandbox.Sandbox:PersistentLevel.BP_Coin_C_0` @ (300, -200, 50)
- `/Game/Maps/Sandbox.Sandbox:PersistentLevel.BP_Coin_C_1` @ (300, 0, 50)
- `/Game/Maps/Sandbox.Sandbox:PersistentLevel.BP_Coin_C_2` @ (300, 200, 50)

Blueprint: `/Game/Blueprints/BP_Coin` (Actor + `CoinMesh` cylinder + `Pickup` =
`UPickupComponent`). The pickup creates its own overlap sphere at BeginPlay.

## Learnings

- Create a BP with `BlueprintTools.create(folder, name, asset_type=parent class ref)`.
- Add components to a BP asset with `ActorTools.add_component(owner=BP ref, …)`;
  the returned ref is the `..._C:<Name>_GEN_VARIABLE` template sub-object.
- Set component properties on that template with `ObjectTools.set_properties`
  (values = JSON string; object props take content paths).
- `ActorTools.get_components` needs an **actor instance**, not a BP asset — verify a
  BP's component class via `ObjectTools.get_class` on the `_GEN_VARIABLE` ref.

## Acceptance criteria — met

- [x] `BP_Coin` references `UPickupComponent` (verified via `get_class`).
- [x] 3 coins placed at documented object paths; post-save re-query confirms all 3.
- [x] `CaptureViewport` shows 3 coins on the floor.
