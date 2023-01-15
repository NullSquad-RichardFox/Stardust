// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildSlotWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Stardust/UI/Planet/PlanetWidget.h"



void UBuildSlotWidget::NativeOnInitialized()
{
	HitBox->OnClicked.AddDynamic(this, &UBuildSlotWidget::ClickEvent);
}

void UBuildSlotWidget::Preload(AActor* ParentActor, int32 InBuildSlotIndex)
{
	OwningActor = ParentActor;
	BuildSlotIndex = InBuildSlotIndex;

	NotificationCount->SetVisibility(ESlateVisibility::Collapsed);
	NotificationIcon->SetVisibility(ESlateVisibility::Collapsed);
}

void UBuildSlotWidget::ClickEvent()
{
	if (UPlanetWidget* PlanetWidget = Cast<UPlanetWidget>(GetParent()->GetOuter()->GetOuter()))
	{
		PlanetWidget->BuildSlotClicked(BuildSlotIndex);
	}
}
