# Review Points — reusable insights

Self-review insights extracted while building the harness + collectible + shooter.
Check these before submitting UE C++ / MCP work.

## C++ / UE API

- **Bare `AActor` has no root** → `SetActorLocation` no-ops. Add a `USceneComponent`
  root in tests/spawns that need a position.
- **Don't shadow engine members.** A local/free `DrawLine` collides with
  `AHUD::DrawLine`; pick distinct names (`DrawTextLine`).
- **Dynamic multicast delegates** (`DECLARE_DYNAMIC_MULTICAST_DELEGATE*`) bind only
  `UFUNCTION`s via `AddDynamic` — no `AddLambda`. Test the effect, not the delegate.
- **Enhanced Input fully in C++**: create `UInputAction`/`UInputMappingContext` as
  `CreateDefaultSubobject`s; WASD→Axis2D needs `UInputModifierNegate` +
  `UInputModifierSwizzleAxis` (Order `YXZ`); mouse via `EKeys::Mouse2D`;
  `AddMappingContext` in `BeginPlay`. No content assets required.
- **Damage**: prefer a direct `UHealthComponent::ApplyDamage()` over engine
  `TakeDamage`/`UGameplayStatics::ApplyDamage` — deterministic + headless-testable.
- **Run/game state on `AGameStateBase`** (not GameMode/PlayerState) for single
  player: HUD reads it, gameplay writes it, tests construct it without PIE.
- Make gameplay **testable by design**: separable impact/fire/death methods;
  meshes & sounds as editable `UPROPERTY`s (primitive/empty default, Fab optional).

## MCP / editor loop

- Plugin enabled ≠ working: also need the server started (`bAutoStartServer=True`)
  and the editor-control toolsets enabled. See `docs/experiments/00-bootstrap.md`.
- **Editor must be CLOSED to compile new C++** (hot-reload dylib trap); the agent
  drives kill→build→relaunch unattended (auto-start + MCP client auto-reconnect).
- **No input injection** in stock MCP → drive PIE by teleporting `UEDPIE_0_<Map>`
  actors; verify via `CaptureEditorImage` + `UE_LOG` signals + `find_actors` counts.
- **`CaptureViewport`** = editor world (forced pose, no PIE HUD); **`CaptureEditorImage`**
  = live PIE view + HUD. Both base64 → `scripts/decode-capture.py` + `sips -Z`.
- Strip the auto-generated `[/Script/AndroidFileServerEditor...]` block from
  `Config/DefaultEngine.ini` before committing (machine-local `SecurityToken`).
- **Fab import is not automatable** via stock MCP — manual marketplace step; ship on
  primitives, wire dropped-in `/Game` assets via `ObjectTools.set_properties`.

## Git / hygiene

- `.uasset`/`.umap` via Git LFS; never commit `Binaries/Intermediate/Saved` or
  `__pycache__`. Don't commit timestamped `custom-*.png` tool outputs.
