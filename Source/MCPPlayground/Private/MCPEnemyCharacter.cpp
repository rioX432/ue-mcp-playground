// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPEnemyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MCPPlaygroundGameState.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

AMCPEnemyCharacter::AMCPEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	BodyMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.8f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(ConeMesh.Object);
	}
}

void AMCPEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (Health)
	{
		Health->OnDeath.AddDynamic(this, &AMCPEnemyCharacter::HandleEnemyDeath);
	}
}

FVector AMCPEnemyCharacter::ComputeMoveDirectionToTarget(const AActor* Target) const
{
	if (!Target)
	{
		return FVector::ZeroVector;
	}
	FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.0f;
	return ToTarget.GetSafeNormal();
}

void AMCPEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player)
	{
		return;
	}

	// Chase.
	AddMovementInput(ComputeMoveDirectionToTarget(Player));

	// Contact damage on a cooldown.
	const float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	if (Distance <= ContactRange && (Now - LastContactTime) >= ContactInterval)
	{
		LastContactTime = Now;
		if (UHealthComponent* PlayerHealth = Player->FindComponentByClass<UHealthComponent>())
		{
			PlayerHealth->ApplyDamage(ContactDamage);
		}
	}
}

void AMCPEnemyCharacter::HandleEnemyDeath()
{
	if (const UWorld* World = GetWorld())
	{
		if (AMCPPlaygroundGameState* GameState = World->GetGameState<AMCPPlaygroundGameState>())
		{
			GameState->RegisterEnemyKilled();
		}
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	Destroy();
}
