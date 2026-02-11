#include "UI/LobbyPlayerItemWidget.h"
#include "Components/TextBlock.h"

void ULobbyPlayerItemWidget::SetPlayerName(FString PlayerName)
{
	PlayerNameText->SetText(FText::FromString(PlayerName));
}
