#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuButtonClickedSignature);

UCLASS()
class TD_6_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnPlayButtonClickedEvent();

	UFUNCTION()
	void OnQuitButtonClickedEvent();

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> PlayButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> QuitButton = nullptr;

public:
	FOnMainMenuButtonClickedSignature OnPlayButtonClicked;

	FOnMainMenuButtonClickedSignature OnQuitButtonClicked;
};
