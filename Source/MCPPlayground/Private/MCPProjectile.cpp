// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HealthComponent.h"
#include "UObject/ConstructorHelpers.h"

AMCPProjectile::AMCPProjectile()
{
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(16.0f);
	CollisionSphere->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionSphere->SetNotifyRigidBodyCollision(true); // simulation-generated hit events
	RootComponent = CollisionSphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);
	Mesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		Mesh->SetStaticMesh(SphereMesh.Object);
	}

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 3000.0f;
	Movement->MaxSpeed = 3000.0f;
	Movement->ProjectileGravityScale = 0.0f;
	Movement->bRotationFollowsVelocity = true;

	CollisionSphere->OnComponentHit.AddDynamic(this, &AMCPProjectile::OnHit);

	InitialLifeSpan = 3.0f;
}

void AMCPProjectile::OnHit(UPrimitiveComponent* /*HitComp*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/,
	FVector /*NormalImpulse*/, const FHitResult& /*Hit*/)
{
	HandleImpact(OtherActor);
}

void AMCPProjectile::HandleImpact(AActor* HitActor)
{
	if (HitActor && HitActor != GetInstigator() && HitActor != GetOwner())
	{
		if (UHealthComponent* HitHealth = HitActor->FindComponentByClass<UHealthComponent>())
		{
			HitHealth->ApplyDamage(Damage);
		}
	}

	Destroy();
}
