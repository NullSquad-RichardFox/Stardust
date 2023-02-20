// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSlotWidget.h"
#include "DistrictMenu.h"
#include "Components/PanelWidget.h"


void UBuildingSlotWidget::OnClicked()
{
	if (UDistrictMenu* DistrictMenu = Cast<UDistrictMenu>(GetParent()->GetOuter()->GetOuter()))
	{
		DistrictMenu->BuildingSlotClicked(SlotIndex);
	}
}