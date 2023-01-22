// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingListItem.h"
#include "Components/Button.h"
#include "Stardust/GameObjects/Planet.h"
#include "Stardust/UI/Planet/DistrictMenu.h"


void UBuildingListItem::NativeOnInitialized()
{
	HitBox->OnClicked.AddDynamic(this, &UBuildingListItem::OnClicked);
}

void UBuildingListItem::OnItemAddedHandle(UObject* Object)
{

}

void UBuildingListItem::OnClicked()
{
	if (APlanet* OwningPlanet = Cast<APlanet>(OwningActor))
	{
		OwningPlanet->BuildBuilding(BuildSlotIndex, BuildingType);

		if (UDistrictMenu* DistrictMenu = Cast<UDistrictMenu>(GetOuter()->GetOuter()))
		{
			DistrictMenu->SetCanBuildBuildings(false);
		}
	}
}