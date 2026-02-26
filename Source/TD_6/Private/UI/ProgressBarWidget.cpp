#include "UI/ProgressBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UProgressBarWidget::SetProgress(float Value)
{
	ProgressBar->SetPercent(Value);

	if (Value >= 1)
	{
		ProgressBar->SetFillColorAndOpacity(CompletedColor);
	}
}

void UProgressBarWidget::SetProgressBarTextVisibility(bool IsVisible)
{
	ProgressBarText->SetVisibility(IsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
