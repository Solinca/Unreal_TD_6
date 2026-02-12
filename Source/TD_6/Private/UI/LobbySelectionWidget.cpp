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

	RefreshButton->SetIsEnabled(false);
}

void ULobbySelectionWidget::SetLobbyList(const TArray<FCustomSessionInfo>& SessionResults)
{
	for (FCustomSessionInfo SessionInfo : SessionResults)
	{
		ULobbyItemWidget* LobbyItem = CreateWidget<ULobbyItemWidget>(this, LobbyItemWidgetClass);

		LobbyItem->SetData(this, SessionInfo.SessionSearchResultIndex, SessionInfo.SessionName, SessionInfo.CurrentPlayerCount, SessionInfo.MaxPlayerConnectionAmount);

		LobbyContainer->AddChildToVerticalBox(LobbyItem);
	}

	RefreshButton->SetIsEnabled(true);
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
