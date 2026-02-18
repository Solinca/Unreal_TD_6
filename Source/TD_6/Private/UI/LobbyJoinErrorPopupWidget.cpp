#include "UI/LobbyJoinErrorPopupWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void ULobbyJoinErrorPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OkButton->OnClicked.AddDynamic(this, &ULobbyJoinErrorPopupWidget::OnOkButtonClickedEvent);
}

void ULobbyJoinErrorPopupWidget::OnOkButtonClickedEvent()
{
	OnOkButtonClicked.Broadcast();
}

void ULobbyJoinErrorPopupWidget::SetErrorMessageText(FString ErrorMessage)
{
	ErrorMessageText->SetText(FText::FromString(ErrorMessage));
}
