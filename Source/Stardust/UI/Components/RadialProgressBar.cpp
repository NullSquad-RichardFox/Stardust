// Fill out your copyright notice in the Description page of Project Settings.


#include "RadialProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"



void URadialProgressBar::SetAlpha(float Alpha)
{
	ProgressBar->GetDynamicMaterial()->SetScalarParameterValue(TEXT("Alpha"), Alpha);
}

void URadialProgressBar::SetCount(int32 Count)
{
	CountText->SetText(FText::AsNumber(Count));
}