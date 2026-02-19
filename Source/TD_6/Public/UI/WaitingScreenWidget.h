#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaitingScreenWidget.generated.h"

UCLASS()
class TD_6_API UWaitingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CountdownText = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> WaitingScreenFadeOut = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> CountdownTextPopup = nullptr;

public:
	void SetCountdownText(FString Countdown);

	void PlayWaitingScreenFadeOutAnimation();

	void PlayCountdownTextPopupAnimation();
};
