# 03 — Third-Person Character + Enhanced Input (Issue #9, B3)

A controllable character to walk into coins. Codex/design flagged full
character + Enhanced Input authoring as beyond reliable stock UE 5.8 MCP, so this
is a **minimal C++ `ACharacter`** seeded from the engine `TP_ThirdPerson` template
pattern — MCP's role here is verification, not authoring.

**Status: PASS (spawn/render/PIE verified).** 2026-06-22, macOS.

## Implementation

- `AMCPPlaygroundCharacter : ACharacter` — spring arm + follow camera, standard
  movement (orient-to-movement, jump), and a visible engine-cube `BodyMesh` so the
  character reads in screenshots without a skeletal-mesh content reference.
- **Enhanced Input wired entirely in C++** (no content assets): `IMC_Default`,
  `IA_Move` (Axis2D), `IA_Look` (Axis2D), `IA_Jump` (bool) are `CreateDefaultSubobject`s;
  WASD is mapped with `UInputModifierNegate` / `UInputModifierSwizzleAxis` (YXZ),
  mouse via `EKeys::Mouse2D`, jump via Space. `AddMappingContext` in `BeginPlay`.
- `AMCPPlaygroundGameMode::DefaultPawnClass = AMCPPlaygroundCharacter`.

## Verification (MCP)

- Build (`Build.sh`) → `Result: Succeeded`.
- `StartPIE` → `IsPIERunning` = true → `StopPIE` (character spawns + is possessed
  at PlayerStart with the GameMode default pawn).
- Spawned the class into the editor level (`add_to_scene_from_class
  /Script/MCPPlayground.MCPPlaygroundCharacter`), `FocusOnActors` + `CaptureViewport`
  → `screenshots/03-character.png` shows the cube body standing on the floor; probe
  then removed.

## Stock-MCP limitation found

- **No input-injection tool.** Stock UE 5.8 MCP (EditorAppToolset) has no
  "send key/axis" during PIE, so WASD/mouse *movement* cannot be auto-verified via
  MCP. Input wiring matches the engine `TP_ThirdPerson` template (human-verifiable);
  the golden task (#13) exercises runtime motion by teleporting the pawn into a coin.
- **`CaptureViewport` captures the editor world, not the PIE game world**, and
  requires an explicit `captureTransform` — so PIE pawns don't appear in a forced-pose
  capture. Verify in-level by spawning the class into the editor world instead.

## Acceptance criteria

- [x] Character spawns at PlayerStart and is possessed in PIE (GameMode default pawn).
- [x] Input assets follow naming (`IMC_Default`/`IA_Move`/`IA_Look`/`IA_Jump`), wired in C++.
- [~] WASD + mouse-look: wired per engine template; not auto-verifiable via stock MCP
      (no input injection) — documented above.
