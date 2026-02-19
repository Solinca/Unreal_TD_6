#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HuntVisionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TD_6_API UHuntVisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHuntVisionComponent();

	virtual void BeginPlay() override;

	void Activate();

	void Deactivate();

private:
	void UpdateHuntVision();

	void SetCharacterHighlight(ACharacter* Character, bool bHighlight);

	void ClearAllHighlights();

	UPROPERTY(EditAnywhere, Category = "Hunt Vision")
	float ScanInterval = 0.05f;

	UPROPERTY()
	TObjectPtr<class AMyCharacter> CachedOwnerCharacter = nullptr;

	UPROPERTY()
	TObjectPtr<APlayerController> CachedPC = nullptr;

	UPROPERTY()
	TObjectPtr<class UCameraComponent> CachedCamera = nullptr;

	FTimerHandle ScanTimerHandle;

	UPROPERTY()
	TArray<TWeakObjectPtr<ACharacter>> HighlightedCharacters;
};
