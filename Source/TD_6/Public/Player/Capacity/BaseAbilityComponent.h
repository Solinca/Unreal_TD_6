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

	virtual void BeginPlay() override;

	TWeakObjectPtr<class AMyCharacter> CachedMyCharacter = nullptr;

	TWeakObjectPtr<class AMyPlayerController> CachedMyPlayerController = nullptr;

	FTimerHandle AbilityTimer;

	TWeakObjectPtr<class UMonsterDataAsset> MonsterDataAsset;

	virtual void ActivateAbility() PURE_VIRTUAL(UBaseAbilityComponent::ActivateAbility, );

	virtual void DeactivateAbility() PURE_VIRTUAL(UBaseAbilityComponent::DeactivateAbility, );

public:
	UFUNCTION(NetMulticast, Reliable)
	void StartAbility(class UMonsterDataAsset* InData);

	UFUNCTION(NetMulticast, Reliable)
	void StopAbility();
};
