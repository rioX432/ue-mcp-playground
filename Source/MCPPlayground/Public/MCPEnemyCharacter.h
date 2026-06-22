// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MCPEnemyCharacter.generated.h"

class UHealthComponent;
class UStaticMeshComponent;

/**
 * Enemy that chases the player and damages it on contact; awards score on death.
 *
 * Chase needs no NavMesh: each Tick it moves toward the player via AddMovementInput
 * (direction from the pure, testable ComputeMoveDirectionToTarget). On death it
 * notifies the GameState (score + EnemiesAlive--) and removes itself. Visual body is
 * an engine primitive (editable UPROPERTY; Fab swap optional).
 */
UCLASS()
class MCPPLAYGROUND_API AMCPEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMCPEnemyCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float ContactDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float ContactRange = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float ContactInterval = 1.0f;

	/** Normalized XY direction from this enemy toward Target; zero if no target. */
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	FVector ComputeMoveDirectionToTarget(const AActor* Target) const;

	virtual void Tick(float DeltaSeconds) override;

	/** Bound to Health->OnDeath: reports the kill to the GameState and removes the enemy. Public for tests. */
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void HandleEnemyDeath();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Enemy")
	TObjectPtr<UHealthComponent> Health;

	UPROPERTY(VisibleAnywhere, Category = "Enemy")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	float LastContactTime = -1000.0f;
};
