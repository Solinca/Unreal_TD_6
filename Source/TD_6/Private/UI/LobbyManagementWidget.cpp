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

void ULobbyManagementWidget::SetupLobby(bool IsHost)
{
	// Set a TArray parameter with a special struct containing:

	// Lobby Name
	// Lobby Max Player Count
	// Lobby Max Monster Count
	// Either a list for Player and Monster, either a NetworkPlayer List like this:
	// Network Player List = TArray of FPlayer struct
		// Player Name
		// Player Team (special enum ETeam::Player and ETeam::Monster)

	ULobbyPlayerItemWidget* Player1 = CreateWidget<ULobbyPlayerItemWidget>(this, LobbyPlayerItemWidgetClass);

	Player1->SetPlayerName("Player 1");

	PlayerListContainer->AddChild(Player1);

	ULobbyPlayerItemWidget* Player2 = CreateWidget<ULobbyPlayerItemWidget>(this, LobbyPlayerItemWidgetClass);

	Player2->SetPlayerName("Player 2");

	PlayerListContainer->AddChild(Player2);

	ULobbyPlayerItemWidget* Player3 = CreateWidget<ULobbyPlayerItemWidget>(this, LobbyPlayerItemWidgetClass);

	Player3->SetPlayerName("Player 3");

	PlayerListContainer->AddChild(Player3);

	ULobbyPlayerItemWidget* Monster = CreateWidget<ULobbyPlayerItemWidget>(this, LobbyPlayerItemWidgetClass);

	Monster->SetPlayerName("Big Bad Monster Player");

	MonsterListContainer->AddChild(Monster);

	LobbyNameText->SetText(FText::FromString("I'm a test Lobby Name !!!!"));

	LobbyPlayerCountText->SetText(FText::FromString("3 / 4"));

	LobbyMonsterCountText->SetText(FText::FromString("1 / 1"));

	if (!IsHost)
	{
		StartButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
