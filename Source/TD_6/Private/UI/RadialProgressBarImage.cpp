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

	IconImage->SetBrushFromTexture(IconTexture);

	UpdateValue(1.f);
}

void URadialProgressBarImage::UpdateValue(const float CurrentValue)
{
	RadialMaterialInstance->SetScalarParameterValue(FName("Percent"), CurrentValue);
}
