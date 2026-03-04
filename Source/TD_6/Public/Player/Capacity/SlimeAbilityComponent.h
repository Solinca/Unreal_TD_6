#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "SlimeAbilityComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API USlimeAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

private:
	TWeakObjectPtr<class AMyCharacter> MyChara = nullptr;

	FTimerHandle TransformTimerHandle;

	FTimerHandle ScaleLerpTimerHandle;

	FVector OriginalScale = FVector::OneVector;

	float SlimeSprintSpeed = 0.f;

	float CurrentLerpTime = 0.f;

	bool bIsTransforming = false;

	bool bIsFlattening = false;

	void OnTransformationComplete();

	void OnRevertTransformationComplete();

	void UpdateScaleLerp();

protected:
	USlimeAbilityComponent();

	UPROPERTY(EditDefaultsOnly, Category = "Slime|Transformation")
	float TransformationDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Slime|Transformation")
	float TransformationSlowSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Slime|Transformation")
	FVector FlattenedScale = FVector(1.5f, 1.5f, 0.3f);

	UPROPERTY(EditDefaultsOnly, Category = "Slime|Transformation")
	float ScaleLerpRate = 0.016f;

public:
	virtual void ActivateAbility() override;

	virtual void DeactivateAbility() override;
};