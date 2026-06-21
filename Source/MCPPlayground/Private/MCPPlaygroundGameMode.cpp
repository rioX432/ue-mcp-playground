// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPPlaygroundGameMode.h"

#include "MCPPlaygroundCharacter.h"
#include "MCPPlaygroundGameState.h"
#include "MCPPlaygroundHUD.h"

AMCPPlaygroundGameMode::AMCPPlaygroundGameMode()
{
	GameStateClass = AMCPPlaygroundGameState::StaticClass();
	HUDClass = AMCPPlaygroundHUD::StaticClass();
	DefaultPawnClass = AMCPPlaygroundCharacter::StaticClass();
}
