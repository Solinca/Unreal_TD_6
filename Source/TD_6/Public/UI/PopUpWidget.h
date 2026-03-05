#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopUpWidget.generated.h"

UCLASS()
class TD_6_API UPopUpWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PopUpMessageText = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> FadePopup = nullptr;

public:
	void SetupText(const FText& Message);
};
