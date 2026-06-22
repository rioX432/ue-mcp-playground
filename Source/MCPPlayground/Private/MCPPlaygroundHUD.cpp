// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPPlaygroundHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "CanvasItem.h"
#include "GameFramework/Pawn.h"
#include "HealthComponent.h"
#include "MCPPlaygroundGameState.h"

namespace
{
	void DrawTextLine(UCanvas* Canvas, const FString& Text, float X, float Y, float Scale, const FLinearColor& Color)
	{
		FCanvasTextItem Item(FVector2D(X, Y), FText::FromString(Text), GEngine->GetLargeFont(), Color);
		Item.Scale = FVector2D(Scale, Scale);
		Item.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(Item);
	}
}

void AMCPPlaygroundHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	const AMCPPlaygroundGameState* GameState = GetWorld() ? GetWorld()->GetGameState<AMCPPlaygroundGameState>() : nullptr;
	const int32 Score = GameState ? GameState->Score : 0;
	const int32 Wave = GameState ? GameState->Wave : 0;
	const bool bGameOver = GameState ? GameState->bGameOver : false;

	// Player HP from the controlled pawn's HealthComponent.
	float Hp = 0.0f;
	float MaxHp = 0.0f;
	if (const APawn* Pawn = GetOwningPawn())
	{
		if (const UHealthComponent* Health = Pawn->FindComponentByClass<UHealthComponent>())
		{
			Hp = Health->Health;
			MaxHp = Health->MaxHealth;
		}
	}

	DrawTextLine(Canvas, FString::Printf(TEXT("Score: %d"), Score), 50.0f, 50.0f, 2.0f, FLinearColor::White);
	DrawTextLine(Canvas, FString::Printf(TEXT("Wave: %d"), Wave), 50.0f, 90.0f, 2.0f, FLinearColor::White);
	DrawTextLine(Canvas, FString::Printf(TEXT("HP: %.0f / %.0f"), Hp, MaxHp), 50.0f, 130.0f, 2.0f,
		Hp <= MaxHp * 0.34f ? FLinearColor::Red : FLinearColor::White);

	if (bGameOver)
	{
		DrawTextLine(Canvas, TEXT("GAME OVER"), Canvas->ClipX * 0.5f - 180.0f, Canvas->ClipY * 0.5f - 40.0f, 4.0f, FLinearColor::Red);
		DrawTextLine(Canvas, TEXT("Press R to restart"), Canvas->ClipX * 0.5f - 150.0f, Canvas->ClipY * 0.5f + 30.0f, 1.5f, FLinearColor::Yellow);
	}
	else
	{
		// Center crosshair: shots are fired toward the screen center.
		const float CX = Canvas->ClipX * 0.5f;
		const float CY = Canvas->ClipY * 0.5f;
		const float Gap = 6.0f;
		const float Len = 14.0f;
		const FLinearColor CrosshairColor(0.1f, 1.0f, 0.1f);
		DrawLine(CX - Gap - Len, CY, CX - Gap, CY, CrosshairColor, 2.0f);
		DrawLine(CX + Gap, CY, CX + Gap + Len, CY, CrosshairColor, 2.0f);
		DrawLine(CX, CY - Gap - Len, CX, CY - Gap, CrosshairColor, 2.0f);
		DrawLine(CX, CY + Gap, CX, CY + Gap + Len, CrosshairColor, 2.0f);
	}
}
