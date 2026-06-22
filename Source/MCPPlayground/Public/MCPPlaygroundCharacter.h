// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MCPPlaygroundCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UPointLightComponent;
class UHealthComponent;
class UInputAction;
class UInputMappingContext;
class AMCPProjectile;
class USoundBase;
struct FInputActionValue;

/**
 * Player character for the collectible playground. A standard third-person
 * ACharacter (spring arm + follow camera) that walks into coins.
 *
 * Enhanced Input is wired entirely in C++ (the InputActions and MappingContext are
 * created as default subobjects, not content assets) so the character is
 * self-contained and headless-buildable — no Blueprint or input assets required.
 * A visible cube body is attached so the character reads clearly in screenshots
 * without referencing skeletal-mesh content.
 */
UCLASS()
class MCPPLAYGROUND_API AMCPPlaygroundCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMCPPlaygroundCharacter();

	/** Spawns a projectile from a muzzle offset along the camera aim. Respects fire cooldown. Public for tests. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	/** Bound to Health->OnDeath: notifies the GameState that the player is down. */
	UFUNCTION()
	void HandleDeath();

	/** Bound to IA_Restart: restarts the run via the GameMode when game over. */
	void HandleRestart();

	/** Bound to Health->OnDamaged: plays the hurt sound (if set). */
	UFUNCTION()
	void HandleDamaged(float NewHealth);

	/** Turns the muzzle-flash light off (timer callback after a shot). */
	void HideMuzzleFlash();

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Health")
	TObjectPtr<UHealthComponent> Health;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	/** Simple visible body (engine cube) so the character is visible without content. */
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	/** Weapon mesh, attached to the skeletal mesh's "hand_r" bone. Assign the mesh on the Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	/** Asset-free muzzle flash: a point light pulsed on each shot. */
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UPointLightComponent> MuzzleFlash;

	FTimerHandle MuzzleFlashTimer;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY()
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY()
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY()
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY()
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY()
	TObjectPtr<UInputAction> RestartAction;

	/** Projectile to spawn when firing (defaults to AMCPProjectile). */
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AMCPProjectile> ProjectileClass;

	/** Minimum seconds between shots. */
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireCooldown = 0.25f;

	float LastFireTime = -1000.0f;

	/** Optional SFX (assign a Fab/free sound; silent if unset). */
	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundBase> HurtSound;
};
