#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialProgressBarImage.generated.h"

class UImage;
class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCooldownFinished);

UCLASS()
class TD_6_API URadialProgressBarImage : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> RadialImage{ nullptr };

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Design")
	FColor RadialColor{ FColor::White };

	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UTexture2D> IconTexture{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Design")
	float CooldownOpacity = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float ReadyOpacity = 1.f;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Design")
	TObjectPtr<UMaterialInstanceDynamic> RadialMaterialInstance{ nullptr };

	float CooldownDuration = 0.f;
	float CooldownElapsed = 0.f;
	bool bIsCoolingDown = false;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Radial Bar")
	void StartCooldown(float Duration);

	UFUNCTION(BlueprintCallable, Category = "Radial Bar")
	void CancelCooldown();

	UFUNCTION(BlueprintCallable, Category = "Radial Bar")
	void UpdateValue(float CurrentValue);

	UFUNCTION(BlueprintPure, Category = "Radial Bar")
	bool IsReady() const { return !bIsCoolingDown; }

	UPROPERTY(BlueprintAssignable, Category = "Radial Bar")
	FOnCooldownFinished OnCooldownFinished;

private:
	void SetReadyVisual(bool bReady);
};