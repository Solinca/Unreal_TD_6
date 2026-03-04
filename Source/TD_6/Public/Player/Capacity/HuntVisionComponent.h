#pragma once

#include "CoreMinimal.h"
#include "Player/Capacity/BaseAbilityComponent.h"
#include "HuntVisionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TD_6_API UHuntVisionComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

private:
	FTimerHandle ScanTimerHandle;

	UPROPERTY(Transient)
	TWeakObjectPtr<class AMyCharacter> MyChara = nullptr;

	UPROPERTY(Transient)
	TArray<class ACharacter*> HighlightedCharacters;

	void UpdateHuntVision();

	void ClearAllHighlights();

protected:
	UHuntVisionComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float ScanInterval = 0.05f;

public:
	virtual void ActivateAbility() override;

	virtual void DeactivateAbility() override;
};
