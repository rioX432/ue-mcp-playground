// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MCPPlaygroundHUD.generated.h"

/**
 * Minimal debug HUD that draws the current collectible score as on-screen text.
 *
 * Implemented in C++ via AHUD::DrawHUD (not UMG) on purpose: UMG authoring through
 * stock UE 5.8 MCP is the unreliable part, whereas a C++ HUD stays
 * headless-buildable and is wired purely from code.
 */
UCLASS()
class MCPPLAYGROUND_API AMCPPlaygroundHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
