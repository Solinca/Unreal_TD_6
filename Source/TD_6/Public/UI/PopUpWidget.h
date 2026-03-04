// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopUpWidget.generated.h"

UCLASS()
class TD_6_API UPopUpWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextPopUp;

public:
	void SetupText(const FText& InText) const;
};
