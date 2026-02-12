#include "UI/LobbyItemWidget.h"
#include "UI/LobbySelectionWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void ULobbyItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SelectLobbyButton->OnClicked.AddDynamic(this, &ULobbyItemWidget::OnSelectLobbyClickedEvent);
}

void ULobbyItemWidget::OnSelectLobbyClickedEvent()
{
	LobbySelectionWidget->OnLobbyButtonClicked(this);
}

void ULobbyItemWidget::SetData(class ULobbySelectionWidget* Parent, int LobbyID, FString LobbyName, int CurrentPlayerCount, int MaxPlayerCount)
{
	LobbySelectionWidget = Parent;

	CurrentLobbyID = LobbyID;

	LobbyNameText->SetText(FText::FromString(LobbyName));

	LobbyPlayerCountText->SetText(FText::Format(FText::FromString("{0} / {1}"), CurrentPlayerCount, MaxPlayerCount));
}

void ULobbyItemWidget::SetHighlight(bool IsSelected)
{
	Background->SetBrushColor(IsSelected ? SelectedColor : NormalColor);
}
