#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapActor.generated.h"

class USphereComponent;
class UAudioComponent;
class USoundBase;
class AMyCharacter;

UCLASS()
class TD_6_API ATrapActor : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Trap")
	TObjectPtr<USphereComponent> TriggerSphere = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Trap")
	TObjectPtr<UStaticMeshComponent> TrapMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Trap|Audio")
	TObjectPtr<USoundBase> TrapTriggeredSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Trap|Settings")
	float TriggerRadius = 80.f;

	float SnareTime = 3.f;

	TWeakObjectPtr<AMyCharacter> SnaredCharacter = nullptr;

	FTimerHandle SnareTimerHandle;

	bool bIsTriggered = false;

	UFUNCTION()
	void OnTriggerOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void SnarePlayer(AMyCharacter* TargetCharacter);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnTrapTriggered(AMyCharacter* TargetCharacter);

	void ReleasePlayer();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnPlayerReleased();	

public:
	ATrapActor();

	void InitTrap(float InSnareTime);
};
