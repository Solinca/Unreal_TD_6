#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyManagementWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyManagementButtonClickedSignature);

UCLASS()
class TD_6_API ULobbyManagementWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnStartButtonClickedEvent();

	UFUNCTION()
	void OnGoToMonsterButtonClickedEvent();

	UFUNCTION()
	void OnGoToPlayerButtonClickedEvent();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> PlayerListContainer = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> MonsterListContainer = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> StartButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> GoToMonsterButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> GoToPlayerButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LobbyNameText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LobbyPlayerCountText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LobbyMonsterCountText = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyPlayerItemWidgetClass = nullptr;

public:
	void SetupLobby();

	FOnLobbyManagementButtonClickedSignature OnStartButtonClicked;

	FOnLobbyManagementButtonClickedSignature OnGoToMonsterButtonClicked;

	FOnLobbyManagementButtonClickedSignature OnGoToPlayerButtonClicked;
};
