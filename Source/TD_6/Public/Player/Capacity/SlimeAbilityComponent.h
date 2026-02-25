// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "SlimeAbilityComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TD_6_API USlimeAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

private:
	UFUNCTION(Server, Reliable)
	void SetMovementSpeedServerSide(float MovementSpeed);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Slime|Transformation")
	float TransformationDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Slime|Transformation")
	float TransformationSlowSpeed = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Slime|Transformation")
	FVector FlattenedScale = FVector(1.5f, 1.5f, 0.3f);

	UPROPERTY(EditDefaultsOnly, Category = "Slime|Transformation")
	float ScaleLerpRate = 0.016f;

	FTimerHandle TransformTimerHandle;
	FTimerHandle ScaleLerpTimerHandle;

	FVector OriginalScale = FVector::OneVector;

	float DefaultMaxSpeed = 0.f;
	float SlimeSprintSpeed = 0.f;
	float CurrentLerpTime = 0.f;

	bool bIsTransforming = false;
	bool bIsFlattening = false;

public:
	USlimeAbilityComponent();

	virtual void BeginPlay() override;

	virtual void ActivateAbility() override;
	virtual void DeactivateAbility() override;

private:
	void OnTransformationComplete();
	void OnRevertTransformationComplete();
	void UpdateScaleLerp();
};