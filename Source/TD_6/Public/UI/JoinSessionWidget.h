#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoinSessionWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinSessionConfirmedSignature, const FString&, Username);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJoinSessionCancelledSignature);

UCLASS()
class TD_6_API UJoinSessionWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnJoinButtonClickedEvent();

	UFUNCTION()
	void OnBackButtonClickedEvent();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> UsernameInput = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> JoinButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BackButton = nullptr;

public:
	void ResetToDefaults() const;

	FOnJoinSessionConfirmedSignature OnJoinSessionConfirmed;

	FOnJoinSessionCancelledSignature OnJoinSessionCancelled;
};
