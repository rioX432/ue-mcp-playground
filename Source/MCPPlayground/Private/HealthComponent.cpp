// Copyright Epic Games, Inc. All Rights Reserved.

#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

void UHealthComponent::ApplyDamage(float Amount)
{
	if (bDeathBroadcast || Amount <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health - Amount, 0.0f, MaxHealth);

	if (Health <= 0.0f)
	{
		bDeathBroadcast = true;
		OnDeath.Broadcast();
	}
}
