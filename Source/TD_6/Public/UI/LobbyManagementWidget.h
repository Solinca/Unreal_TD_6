#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Global/MyGameInstance.h"
#include "LobbyManagementWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyManagementButtonClickedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyManagementFadeFinishedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetMonsterButtonClickedSignature, EMonsterType, MonsterType);

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
	
	UFUNCTION()
	void OnBackButtonClickedEvent();
	
	UFUNCTION()
	void OnFadeOutFinished();

	UFUNCTION()
	void OnSetMonsterButcherButtonClicked();

	UFUNCTION()
	void OnSetMonsterHunterButtonClicked();

	UFUNCTION()
	void OnSetMonsterGhostButtonClicked();

	UFUNCTION()
	void OnSetMonsterSlimeButtonClicked();

	UFUNCTION()
	void OnSetMonsterPredatorButtonClicked();

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

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> SetMonsterButtonContainer = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SetMonsterButcherButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SetMonsterHunterButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SetMonsterGhostButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SetMonsterSlimeButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SetMonsterPredatorButton = nullptr;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnimation = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyPlayerItemWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FColor DefaultButtonColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FColor ButcherColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FColor HunterColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FColor GhostColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FColor SlimeColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FColor PredatorColor;

public:
	void StartFadeAnimation();

	void UpdateLobby(TArray<FCustomPlayerData> PlayerDataList, FString SessionName, int MaxPlayerConnectionCount, int MaxMonsterCount, bool IsHost, FUniqueNetIdRepl CurrentPlayerID);

	FOnLobbyManagementButtonClickedSignature OnStartButtonClicked;

	FOnLobbyManagementButtonClickedSignature OnGoToMonsterButtonClicked;

	FOnLobbyManagementButtonClickedSignature OnGoToPlayerButtonClicked;
	
	FOnLobbyManagementButtonClickedSignature OnBackButtonClicked;

	FOnLobbyManagementFadeFinishedSignature OnLobbyManagementFadeFinished;

	FOnSetMonsterButtonClickedSignature OnSetMonsterButtonClicked;
};
