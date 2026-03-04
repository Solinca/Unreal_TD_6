// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PopUpWidget.h"
#include "Components/TextBlock.h"

void UPopUpWidget::SetupText(const FText& InText) const
{
	TextPopUp->SetText(InText);
}
