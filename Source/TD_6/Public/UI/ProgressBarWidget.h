#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProgressBarWidget.generated.h"

UCLASS()
class TD_6_API UProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ProgressBarText = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FColor CompletedColor;

public:
	void SetProgress(float Value);

	void SetProgressBarTextVisibility(bool IsVisible);
};
