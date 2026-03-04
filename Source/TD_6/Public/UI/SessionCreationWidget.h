#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionCreationWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnSessionCreationConfirmedSignature, const FString&, SessionName, const FString&, Username, int32, MaxPlayers, int32, MaxMonsters, bool, IsLan);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionCreationCancelledSignature);

UCLASS()
class TD_6_API USessionCreationWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnCreateButtonClickedEvent();

	UFUNCTION()
	void OnBackButtonClickedEvent();

	UFUNCTION()
	void OnMaxMonstersValueChanged(float Value);

	UFUNCTION()
	void OnMaxPlayersValueChanged(float Value);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> SessionNameInput = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> UsernameInput = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USpinBox> MaxPlayersSpinBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USpinBox> MaxMonstersSpinBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCheckBox> LanMatchCheckBox = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CreateButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BackButton = nullptr;

public:
	void ResetToDefaults() const;

	FOnSessionCreationConfirmedSignature OnSessionCreationConfirmed;

	FOnSessionCreationCancelledSignature OnSessionCreationCancelled;
};
