// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MCPPlaygroundCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UInputAction;
class UInputMappingContext;
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

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	/** Simple visible body (engine cube) so the character is visible without content. */
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY()
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY()
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY()
	TObjectPtr<UInputAction> JumpAction;
};
