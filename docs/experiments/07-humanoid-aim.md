# 07 — Humanoid Player + Revolver + Over-the-Shoulder Aim/Fire

Presentation + feel pass on the shooter: swap the primitive cube player for a
humanoid holding a revolver, and fix the fire/aim so shots go where the crosshair
points without the camera jerking.

**Status: PASS (verified live in PIE via MCP; gameplay still 12/12 headless green).**
2026-06-22, official UE 5.8 MCP.

## What shipped

### G — Humanoid player holding a revolver (Blueprint asset swap)
Framed per the Core Value: gameplay code stays content-free on engine primitives;
the humanoid is an **optional asset swap wired on `BP_Hero`**, not a code dependency.

- C++ `AMCPPlaygroundCharacter` keeps its **primitive default** (`BodyMesh` engine
  cube) for headless builds, plus a `WeaponMesh` `UStaticMeshComponent` attached to
  the **`hand_r` bone** of the character's skeletal mesh (`SetupAttachment(GetMesh(),
  "hand_r")`) so a held weapon follows the hand through animation — content-free in C++.
- `BP_Hero` (the playable Blueprint) assigns the **Manny `SkeletalMesh` + ABP**
  (engine third-person mannequin) on the inherited `Mesh`, hides the cube, and drops
  a revolver mesh into the `WeaponMesh` slot. The grip transform (rotation / scale /
  offset on `hand_r`) was hand-tuned via MCP `ObjectTools.set_properties` so the
  revolver reads as held forward.

### Fire/aim fix (the feel work)
Three coupled bugs in the original fire path, fixed in `FireWeapon()`:

1. **Camera "zoom" on every shot** — projectiles were spawned in front of the
   *camera*; a colliding actor right at the spring arm made the boom collide and
   snap the view in. **Fix:** spawn from the **weapon muzzle**
   (`WeaponMesh->GetComponentLocation()`), near the character, never the camera.
2. **Shots didn't go to the crosshair** — **Fix:** line-trace from the camera
   center into the world (`LineTraceSingleByChannel`, `ECC_Visibility`, 10000 uu),
   take the impact point as the aim point, and orient the projectile
   `(AimPoint - MuzzleLocation).Rotation()`. Muzzle-origin, camera-direction aim.
3. **No aiming reference** — **Fix:** a `AMCPPlaygroundHUD` **center crosshair**
   (four `DrawLine` ticks at `ClipX/2, ClipY/2`) marking exactly where the trace
   originates.

Plus an **over-the-shoulder camera**: `CameraBoom->bUsePawnControlRotation = true`
with a `SocketOffset` configured + saved on `BP_Hero` (commit `8f3fc75`) so the
pawn sits to one side and the center crosshair has a clear line to the target.

## Verified live in PIE (MCP)

| Arc | Evidence |
|---|---|
| Humanoid renders holding the revolver | `CaptureEditorImage` → `screenshots/07-shooter-humanoid.png` (Manny in Sandbox, HUD Score/Wave/HP) |
| Over-shoulder framing + crosshair + coins/enemy in view | `CaptureEditorImage` → `screenshots/08-shooter-aim.png` |
| Gameplay regression | full suite **12/12 green** (no gameplay logic changed — `FireWeapon` still spawns from a muzzle offset along aim) |

![Humanoid player in Sandbox](screenshots/07-shooter-humanoid.png)
![Over-the-shoulder aim](screenshots/08-shooter-aim.png)

## Reusable findings

- **Spawning a colliding projectile in front of a spring-arm camera collapses the
  boom.** Spawn near the pawn (a muzzle socket/component), aim by tracing from the
  camera — don't co-locate the spawn with the camera.
- **Held-weapon attach is content-free in C++**: attach a mesh component to a named
  skeletal bone (`hand_r`); assign the actual mesh + tune the grip on the Blueprint
  via MCP `set_properties`. Keeps headless builds asset-free, art an editor swap.
- **The whole humanoid/grip/camera tune was done over MCP** — Blueprint property
  edits + `CaptureEditorImage` review loop, no manual editor authoring beyond what
  the agent drove.
- Camera offsets edited at runtime on a Blueprint must be **saved** to persist
  (separate commit `8f3fc75`) — an unsaved live edit is lost on relaunch.

## Acceptance

- [x] Humanoid player holds a revolver (Manny + ABP + `hand_r` weapon), primitive
      cube remains the content-free C++ default.
- [x] Shots originate at the muzzle and travel to the center crosshair; no camera zoom.
- [x] Over-the-shoulder camera with center crosshair, persisted on `BP_Hero`.
- [x] No gameplay regression — 12/12 headless tests green.
</content>
</invoke>
