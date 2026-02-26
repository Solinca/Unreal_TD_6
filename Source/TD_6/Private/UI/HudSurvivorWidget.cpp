#include "UI/HudSurvivorWidget.h"
#include "Components/Image.h"

void UHudSurvivorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetFlashlightEnabled(false);
}

void UHudSurvivorWidget::ToggleFlashlight()
{
	SetFlashlightEnabled(!bIsFlashlightOn);
}

void UHudSurvivorWidget::SetFlashlightEnabled(bool bEnabled)
{
	bIsFlashlightOn = bEnabled;
	UpdateVisual();
}

void UHudSurvivorWidget::UpdateVisual()
{
	if (FlashlightIcon)
	{
		FlashlightIcon->SetColorAndOpacity(bIsFlashlightOn ? EnabledColor : DisabledColor);
	}
}