// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "InvisibilityComponent.generated.h"

class AMyCharacter;
class APlayerController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API UInvisibilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	FName DissolveParamName = "Dissolve";

	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	float DissolveDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	float DissolveMinValue = -1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	float DissolveMaxValue = 1.0f;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;

	FTimerHandle DissolveTimerHandle;
	
	float CurrentDissolveTime = 0.0f;
	float Rate = 0.016f;

	bool bIsFadingOut = false;
	
public:
	UInvisibilityComponent();

	virtual void BeginPlay() override;

	virtual void ActivateAbility() override;
	virtual void DeactivateAbility() override;

private:
	void InitDynamicMaterials();
	void UpdateDissolve();
};
