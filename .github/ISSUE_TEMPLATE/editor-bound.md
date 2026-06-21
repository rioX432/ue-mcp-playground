---
name: Editor-bound task (MCP)
about: A task that mutates live editor state via MCP (level/actor/asset/Blueprint). Runs in the serial editor lane.
title: "[editor-bound] "
labels: editor-bound
---

## Background / Goal
<!-- What playable/harness outcome does this produce? Link the Core Value. -->

Core Value:

## Lane
**editor-bound** — needs the live Unreal Editor + MCP server (`:8000`) + the
`EditorToolset` / `AutomationTestToolset` plugins. Never overlaps another
editor-bound task (acquire `scripts/editor-lane.sh` first).

## Target map
<!-- e.g. /Game/Maps/Sandbox.Sandbox — the level this operates on. -->

## Explicit object paths
<!-- Real paths, never invented. Discover via get/list tools first.
Actor: /Game/Maps/Sandbox.Sandbox:PersistentLevel.<Name>
Asset: /Game/Folder/<AssetName>  Engine asset: /Engine/BasicShapes/Cube -->

## Pre-state query (ground before acting)
<!-- Which MCP reads establish current state? e.g.
list_toolsets; SceneTools get level actors; AssetTools search; ObjectTools lookup class. -->

## Action (MCP ops)
<!-- The mutation steps via call_tool(toolset, tool, args). -->

## Post-state query (verify it persisted)
<!-- Re-query the same paths after save to prove the change stuck. -->

## Build / test command
<!-- If new C++ symbols are needed first: close editor → Build.sh → relaunch.
Headless test (if any): scripts/run-tests.sh <Filter> -->

## PIE / screenshot artifact
<!-- pie_control (StartPIE) → CaptureViewport → save to docs/experiments/screenshots/.
Note that captures are large base64; decode + downscale before review. -->

## Rollback / cleanup plan
<!-- How to undo if wrong: delete spawned actors by path, revert asset, restore map. -->

## Acceptance Criteria
- [ ]
- [ ]
