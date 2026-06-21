#!/usr/bin/env bash
# Headless UE automation test runner (macOS / Linux).
# Requires: UE_ROOT (engine root) and PROJECT (abs path to .uproject) env vars.
# Usage: UE_ROOT=... PROJECT=... scripts/run-tests.sh [TestFilter]
set -euo pipefail

: "${UE_ROOT:?set UE_ROOT to your UE 5.8 engine root}"
: "${PROJECT:?set PROJECT to the absolute path of MCPPlayground.uproject}"
FILTER="${1:-MCPPlayground}"

case "$(uname -s)" in
  Darwin) CMD="$UE_ROOT/Engine/Binaries/Mac/UnrealEditor-Cmd" ;;
  Linux)  CMD="$UE_ROOT/Engine/Binaries/Linux/UnrealEditor-Cmd" ;;
  *) echo "Unsupported OS for this script; use scripts/run-tests.bat on Windows" >&2; exit 2 ;;
esac

"$CMD" "$PROJECT" \
  -nullrhi -unattended -nopause -nosplash \
  -ExecCmds="Automation RunTests ${FILTER};quit" \
  -testexit="Automation Test Queue Empty" \
  -log -ReportExportPath="$(dirname "$0")/../Saved/TestReports"
