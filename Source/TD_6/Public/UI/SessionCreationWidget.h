#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionCreationWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnSessionCreationConfirmedSignature, const FString&, SessionName, const FString&, Username, int32, MaxPlayers, int32, MaxMonsters, bool, IsLan);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionCreationCancelledSignature);

class UEditableText;
class USpinBox;
class UCheckBox;
class UButton;

UCLASS()
class TD_6_API USessionCreationWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnCreateButtonClickedEvent();

	UFUNCTION()
	void OnBackButtonClickedEvent();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> SessionNameInput = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> UsernameInput = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USpinBox> MaxPlayersSpinBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USpinBox> MaxMonstersSpinBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> LanMatchCheckBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton = nullptr;

public:
	void ResetToDefaults() const;

	FOnSessionCreationConfirmedSignature OnSessionCreationConfirmed;

	FOnSessionCreationCancelledSignature OnSessionCreationCancelled;
};
