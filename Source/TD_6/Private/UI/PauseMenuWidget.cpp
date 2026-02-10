#include "UI/PauseMenuWidget.h"
#include "Components/Button.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ContinueButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnContinueButtonClickedEvent);

	QuitButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitButtonClickedEvent);
}

void UPauseMenuWidget::OnContinueButtonClickedEvent()
{
	OnContinueButtonClicked.Broadcast();
}

void UPauseMenuWidget::OnQuitButtonClickedEvent()
{
	OnQuitButtonClicked.Broadcast();
}