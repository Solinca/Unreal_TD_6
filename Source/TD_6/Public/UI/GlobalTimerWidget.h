#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlobalTimerWidget.generated.h"

UCLASS()
class TD_6_API UGlobalTimerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> GlobalTimerText = nullptr;

public:
	void SetGlobalTimer(int GlobalTimer);
};
