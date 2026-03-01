#include "UI/GlobalTimerWidget.h"
#include "Components/TextBlock.h"

void UGlobalTimerWidget::SetGlobalTimer(int GlobalTimer)
{
	GlobalTimerText->SetText(FText::Format(FText::FromString("{0}:{1}"), GlobalTimer / 60, GlobalTimer % 60));
}
