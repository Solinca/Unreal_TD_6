// RadialProgressBarImage.cpp

#include "UI/RadialProgressBarImage.h"
#include "Components/Image.h"

void URadialProgressBarImage::NativeConstruct()
{
	Super::NativeConstruct();

	if (!RadialMaterialInstance)
	{
		UMaterial* BaseMat = Cast<UMaterial>(RadialImage->GetBrush().GetResourceObject());
		RadialMaterialInstance = UMaterialInstanceDynamic::Create(BaseMat, this);

		if (RadialMaterialInstance)
		{
			RadialImage->SetBrushFromMaterial(RadialMaterialInstance);
			RadialMaterialInstance->SetVectorParameterValue("BarColor", RadialColor);
		}
	}

	if (IconTexture)
	{
		IconImage->SetBrushFromTexture(IconTexture);
	}

	UpdateValue(1.f);
	SetReadyVisual(true);
}

void URadialProgressBarImage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsCoolingDown)
	{
		return;
	}

	CooldownElapsed += InDeltaTime;
	const float Progress = FMath::Clamp(CooldownElapsed / CooldownDuration, 0.f, 1.f);
	UpdateValue(Progress);

	if (CooldownElapsed >= CooldownDuration)
	{
		bIsCoolingDown = false;
		UpdateValue(1.f);
		SetReadyVisual(true);
		OnCooldownFinished.Broadcast();
	}
}

void URadialProgressBarImage::StartCooldown(float Duration)
{
	if (Duration <= 0.f)
	{
		return;
	}

	CooldownDuration = Duration;
	CooldownElapsed = 0.f;
	bIsCoolingDown = true;

	UpdateValue(0.f);
	SetReadyVisual(false);
}

void URadialProgressBarImage::CancelCooldown()
{
	bIsCoolingDown = false;
	CooldownElapsed = 0.f;
	UpdateValue(1.f);
	SetReadyVisual(true);
}

void URadialProgressBarImage::UpdateValue(float CurrentValue)
{
	if (RadialMaterialInstance)
	{
		RadialMaterialInstance->SetScalarParameterValue(FName("Percent"), FMath::Clamp(CurrentValue, 0.f, 1.f));
	}
}

void URadialProgressBarImage::SetReadyVisual(bool bReady)
{
	const float Opacity = bReady ? ReadyOpacity : CooldownOpacity;
	SetRenderOpacity(Opacity);
}