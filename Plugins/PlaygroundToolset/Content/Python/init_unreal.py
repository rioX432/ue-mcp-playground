# Copyright MCPPlayground. All Rights Reserved.

"""Auto-run on editor startup: register the Playground toolset with the registry."""

from toolset_registry.registration import Registration

from playground_toolset.screenshot_toolset import PlaygroundToolset

# Registering the class makes its @tool_call methods discoverable via the MCP
# meta-tools (list_toolsets / describe_toolset / call_tool).
Registration([PlaygroundToolset]).register()
