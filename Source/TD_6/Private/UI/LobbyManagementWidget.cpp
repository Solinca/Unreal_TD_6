#include "UI/LobbyManagementWidget.h"
#include "UI/LobbyPlayerItemWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"

void ULobbyManagementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnStartButtonClickedEvent);

	GoToMonsterButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnGoToMonsterButtonClickedEvent);

	GoToPlayerButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnGoToPlayerButtonClickedEvent);

	BackButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnBackButtonClickedEvent);
}

void ULobbyManagementWidget::OnStartButtonClickedEvent()
{
	OnStartButtonClicked.Broadcast();
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

	LobbyPlayerCountText->SetText(FText::Format(FText::FromString("{0} / {1}"), PlayerCount, MaxPlayerConnectionCount - MaxMonsterCount));

	LobbyMonsterCountText->SetText(FText::Format(FText::FromString("{0} / {1}"), MonsterCount, MaxMonsterCount));
	
	if (!IsHost)
	{
		StartButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	StartButton->SetIsEnabled((MonsterCount <= MaxMonsterCount && PlayerCount <= MaxPlayerConnectionCount - MaxMonsterCount) /*&& MonsterCount > 0*/);
}
