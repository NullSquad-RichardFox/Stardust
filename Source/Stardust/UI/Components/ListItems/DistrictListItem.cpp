// Fill out your copyright notice in the Description page of Project Settings.


#include "DistrictListItem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Stardust/Libraries/ListDataLibrary.h"
#include "Stardust/GameObjects/Planet.h"
#include "Stardust/UI/Planet/DistrictMenu.h"



void UDistrictListItem::NativeOnInitialized()
{
	HitBox->OnClicked.AddDynamic(this, &UDistrictListItem::ItemClickedHandle);
	ColorShade->SetVisibility(ESlateVisibility::Collapsed);
}

void UDistrictListItem::OnItemAddedHandle(UObject* Object)
{
	UDistrictListData* Data = Cast<UDistrictListData>(Object);
	if (!Data)
		return;

	Data->GetDistrictData(BuildSlotIndex, DistrictType, OwningActor);
	APlanet* OwningPlanet = Cast<APlanet>(OwningActor);
	if (!OwningPlanet)
		return;


	bool bIsBuildable = false;
	for (EFeatureType FeatureType : OwningPlanet->GetBuildSlot(BuildSlotIndex).Features)
	{
		if (UStructDataLibrary::GetData(FeatureType)->BuildableDistricts.Contains(DistrictType))
		{
			bIsBuildable = true;
			break;
		}
	}

	if (!bIsBuildable)
	{
		SetIsEnabled(false);
		ColorShade->SetVisibility(ESlateVisibility::Visible);
	}

	if (UEnum* DistrictTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDistrictType"), true))
	{
		DistrictName->SetText(DistrictTypeEnum->GetDisplayNameTextByIndex((int32)DistrictType));
	}
}

void UDistrictListItem::ItemClickedHandle()
{
	if (APlanet* OwningPlanet = Cast<APlanet>(OwningActor))
	{
		OwningPlanet->BuildDistrict(BuildSlotIndex, DistrictType);
		if (UDistrictMenu* DistrictMenu = Cast<UDistrictMenu>(GetOuter()->GetOuter()))
		{
			DistrictMenu->LockDistrictList();
		}
	}
}