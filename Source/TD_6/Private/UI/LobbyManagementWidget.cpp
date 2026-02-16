#include "UI/LobbyManagementWidget.h"

#include "Animation/WidgetAnimation.h"
#include "UI/LobbyPlayerItemWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"
#include "Global/MyGameState.h"
#include "Player/MyLobbyPlayerController.h"

void ULobbyManagementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnStartButtonClickedEvent);

	GoToMonsterButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnGoToMonsterButtonClickedEvent);

	GoToPlayerButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnGoToPlayerButtonClickedEvent);

	BackButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnBackButtonClickedEvent);

	if (AMyGameState* GS = GetWorld()->GetGameState<AMyGameState>())
	{
		GS->OnGameStartSequence.AddDynamic(this, &ULobbyManagementWidget::OnGameStartFromNetwork);
	}
}

void ULobbyManagementWidget::OnGameStartFromNetwork()
{
	FWidgetAnimationDynamicEvent AnimDelegate;
	AnimDelegate.BindDynamic(this, &ULobbyManagementWidget::OnFadeOutFinished);
	
	BindToAnimationFinished(FadeOutAnimation, AnimDelegate);

	PlayAnimationForward(FadeOutAnimation);
}

void ULobbyManagementWidget::OnStartButtonClickedEvent()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMyLobbyPlayerController* LobbyPC = Cast<AMyLobbyPlayerController>(PC))
		{
			LobbyPC->ServerRequestStartGame();
		}
	}
}

void ULobbyManagementWidget::OnGoToMonsterButtonClickedEvent()
{
	OnGoToMonsterButtonClicked.Broadcast();
}

void ULobbyManagementWidget::OnGoToPlayerButtonClickedEvent()
{
	OnGoToPlayerButtonClicked.Broadcast();
}

void ULobbyManagementWidget::OnBackButtonClickedEvent()
{
	OnBackButtonClicked.Broadcast();
}

void ULobbyManagementWidget::OnFadeOutFinished()
{
	UnbindAllFromAnimationFinished(FadeOutAnimation);

	OnStartButtonClicked.Broadcast();
}

void ULobbyManagementWidget::UpdateLobby(TArray<FCustomPlayerData> PlayerDataList, FString SessionName, int MaxPlayerConnectionCount, int MaxMonsterCount, bool IsHost)
{
	int PlayerCount = 0;

	int MonsterCount = 0;

	PlayerListContainer->ClearChildren();

	MonsterListContainer->ClearChildren();

	for (FCustomPlayerData PlayerData : PlayerDataList)
	{
		ULobbyPlayerItemWidget* PlayerItem = CreateWidget<ULobbyPlayerItemWidget>(this, LobbyPlayerItemWidgetClass);

		PlayerItem->SetPlayerName(PlayerData.CustomPlayerName);

		PlayerListContainer->AddChild(PlayerItem);

		if (PlayerData.CurrentTeam == ETeam::PLAYER)
		{
			PlayerListContainer->AddChild(PlayerItem);

			PlayerCount++;
		}
		else if (PlayerData.CurrentTeam == ETeam::MONSTER)
		{
			MonsterListContainer->AddChild(PlayerItem);

			MonsterCount++;
		}
	}

	LobbyNameText->SetText(FText::FromString(SessionName));

	MaxPlayers = MaxPlayerConnectionCount - MaxMonsterCount;
	MaxMonsters = MaxMonsterCount;

	LobbyPlayerCountText->SetText(FText::Format(FText::FromString("{0} / {1}"), PlayerCount, MaxPlayers));

	LobbyMonsterCountText->SetText(FText::Format(FText::FromString("{0} / {1}"), MonsterCount, MaxMonsters));
	
	if (!IsHost)
	{
		StartButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	StartButton->SetIsEnabled((MonsterCount <= MaxMonsterCount && PlayerCount <= MaxPlayerConnectionCount - MaxMonsterCount) && MonsterCount > 0 && PlayerCount > 0);
}