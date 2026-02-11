#include "UI/LobbySelectionWidget.h"
#include "UI/LobbyItemWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"

void ULobbySelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateButton->OnClicked.AddDynamic(this, &ULobbySelectionWidget::OnCreateButtonClickedEvent);

	JoinButton->OnClicked.AddDynamic(this, &ULobbySelectionWidget::OnJoinButtonClickedEvent);

	RefreshButton->OnClicked.AddDynamic(this, &ULobbySelectionWidget::OnRefreshButtonClickedEvent);
}

void ULobbySelectionWidget::OnCreateButtonClickedEvent()
{
	OnCreateButtonClicked.Broadcast();
}

void ULobbySelectionWidget::OnJoinButtonClickedEvent()
{
	OnJoinButtonClicked.Broadcast();
}

void ULobbySelectionWidget::OnRefreshButtonClickedEvent()
{
	OnRefreshButtonClicked.Broadcast();

	LobbyContainer->ClearChildren();
}

void ULobbySelectionWidget::SetLobbyList()
{
	// Set a TArray parameter with a special struct containing:

	// Lobby Name
	// Lobby ID
	// Lobby CurrentPlayerCount
	// Lobby MaxPlayerCount

	ULobbyItemWidget* Test = CreateWidget<ULobbyItemWidget>(this, LobbyItemWidgetClass);

	Test->SetData(this, 0, "First Lobby", 3, 1);

	LobbyContainer->AddChildToVerticalBox(Test);

	ULobbyItemWidget* Test2 = CreateWidget<ULobbyItemWidget>(this, LobbyItemWidgetClass);

	Test2->SetData(this, 1, "Second Lobby", 3, 2);

	LobbyContainer->AddChildToVerticalBox(Test2);

	ULobbyItemWidget* Test3 = CreateWidget<ULobbyItemWidget>(this, LobbyItemWidgetClass);

	Test3->SetData(this, 2, "Third Lobby", 3, 3);

	LobbyContainer->AddChildToVerticalBox(Test3);
}

void ULobbySelectionWidget::OnLobbyButtonClicked(ULobbyItemWidget* SelectedLobbyItem)
{
	if (CurrentlySelectedLobby)
	{
		CurrentlySelectedLobby->SetHighlight(false);
	}

	CurrentlySelectedLobby = SelectedLobbyItem;

	CurrentlySelectedLobby->SetHighlight(true);

	OnLobbySelected.Broadcast(CurrentlySelectedLobby->GetLobbyID());

	JoinButton->SetIsEnabled(true);
}
