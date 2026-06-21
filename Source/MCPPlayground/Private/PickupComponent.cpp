// Copyright Epic Games, Inc. All Rights Reserved.

#include "PickupComponent.h"

#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "MCPPlaygroundGameState.h"

UPickupComponent::UPickupComponent()
{
	// No tick needed; everything is event driven.
	PrimaryComponentTick.bCanEverTick = false;
}

void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Self-contained overlap detection so a Blueprint only has to add this component.
	OverlapSphere = NewObject<USphereComponent>(Owner, TEXT("PickupOverlapSphere"));
	OverlapSphere->InitSphereRadius(PickupRadius);
	OverlapSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	OverlapSphere->SetGenerateOverlapEvents(true);

	if (USceneComponent* Root = Owner->GetRootComponent())
	{
		OverlapSphere->SetupAttachment(Root);
	}
	else
	{
		Owner->SetRootComponent(OverlapSphere);
	}

	OverlapSphere->RegisterComponent();
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &UPickupComponent::HandleBeginOverlap);
}

void UPickupComponent::HandleBeginOverlap(
	UPrimitiveComponent* /*OverlappedComp*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/,
	bool /*bFromSweep*/,
	const FHitResult& /*SweepResult*/)
{
	Consume(OtherActor);
}

bool UPickupComponent::Consume(AActor* Collector)
{
	// Exactly-once gate first, so repeated overlaps and post-destroy calls are no-ops.
	if (bConsumed || !IsValid(Collector) || Collector == GetOwner())
	{
		return false;
	}

	bConsumed = true;

	if (const UWorld* World = GetWorld())
	{
		if (AMCPPlaygroundGameState* GameState = World->GetGameState<AMCPPlaygroundGameState>())
		{
			GameState->AddScore(ScoreValue);
		}
	}

	OnPickedUp.Broadcast(Collector);

	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorEnableCollision(false);
		Owner->Destroy();
	}

	return true;
}
