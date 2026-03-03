#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapActor.generated.h"

UCLASS()
class TD_6_API ATrapActor : public AActor
{
	GENERATED_BODY()

private:
	TWeakObjectPtr<class AMyCharacter> SnaredCharacter = nullptr;

	FTimerHandle SnareTimerHandle;

	float SnareTime = 3.f;

	bool bIsTriggered = false;

protected:
	ATrapActor();

	UPROPERTY(VisibleAnywhere, Category = "Trap")
	TObjectPtr<class USphereComponent> TriggerSphere = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Trap")
	TObjectPtr<UStaticMeshComponent> TrapMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Trap|Audio")
	TObjectPtr<class USoundBase> TrapPlacedSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Trap|Audio")
	TObjectPtr<class USoundBase> TrapTriggeredSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Trap|Settings")
	float TriggerRadius = 80.f;

	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnTrapPlaced();

	void SnarePlayer(class AMyCharacter* TargetCharacter);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnTrapTriggered(class AMyCharacter* TargetCharacter);

	void ReleasePlayer();

public:
	void InitTrap(float InSnareTime);
};
