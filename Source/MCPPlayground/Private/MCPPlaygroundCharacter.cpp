// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPPlaygroundCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MCPPlaygroundGameMode.h"
#include "MCPPlaygroundGameState.h"
#include "MCPProjectile.h"
#include "Sound/SoundBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

AMCPPlaygroundCharacter::AMCPPlaygroundCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// Let the controller rotation drive the camera, not the body.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Visible body (engine cube) attached to the capsule so the character shows up in
	// screenshots without a skeletal-mesh content reference.
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	BodyMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.8f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
	}

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	// Weapon held in the right hand. The mesh asset is assigned on the Blueprint
	// (BP_Hero) so C++ stays content-free; here we just attach it to the Manny
	// skeleton's "hand_r" bone so it follows the hand through animation.
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("hand_r"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Asset-free muzzle flash: a point light, off until a shot pulses it.
	MuzzleFlash = CreateDefaultSubobject<UPointLightComponent>(TEXT("MuzzleFlash"));
	MuzzleFlash->SetupAttachment(GetCapsuleComponent());
	MuzzleFlash->SetIntensity(8000.0f);
	MuzzleFlash->SetLightColor(FLinearColor(1.0f, 0.85f, 0.4f));
	MuzzleFlash->SetAttenuationRadius(400.0f);
	MuzzleFlash->SetCastShadows(false);
	MuzzleFlash->SetVisibility(false);

	// --- Enhanced Input, fully in C++ (no content assets) ---
	DefaultMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("IMC_Default"));

	MoveAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Move"));
	MoveAction->ValueType = EInputActionValueType::Axis2D;

	LookAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Look"));
	LookAction->ValueType = EInputActionValueType::Axis2D;

	JumpAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Jump"));
	JumpAction->ValueType = EInputActionValueType::Boolean;

	FireAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Fire"));
	FireAction->ValueType = EInputActionValueType::Boolean;

	RestartAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Restart"));
	RestartAction->ValueType = EInputActionValueType::Boolean;

	ProjectileClass = AMCPProjectile::StaticClass();

	// WASD -> Move (Axis2D): X = right (A/D), Y = forward (W/S).
	// D = +X (no modifier); A = -X (Negate); W = +Y (Swizzle X->Y); S = -Y (Negate + Swizzle).
	DefaultMappingContext->MapKey(MoveAction, EKeys::D);

	{
		FEnhancedActionKeyMapping& M = DefaultMappingContext->MapKey(MoveAction, EKeys::A);
		M.Modifiers.Add(NewObject<UInputModifierNegate>(DefaultMappingContext));
	}
	{
		FEnhancedActionKeyMapping& M = DefaultMappingContext->MapKey(MoveAction, EKeys::W);
		M.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext));
	}
	{
		FEnhancedActionKeyMapping& M = DefaultMappingContext->MapKey(MoveAction, EKeys::S);
		M.Modifiers.Add(NewObject<UInputModifierNegate>(DefaultMappingContext));
		M.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(DefaultMappingContext));
	}

	// Mouse XY -> Look (Axis2D).
	DefaultMappingContext->MapKey(LookAction, EKeys::Mouse2D);

	// Space -> Jump.
	DefaultMappingContext->MapKey(JumpAction, EKeys::SpaceBar);

	// Left mouse -> Fire.
	DefaultMappingContext->MapKey(FireAction, EKeys::LeftMouseButton);

	// R -> Restart (after game over).
	DefaultMappingContext->MapKey(RestartAction, EKeys::R);
}

void AMCPPlaygroundCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Health)
	{
		Health->OnDeath.AddDynamic(this, &AMCPPlaygroundCharacter::HandleDeath);
		Health->OnDamaged.AddDynamic(this, &AMCPPlaygroundCharacter::HandleDamaged);
	}

	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMCPPlaygroundCharacter::HandleDeath()
{
	if (const UWorld* World = GetWorld())
	{
		if (AMCPPlaygroundGameState* GameState = World->GetGameState<AMCPPlaygroundGameState>())
		{
			GameState->SetPlayerDead();
		}
	}
}

void AMCPPlaygroundCharacter::HandleDamaged(float /*NewHealth*/)
{
	if (HurtSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HurtSound, GetActorLocation());
	}
}

void AMCPPlaygroundCharacter::HandleRestart()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	const AMCPPlaygroundGameState* GameState = World->GetGameState<AMCPPlaygroundGameState>();
	if (GameState && GameState->bGameOver)
	{
		if (AMCPPlaygroundGameMode* GameMode = World->GetAuthGameMode<AMCPPlaygroundGameMode>())
		{
			GameMode->RestartGame();
		}
	}
}

void AMCPPlaygroundCharacter::FireWeapon()
{
	UWorld* World = GetWorld();
	if (!World || !ProjectileClass)
	{
		return;
	}

	// Fire-rate gate.
	const float Now = World->GetTimeSeconds();
	if (Now - LastFireTime < FireCooldown)
	{
		return;
	}
	LastFireTime = Now;

	// Aim along the camera (third-person center-screen feel), spawn from a muzzle
	// offset ahead of the camera. Self-collision is ignored via the instigator.
	const FVector CamLocation = FollowCamera ? FollowCamera->GetComponentLocation() : GetActorLocation();
	const FRotator AimRotation = FollowCamera ? FollowCamera->GetComponentRotation() : GetActorRotation();
	const FVector MuzzleLocation = CamLocation + AimRotation.Vector() * 150.0f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<AMCPProjectile>(ProjectileClass, MuzzleLocation, AimRotation, SpawnParams);

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLocation);
	}

	// Pulse the muzzle flash for a few frames.
	if (MuzzleFlash)
	{
		MuzzleFlash->SetWorldLocation(MuzzleLocation);
		MuzzleFlash->SetVisibility(true);
		World->GetTimerManager().SetTimer(MuzzleFlashTimer, this, &AMCPPlaygroundCharacter::HideMuzzleFlash, 0.06f, false);
	}
}

void AMCPPlaygroundCharacter::HideMuzzleFlash()
{
	if (MuzzleFlash)
	{
		MuzzleFlash->SetVisibility(false);
	}
}

void AMCPPlaygroundCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, this, &AMCPPlaygroundCharacter::FireWeapon);
		EnhancedInput->BindAction(RestartAction, ETriggerEvent::Started, this, &AMCPPlaygroundCharacter::HandleRestart);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMCPPlaygroundCharacter::Move);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMCPPlaygroundCharacter::Look);
	}
}

void AMCPPlaygroundCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMCPPlaygroundCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
