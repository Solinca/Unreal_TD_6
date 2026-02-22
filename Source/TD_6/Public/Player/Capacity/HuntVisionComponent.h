#pragma once

#include "CoreMinimal.h"
#include "Player/Capacity/BaseAbilityComponent.h"
#include "HuntVisionComponent.generated.h"

class AMyCharacter;
class APlayerController;
class UCameraComponent;
class ACharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TD_6_API UHuntVisionComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Hunt Vision")
	float ScanInterval = 0.05f;

	FTimerHandle ScanTimerHandle;

	TWeakObjectPtr<AMyCharacter> CachedMyCharacter = nullptr;
	TWeakObjectPtr<APlayerController> CachedPlayerController = nullptr;
	TWeakObjectPtr<UCameraComponent> CachedCamera = nullptr;

	TArray<TWeakObjectPtr<ACharacter>> HighlightedCharacters;
	
public:
	UHuntVisionComponent();

	virtual void BeginPlay() override;

	virtual void ActivateAbility() override;
	virtual void DeactivateAbility() override;

private:
	void UpdateHuntVision();
	void SetCharacterHighlight(ACharacter* Character, bool bHighlight);
	void ClearAllHighlights();
};
