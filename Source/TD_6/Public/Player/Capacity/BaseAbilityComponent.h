// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseAbilityComponent.generated.h"


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API UBaseAbilityComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UBaseAbilityComponent();

	virtual void ActivateAbility() PURE_VIRTUAL(UBaseAbilityComponent::ActivateAbility, );
	virtual void DeactivateAbility() PURE_VIRTUAL(UBaseAbilityComponent::DeactivateAbility, );

};
