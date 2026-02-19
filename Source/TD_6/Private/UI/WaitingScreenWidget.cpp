#include "UI/WaitingScreenWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UWaitingScreenWidget::SetCountdownText(FString Countdown)
{
	CountdownText->SetText(FText::FromString(Countdown));
}

void UWaitingScreenWidget::PlayWaitingScreenFadeOutAnimation()
{
	PlayAnimation(WaitingScreenFadeOut);
}

void UWaitingScreenWidget::PlayCountdownTextPopupAnimation()
{
	PlayAnimation(CountdownTextPopup);
}
