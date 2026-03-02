#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudSurvivorWidget.generated.h"

class UImage;

UCLASS()
class TD_6_API UHudSurvivorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerHUD|Flashlight")
	void ToggleFlashlight();

	UFUNCTION(BlueprintCallable, Category = "PlayerHUD|Flashlight")
	void SetFlashlightEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "PlayerHUD|Flashlight")
	bool IsFlashlightOn() const { return bIsFlashlightOn; }

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> FlashlightIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> FlashlightGlowEffect;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerHUD|Appearance")
	FLinearColor EnabledColor = FLinearColor(1.f, 0.9f, 0.5f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category = "PlayerHUD|Appearance")
	FLinearColor DisabledColor = FLinearColor(0.25f, 0.25f, 0.25f, 0.5f);

private:
	bool bIsFlashlightOn = false;
	
	void UpdateVisual();
};
