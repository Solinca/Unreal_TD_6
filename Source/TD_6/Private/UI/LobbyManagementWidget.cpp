#include "UI/LobbyManagementWidget.h"
#include "Animation/WidgetAnimation.h"
#include "UI/LobbyPlayerItemWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"
#include "Global/MyLobbyGameState.h"
#include "Player/MyLobbyPlayerController.h"

void ULobbyManagementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnStartButtonClickedEvent);

	GoToMonsterButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnGoToMonsterButtonClickedEvent);

	GoToPlayerButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnGoToPlayerButtonClickedEvent);

	BackButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnBackButtonClickedEvent);

	SetMonsterButcherButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnSetMonsterButcherButtonClicked);

	SetMonsterHunterButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnSetMonsterHunterButtonClicked);

	SetMonsterGhostButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnSetMonsterGhostButtonClicked);

	SetMonsterSlimeButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnSetMonsterSlimeButtonClicked);

	SetMonsterPredatorButton->OnClicked.AddDynamic(this, &ULobbyManagementWidget::OnSetMonsterPredatorButtonClicked);
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

void ULobbyManagementWidget::OnSetMonsterButcherButtonClicked()
{
	OnSetMonsterButtonClicked.Broadcast(EMonsterType::BUTCHER);
}

void ULobbyManagementWidget::OnSetMonsterHunterButtonClicked()
{
	OnSetMonsterButtonClicked.Broadcast(EMonsterType::HUNTER);
}

void ULobbyManagementWidget::OnSetMonsterGhostButtonClicked()
{
	OnSetMonsterButtonClicked.Broadcast(EMonsterType::GHOST);
}

void ULobbyManagementWidget::OnSetMonsterSlimeButtonClicked()
{
	OnSetMonsterButtonClicked.Broadcast(EMonsterType::SLIME);
}

void ULobbyManagementWidget::OnSetMonsterPredatorButtonClicked()
{
	OnSetMonsterButtonClicked.Broadcast(EMonsterType::PREDATOR);
}

void ULobbyManagementWidget::StartFadeAnimation()
{
	FWidgetAnimationDynamicEvent AnimDelegate;

	AnimDelegate.BindDynamic(this, &ULobbyManagementWidget::OnFadeOutFinished);

	BindToAnimationFinished(FadeOutAnimation, AnimDelegate);

	PlayAnimationForward(FadeOutAnimation);
}

void ULobbyManagementWidget::OnFadeOutFinished()
{
	UnbindAllFromAnimationFinished(FadeOutAnimation);

	OnLobbyManagementFadeFinished.Broadcast();
}

void ULobbyManagementWidget::UpdateLobby(TArray<FCustomPlayerData> PlayerDataList, FString SessionName, int MaxPlayerConnectionCount, int MaxMonsterCount, bool IsHost, FUniqueNetIdRepl CurrentPlayerID)
{
	int PlayerCount = 0;

	int MonsterCount = 0;

	PlayerListContainer->ClearChildren();

	MonsterListContainer->ClearChildren();

	SetMonsterButtonContainer->SetVisibility(ESlateVisibility::Hidden);

	SetMonsterButcherButton->SetBackgroundColor(DefaultButtonColor);

	SetMonsterHunterButton->SetBackgroundColor(DefaultButtonColor);

	SetMonsterGhostButton->SetBackgroundColor(DefaultButtonColor);

	SetMonsterSlimeButton->SetBackgroundColor(DefaultButtonColor);

	SetMonsterPredatorButton->SetBackgroundColor(DefaultButtonColor);

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

			if (PlayerData.CustomPlayerID == CurrentPlayerID)
			{
				SetMonsterButtonContainer->SetVisibility(ESlateVisibility::Visible);

				switch (PlayerData.MonsterType)
				{
					case EMonsterType::BUTCHER:
						SetMonsterButcherButton->SetBackgroundColor(ButcherColor);

						break;

					case EMonsterType::HUNTER:
						SetMonsterHunterButton->SetBackgroundColor(HunterColor);

						break;

					case EMonsterType::SLIME:
						SetMonsterSlimeButton->SetBackgroundColor(SlimeColor);

						break;

					case EMonsterType::GHOST:
						SetMonsterGhostButton->SetBackgroundColor(GhostColor);

						break;

					case EMonsterType::PREDATOR:
						SetMonsterPredatorButton->SetBackgroundColor(PredatorColor);

						break;

					default:
						break;
				}
			}

			switch (PlayerData.MonsterType)
			{
				case EMonsterType::BUTCHER:
					PlayerItem->SetPlayerColor(ButcherColor);

					break;

				case EMonsterType::HUNTER:
					PlayerItem->SetPlayerColor(HunterColor);

					break;

				case EMonsterType::SLIME:
					PlayerItem->SetPlayerColor(SlimeColor);

					break;

				case EMonsterType::GHOST:
					PlayerItem->SetPlayerColor(GhostColor);

					break;

				case EMonsterType::PREDATOR:
					PlayerItem->SetPlayerColor(PredatorColor);

					break;

				default:
					break;
			}

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
	
	StartButton->SetIsEnabled((MonsterCount <= MaxMonsterCount && PlayerCount <= MaxPlayerConnectionCount - MaxMonsterCount) && MonsterCount > 0 && PlayerCount > 0);
}
