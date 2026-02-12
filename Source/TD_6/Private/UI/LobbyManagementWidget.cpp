#include "UI/LobbyManagementWidget.h"
#include "UI/LobbyPlayerItemWidget.h"
#include "Global/MyPlayerState.h"
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

void ULobbyManagementWidget::SetupLobby(const TArray<class AMyPlayerState*>& PlayerList, FString SessionName, int MaxPlayerConnectionCount, int MaxMonsterCount, bool IsHost)
{
	int PlayerCount = 0;

	int MonsterCount = 0;

	PlayerListContainer->ClearChildren();

	MonsterListContainer->ClearChildren();

	for (AMyPlayerState* PlayerState : PlayerList)
	{
		if (!PlayerState)
		{
			continue;
		}

		ULobbyPlayerItemWidget* PlayerItem = CreateWidget<ULobbyPlayerItemWidget>(this, LobbyPlayerItemWidgetClass);

		PlayerItem->SetPlayerName(PlayerState->GetPlayerName());

		PlayerListContainer->AddChild(PlayerItem);

		if (PlayerState->GetCurrentTeam() == ETeam::PLAYER)
		{
			PlayerListContainer->AddChild(PlayerItem);

			PlayerCount++;
		}
		else if (PlayerState->GetCurrentTeam() == ETeam::MONSTER)
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
		StartButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
