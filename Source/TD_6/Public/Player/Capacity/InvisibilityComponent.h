#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "InvisibilityComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API UInvisibilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

private:
	TWeakObjectPtr<class AMyCharacter> MyChara = nullptr;

	FTimerHandle DissolveTimerHandle;

	float CurrentDissolveTime = 0.0f;

	float DissolveRate = 0.016f;

	bool bIsFadingOut = false;

	void UpdateDissolve();

protected:
	UInvisibilityComponent();

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
