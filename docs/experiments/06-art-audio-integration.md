# 06 — Art & Audio Integration (Issues #37 F1, #38 F2)

Lightweight presentation polish for the shooter, framed per the amended Core Value:
the harness **integrates** sourced assets; it does not author/source production art.
Gameplay must work on engine primitives with no asset dependency.

**Status: PASS (integration scaffolding in place + asset-free muzzle flash).**

## What shipped

### F1 — SFX hooks (asset-pluggable, silent default)
- `UHealthComponent::OnDamaged` delegate (non-lethal hits).
- `USoundBase*` slots played via `UGameplayStatics::PlaySoundAtLocation` **only when set**:
  - `FireSound` (character fire), `ImpactSound` (projectile hit),
    `DeathSound` (enemy death), `HurtSound` (player, via OnDamaged).
- Unset → silent → never blocks the loop.

### F2 — Muzzle flash + mesh swappability
- **Asset-free muzzle flash**: a `UPointLightComponent` on the character, pulsed
  ~0.06 s on each shot (no particle asset needed). Demonstrable without any import.
- All visual bodies are **editable `UPROPERTY` meshes** defaulting to engine
  primitives — player cube, enemy cone, projectile sphere — so a Fab mesh can be
  dropped in per-instance/Blueprint without code changes.

## Fab / asset-integration findings (the key reusable knowledge)

- **No usable gameplay assets ship in `/Engine`.** `find_assets /Engine name=Sound`
  → only editor icons/materials; `Spark`/`Muzzle` particles → none. So an audible
  SFX or a particle muzzle flash **cannot be produced autonomously** from engine
  content alone.
- **Fab import is NOT automatable via stock UE 5.8 MCP.** There is no MCP/editor-
  scripting tool to browse or download Fab marketplace items; it is a manual
  Fab-plugin / "Add to Project" step in the editor UI. (Confirmed against the
  available toolsets — `AssetTools` can read/move/duplicate/delete assets already
  in `/Game` or `/Engine`, but cannot fetch external assets.)
- **Therefore the division of labor is:** the autonomous loop ships gameplay on
  primitives + code hooks (sounds, mesh slots, muzzle light); a human (or a future
  custom MCP tool) adds a Fab/free `SoundWave`/`StaticMesh`/`Niagara` into `/Game`,
  then the agent can wire it via MCP (`ObjectTools.set_properties` on the slot, as
  done for `BP_Coin`'s mesh in #10).

## Verification

- Build + full automation suite **12/12 green** with all hooks compiled in.
- Live A/V is not MCP-verifiable: SFX and the muzzle flash trigger on **fire**, and
  stock MCP has no input injection (documented in `05-shooter-loop.md`). A human
  pressing fire (or dropping in assets) confirms them.

## Acceptance

- [x] F1: SFX hooks on the four events; silent/non-blocking without assets.
- [x] F2: asset-free muzzle flash; meshes are editable slots (primitive default).
- [x] Fab-integration findings documented (manual add; agent wires once present).
