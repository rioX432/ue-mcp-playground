# Copyright MCPPlayground. All Rights Reserved.

"""A custom AI toolset for MCPPlayground.

Demonstrates that the harness can extend the official UE 5.8 MCP surface with
project-specific tools: a ToolsetDefinition written in Python and registered with
the Toolset Registry so it shows up in `list_toolsets` / `call_tool` from any MCP
client (Claude Code).
"""

import datetime
import os
import re

import unreal

import toolset_registry


@unreal.uclass()
class PlaygroundToolset(unreal.ToolsetDefinition):
    """MCPPlayground project tools."""

    @toolset_registry.tool_call
    @staticmethod
    def take_labeled_screenshot(label: str) -> str:
        """Capture the editor viewport to a timestamped, labeled PNG under
        docs/experiments/screenshots/ and return the file path.

        The screenshot is requested via the high-res screenshot system and is
        written asynchronously (it appears within ~1 second, on a subsequent
        render frame).

        Args:
            label: Short human label included in the file name.

        Returns:
            The absolute path the screenshot will be written to.
        """
        safe_label = re.sub(r"[^A-Za-z0-9_-]+", "_", label).strip("_") or "shot"
        timestamp = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
        out_dir = os.path.join(
            unreal.Paths.project_dir(), "docs", "experiments", "screenshots"
        )
        os.makedirs(out_dir, exist_ok=True)
        out_path = os.path.normpath(
            os.path.join(out_dir, f"custom-{safe_label}-{timestamp}.png")
        )

        unreal.AutomationLibrary.take_high_res_screenshot(1280, 720, out_path)

        unreal.log(f"[PlaygroundToolset] take_labeled_screenshot -> {out_path}")
        return out_path
