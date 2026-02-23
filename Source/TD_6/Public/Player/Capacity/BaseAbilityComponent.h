// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseAbilityComponent.generated.h"

class UMonsterDataAsset;
class AMyCharacter;
class AMyPlayerController;

UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API UBaseAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	TWeakObjectPtr<AMyCharacter> CachedMyCharacter = nullptr;
	TWeakObjectPtr<AMyPlayerController> CachedMyPlayerController = nullptr;

	FTimerHandle AbilityTimer;

	TWeakObjectPtr<UMonsterDataAsset> MonsterDataAsset;

	virtual void OnRep_IsActive() override;
	
public:	
	UBaseAbilityComponent();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartAbility(UMonsterDataAsset* InData);
	virtual void StopAbility();

protected:
	virtual void ActivateAbility() PURE_VIRTUAL(UBaseAbilityComponent::ActivateAbility, );
	virtual void DeactivateAbility() PURE_VIRTUAL(UBaseAbilityComponent::DeactivateAbility, );
};
