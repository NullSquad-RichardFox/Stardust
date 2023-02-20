// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceListItem.h"
#include "Components/TextBlock.h"
#include "Stardust/Libraries/ListDataLibrary.h"


void UResourceListItem::OnItemAddedHandle(UObject* Object)
{
	if (UResourceListData* Data = Cast<UResourceListData>(Object))
	{
		EResourceType ResourceType;
		float Amount, Change;
		Data->GetResourceData(ResourceType, Amount, Change);

		if (UEnum* ResourceTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EResourceType"), true))
		{
			ResourceName->SetText(ResourceTypeEnum->GetDisplayNameTextByIndex((int32)ResourceType));
		}

		ValueText->SetText(FText::AsNumber(Amount));

		ResourceChangeText->SetText(FText::AsNumber(Change));
		ResourceChangeText->SetColorAndOpacity(FSlateColor(FLinearColor(Change < 0 ? 255 : 0, Change > 0 ? 255 : 0, 0)));
	}
}