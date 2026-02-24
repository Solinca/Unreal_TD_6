#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "InvisibilityComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API UInvisibilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;

	FTimerHandle DissolveTimerHandle;

	float CurrentDissolveTime = 0.0f;

	float DissolveRate = 0.016f;

	bool bIsFadingOut = false;

	void InitDynamicMaterials();

	void UpdateDissolve();

protected:
	UInvisibilityComponent();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	FName DissolveParamName = "Dissolve";

	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	float DissolveDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	float DissolveMinValue = -1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	float DissolveMaxValue = 1.0f;

public:
	virtual void ActivateAbility() override;

	virtual void DeactivateAbility() override;
};
