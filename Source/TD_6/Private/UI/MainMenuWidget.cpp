#include "UI/MainMenuWidget.h"
#include "Components/Button.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPlayButtonClickedEvent);

	QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonClickedEvent);
}

void UMainMenuWidget::OnPlayButtonClickedEvent()
{
	OnPlayButtonClicked.Broadcast();
}

void UMainMenuWidget::OnQuitButtonClickedEvent()
{
	OnQuitButtonClicked.Broadcast();
}
