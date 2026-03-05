#include "UI/PopUpWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UPopUpWidget::SetupText(const FText& Message)
{
	PopUpMessageText->SetText(Message);

	PlayAnimation(FadePopup);
}
