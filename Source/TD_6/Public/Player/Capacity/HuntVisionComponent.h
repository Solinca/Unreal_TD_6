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

	TWeakObjectPtr<class UCameraComponent> CachedCamera = nullptr;

	TArray<TWeakObjectPtr<class ACharacter>> HighlightedCharacters;

	void UpdateHuntVision();

	void SetCharacterHighlight(class ACharacter* Character, bool bHighlight);

	void ClearAllHighlights();

protected:
	UHuntVisionComponent();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float ScanInterval = 0.05f;

public:
	virtual void ActivateAbility() override;

	virtual void DeactivateAbility() override;
};
