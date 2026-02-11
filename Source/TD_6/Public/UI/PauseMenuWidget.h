#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseMenuButtonClickedSignature);

UCLASS()
class TD_6_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void OnContinueButtonClickedEvent();

	UFUNCTION()
	void OnQuitButtonClickedEvent();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ContinueButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> QuitButton = nullptr;

public:
	FOnPauseMenuButtonClickedSignature OnContinueButtonClicked;

	FOnPauseMenuButtonClickedSignature OnQuitButtonClicked;
};
