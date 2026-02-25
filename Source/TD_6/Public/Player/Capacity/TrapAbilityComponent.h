#pragma once

#include "CoreMinimal.h"
#include "Player/Capacity/BaseAbilityComponent.h"
#include "TrapAbilityComponent.generated.h"

class ATrapActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API UTrapAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Trap")
	TSubclassOf<ATrapActor> TrapActorClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Trap")
	float SpawnDistanceFromOwner = 200.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Trap")
	float GroundTraceHeight = 500.f;

	TArray<TWeakObjectPtr<ATrapActor>> SpawnedTraps;

public:
	UTrapAbilityComponent();

	virtual void ActivateAbility() override;
	virtual void DeactivateAbility() override {};

private:
	void RemoveOldestTrapIfNeeded();
};
