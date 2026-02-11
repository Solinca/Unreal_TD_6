#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbySelectionWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbySelectionButtonClickedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbySelectedSignature, int, LobbyID);

UCLASS()
class TD_6_API ULobbySelectionWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnCreateButtonClickedEvent();

	UFUNCTION()
	void OnJoinButtonClickedEvent();

	UFUNCTION()
	void OnRefreshButtonClickedEvent();

	TObjectPtr<class ULobbyItemWidget> CurrentlySelectedLobby = nullptr;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> LobbyContainer = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CreateButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> JoinButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RefreshButton = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyItemWidgetClass = nullptr;

public:
	void SetLobbyList();

	void OnLobbyButtonClicked(class ULobbyItemWidget* SelectedLobbyItem);

	FOnLobbySelectionButtonClickedSignature OnCreateButtonClicked;

	FOnLobbySelectionButtonClickedSignature OnJoinButtonClicked;

	FOnLobbySelectionButtonClickedSignature OnRefreshButtonClicked;

	FOnLobbySelectedSignature OnLobbySelected;
};
