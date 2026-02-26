#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialProgressBarImage.generated.h"

class UImage;
class UMaterialInstanceDynamic;

UCLASS()
class TD_6_API URadialProgressBarImage : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UImage> RadialImage{ nullptr };
	
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UImage> IconImage{ nullptr };
	
	UPROPERTY(EditAnywhere, Category = "Design")
	FColor RadialColor{ FColor::White };
	
	UPROPERTY(EditAnywhere, Category = "Design")
	TObjectPtr<UTexture2D> IconTexture{ nullptr };
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Design")
	TObjectPtr<UMaterialInstanceDynamic> RadialMaterialInstance{ nullptr };

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "Radial Bar")
	void UpdateValue(const float CurrentValue);

};
