#include "UI/GlobalTimerWidget.h"
#include "Components/TextBlock.h"

void UGlobalTimerWidget::SetGlobalTimer(int GlobalTimer)
{
	FString CompiledGlobalTimer;

	CompiledGlobalTimer = FString::FromInt(GlobalTimer / 60) + ":";

	int Seconds = GlobalTimer % 60;

	if (Seconds < 10)
	{
		CompiledGlobalTimer += "0";
	}

	CompiledGlobalTimer += FString::FromInt(Seconds);

	GlobalTimerText->SetText(FText::FromString(CompiledGlobalTimer));
}
