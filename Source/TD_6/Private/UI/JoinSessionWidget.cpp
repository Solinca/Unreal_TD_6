#include "UI/JoinSessionWidget.h"
#include "Components/EditableText.h"
#include "Components/Button.h"

void UJoinSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	JoinButton->OnClicked.AddDynamic(this, &UJoinSessionWidget::OnJoinButtonClickedEvent);

	BackButton->OnClicked.AddDynamic(this, &UJoinSessionWidget::OnBackButtonClickedEvent);

	ResetToDefaults();
}

void UJoinSessionWidget::OnJoinButtonClickedEvent()
{
	const FString Username = UsernameInput->GetText().ToString().TrimStartAndEnd();

	if (Username.Len() < 1)
	{
		return;
	}

	OnJoinSessionConfirmed.Broadcast(Username);
}

void UJoinSessionWidget::OnBackButtonClickedEvent()
{
	OnJoinSessionCancelled.Broadcast();
}

void UJoinSessionWidget::ResetToDefaults() const
{
	UsernameInput->SetHintText(FText::FromString("Username"));
}
