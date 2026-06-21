#!/usr/bin/env python3
"""Decode a CaptureViewport MCP tool-result (JSON with base64 PNG) to a PNG file.

The official UE MCP returns the screenshot as a ~2-4MB base64 string that overflows
the agent context, so the harness saves the raw tool result to a file. This helper
extracts returnValue.image.data and writes a PNG. Usage:
    decode-capture.py <tool-result.txt> <out.png>
"""
import sys, json, base64

src, out = sys.argv[1], sys.argv[2]
obj = json.loads(open(src).read())
rv = obj["returnValue"]
img = rv["image"]
data = base64.b64decode(img["data"])
open(out, "wb").write(data)
labeled = rv.get("labeledActors", [])
print(f"mime={img['mimeType']} bytes={len(data)} labeledActors={len(labeled)}")
cam = rv.get("cameraLocation")
if cam:
    print(f"camera=({cam['x']:.0f},{cam['y']:.0f},{cam['z']:.0f}) fov={rv.get('cameraFOV')}")
print(f"saved={out}")
