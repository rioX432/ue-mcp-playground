# 05 — Wave-Survival Shooter: Editor Golden Verification (Issue #13/V1)

Live PIE proof, via MCP, of the third-person wave-survival loop built in Epics C–E.

**Status: PASS (survival loop verified live; shoot→kill→score verified headlessly).**
2026-06-22, official UE 5.8 MCP.

## What runs

- `AMCPWaveManager` placed in `/Game/Maps/Sandbox` spawns escalating waves of
  `AMCPEnemyCharacter` in a ring; enemies chase the player and contact-damage it;
  player `UHealthComponent` death → `GameState->SetPlayerDead()` → GAME OVER;
  `R` restarts via `GameMode::RestartGame`. HUD shows Score / Wave / HP / GAME OVER.

## Verified live in PIE (MCP)

| Arc | Evidence |
|---|---|
| Wave spawns | `find_actors "Enemy"` → 3 `MCPEnemyCharacter` in `UEDPIE_0_Sandbox`; log `[MCPPlayground] Wave 1 started, 3 enemies` |
| Chase + contact damage | teleported the PIE pawn onto an enemy; enemies dealt contact damage |
| Player death → GAME OVER | log `[MCPPlayground] Player died -> GAME OVER (wave 1, score 0)` (~within seconds of contact) |
| HUD GAME OVER | `CaptureEditorImage` → `screenshots/06-shooter-gameover.png` (red "GAME OVER", HUD Score/Wave/HP) |

## Verified headlessly (not drivable via stock MCP)

`shoot → kill → score` and `restart` are not PIE-driven here because **stock UE 5.8
MCP has no input-injection tool** (can't press fire/R) — movement is faked by
teleporting the PIE pawn. These arcs are proven by automation tests:

- `MCPPlayground.Firing.SpawnsProjectile` — fire spawns a projectile (+ cooldown).
- `MCPPlayground.Projectile.Impact` — projectile damages a target's HealthComponent.
- `MCPPlayground.Enemy.ChaseAndKill` — enemy death → score +1, EnemiesAlive--.
- `MCPPlayground.Health.Reset` — restart heals the player.
- Full suite: **12/12 green**.

## Reusable findings

- **PIE world is addressable** via the `UEDPIE_0_<Map>` package — `find_actors` /
  `set_actor_transform` reach live pawns/enemies; teleporting the pawn substitutes
  for unavailable input.
- An idle player standing in enemies dies in seconds — contact-damage cooldown +
  multiple chasers make the survival loop self-driving for verification.
- The map carries both the original `BP_Coin`s (collectible bonus) and the
  `WaveManager` — they coexist.

## Acceptance

- [x] Death → GAME OVER demonstrated live with HUD screenshot + log.
- [x] Wave spawn + chase + contact damage demonstrated live.
- [~] shoot → kill → score: headless-verified (no MCP input injection for live fire).
