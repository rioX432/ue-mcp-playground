# Golden Task 01 — Collectible Pickup

The first end-to-end proof that an AI agent can autonomously build a gameplay
mechanic in UE 5.8 via MCP. Chosen because it exercises every harness primitive in
one loop.

## Success criteria
A third-person character can walk over a coin; the coin disappears and a score
counter increments; an automation test proves the pickup increments the score
headlessly; a `capture_viewport` screenshot shows the HUD score after pickup.

## Harness primitives exercised
| Step | MCP capability |
|------|----------------|
| Spawn player + floor + coin | actor spawn / transform, explicit object paths |
| Pickup logic (score +1) | C++ component + Blueprint node editing |
| WASD movement | Enhanced Input mapping |
| Run & observe | `pie_control` + `capture_viewport(include_ui=true)` |
| Prove correctness | `Automation RunTests` (headless, `-nullrhi`) |

## Log
<!-- One entry per attempt: date, MCP tools used, what worked, what failed, screenshot path. -->

### YYYY-MM-DD — attempt 1
- MCP tools:
- Worked:
- Failed:
- Screenshot:
- Next:
