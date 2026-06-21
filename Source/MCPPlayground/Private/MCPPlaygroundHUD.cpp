// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPPlaygroundHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "CanvasItem.h"
#include "MCPPlaygroundGameState.h"

void AMCPPlaygroundHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	int32 Score = 0;
	if (const UWorld* World = GetWorld())
	{
		if (const AMCPPlaygroundGameState* GameState = World->GetGameState<AMCPPlaygroundGameState>())
		{
			Score = GameState->Score;
		}
	}

	const FString ScoreText = FString::Printf(TEXT("Score: %d"), Score);
	FCanvasTextItem TextItem(
		FVector2D(50.0f, 50.0f),
		FText::FromString(ScoreText),
		GEngine->GetLargeFont(),
		FLinearColor::White);
	TextItem.Scale = FVector2D(2.0f, 2.0f);
	TextItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(TextItem);
}
