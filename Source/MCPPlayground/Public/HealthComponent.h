// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

/** Fired once when health reaches zero. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

/**
 * Reusable health/damage for both the player and enemies.
 *
 * Damage is applied by **direct `ApplyDamage()` calls**, not the engine
 * TakeDamage/UGameplayStatics path — deterministic and trivially unit-testable
 * under -nullrhi.
 */
UCLASS(ClassGroup = (MCPPlayground), meta = (BlueprintSpawnableComponent))
class MCPPLAYGROUND_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float Health = 100.0f;

	/** Subtracts Amount (ignored if <= 0 or already dead); clamps to [0, MaxHealth]; broadcasts OnDeath once at zero. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDead() const { return Health <= 0.0f; }

	/** Restores full health and re-arms the death broadcast (used on restart). */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetHealth();

	/** Broadcast exactly once when health first reaches zero. */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeath OnDeath;

protected:
	virtual void BeginPlay() override;

private:
	bool bDeathBroadcast = false;
};
