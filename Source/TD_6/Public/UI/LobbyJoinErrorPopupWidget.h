#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyJoinErrorPopupWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOkButtonClickedSignature);

UCLASS()
class TD_6_API ULobbyJoinErrorPopupWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnOkButtonClickedEvent();
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> OkButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ErrorMessageText = nullptr;

public:
	FOnOkButtonClickedSignature OnOkButtonClicked;

	void SetErrorMessageText(FString ErrorMessage);
};
