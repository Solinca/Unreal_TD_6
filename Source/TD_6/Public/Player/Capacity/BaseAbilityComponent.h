#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseAbilityComponent.generated.h"

UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API UBaseAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UBaseAbilityComponent();

	TWeakObjectPtr<class UMonsterDataAsset> MonsterDataAsset;

	FTimerHandle AbilityTimer;

	virtual void ActivateAbility() PURE_VIRTUAL(UBaseAbilityComponent::ActivateAbility, );

	virtual void DeactivateAbility() PURE_VIRTUAL(UBaseAbilityComponent::DeactivateAbility, );

public:
	UFUNCTION(Server, Reliable)
	void StartAbility(class UMonsterDataAsset* InData);

	UFUNCTION(Server, Reliable)
	void StopAbility();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayScreamWithSound(USoundBase* Sound);
};