// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickupComponent.generated.h"

class USphereComponent;

/** Fired when this pickup is collected; carries the collecting actor. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickedUp, AActor*, Collector);

/**
 * Reusable collectible behavior. Attach to any actor (e.g. BP_Coin) to make it
 * award score on overlap and then remove itself. The component creates its own
 * overlap sphere at BeginPlay, so a Blueprint only needs to add this component.
 *
 * The "award exactly once" guarantee lives in Consume(), guarded by bConsumed, so
 * it is robust to repeated overlap events and is directly unit-testable without
 * physics or PIE.
 */
UCLASS(ClassGroup = (MCPPlayground), meta = (BlueprintSpawnableComponent))
class MCPPLAYGROUND_API UPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPickupComponent();

	/** Score awarded to the GameState when collected. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	int32 ScoreValue = 1;

	/** Radius (cm) of the auto-created overlap sphere. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	float PickupRadius = 100.0f;

	/** Broadcast once when the pickup is consumed. */
	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FOnPickedUp OnPickedUp;

	/**
	 * Core collect logic: awards ScoreValue to the GameState exactly once,
	 * broadcasts OnPickedUp, then destroys the owner. Safe to call repeatedly —
	 * subsequent calls are no-ops. Returns true only on the call that consumed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool Consume(AActor* Collector);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/** Auto-created at BeginPlay; owns the overlap detection. */
	UPROPERTY()
	TObjectPtr<USphereComponent> OverlapSphere;

	/** Exactly-once gate. */
	bool bConsumed = false;
};
