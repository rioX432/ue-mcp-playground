#!/usr/bin/env bash
# Editor-lane lock for /dev-all: guarantees only one editor-bound task drives the
# single live Unreal Editor at a time. The MCP server is loopback-only on :8000,
# so "editor up" == "something is listening on 8000".
#
# Usage:
#   scripts/editor-lane.sh status    # report lock + editor state, exit 0
#   scripts/editor-lane.sh acquire   # take the lock (fails if held or editor down)
#   scripts/editor-lane.sh release   # release the lock
set -euo pipefail

LOCK_DIR="$(cd "$(dirname "$0")/.." && pwd)/.claude/.editor-lane.lock"
PORT="${MCP_PORT:-8000}"

editor_up() {
  if command -v lsof >/dev/null 2>&1; then
    lsof -nP -iTCP:"$PORT" -sTCP:LISTEN >/dev/null 2>&1
  else
    # Fallback: probe the MCP endpoint (405 to GET means the server is alive).
    curl -s -o /dev/null --max-time 3 "http://127.0.0.1:${PORT}/mcp" 2>/dev/null
  fi
}

case "${1:-status}" in
  status)
    if [ -d "$LOCK_DIR" ]; then echo "lock: HELD ($(cat "$LOCK_DIR/owner" 2>/dev/null || echo unknown))"; else echo "lock: free"; fi
    if editor_up; then echo "editor: up (:$PORT listening)"; else echo "editor: DOWN (:$PORT not listening)"; fi
    ;;
  acquire)
    editor_up || { echo "ERROR: editor not running on :$PORT — ask the user to launch the editor" >&2; exit 3; }
    # mkdir is atomic: it fails if the lock already exists.
    if mkdir "$LOCK_DIR" 2>/dev/null; then
      printf '%s' "${LOCK_OWNER:-pid-$$}" > "$LOCK_DIR/owner"
      echo "lock: ACQUIRED by ${LOCK_OWNER:-pid-$$}"
    else
      echo "ERROR: editor lane already locked by $(cat "$LOCK_DIR/owner" 2>/dev/null || echo unknown)" >&2
      exit 4
    fi
    ;;
  release)
    rm -rf "$LOCK_DIR"
    echo "lock: released"
    ;;
  *)
    echo "usage: $0 {status|acquire|release}" >&2; exit 2 ;;
esac
