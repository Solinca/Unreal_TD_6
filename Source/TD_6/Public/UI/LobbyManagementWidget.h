#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Global/MyGameInstance.h"
#include "LobbyManagementWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyManagementButtonClickedSignature);

UCLASS()
class TD_6_API ULobbyManagementWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnGameStartFromNetwork();
	
	UFUNCTION()
	void OnStartButtonClickedEvent();

	UFUNCTION()
	void OnGoToMonsterButtonClickedEvent();

	UFUNCTION()
	void OnGoToPlayerButtonClickedEvent();
	
	UFUNCTION()
	void OnBackButtonClickedEvent();
	
	UFUNCTION()
	void OnFadeOutFinished();

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
	TObjectPtr<class UButton> BackButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LobbyNameText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LobbyPlayerCountText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LobbyMonsterCountText = nullptr;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnimation = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyPlayerItemWidgetClass = nullptr;

	int MaxPlayers{ 0 };
	int MaxMonsters{ 0 };
public:
	void UpdateLobby(TArray<FCustomPlayerData> PlayerDataList, FString SessionName, int MaxPlayerConnectionCount, int MaxMonsterCount, bool IsHost);

	FOnLobbyManagementButtonClickedSignature OnStartButtonClicked;

	FOnLobbyManagementButtonClickedSignature OnGoToMonsterButtonClicked;

	FOnLobbyManagementButtonClickedSignature OnGoToPlayerButtonClicked;
	
	FOnLobbyManagementButtonClickedSignature OnBackButtonClicked;
};
