// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MCPProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

/**
 * Simple damaging projectile: a sphere that flies straight (ProjectileMovement) and
 * damages the first thing it hits. The impact path is exposed as HandleImpact() so
 * damage behavior is unit-testable headlessly without simulating flight.
 *
 * The visual mesh is an editable UPROPERTY defaulting to an engine sphere — no
 * content asset required (Fab swap optional).
 */
UCLASS()
class MCPPLAYGROUND_API AMCPProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMCPProjectile();

	/** Damage dealt to the hit actor's UHealthComponent. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Damage = 25.0f;

	/**
	 * Applies Damage to HitActor's UHealthComponent (if present, and not the
	 * instigator/owner), then destroys the projectile. Separable from flight so
	 * tests can call it directly.
	 */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void HandleImpact(AActor* HitActor);

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> Movement;
};
