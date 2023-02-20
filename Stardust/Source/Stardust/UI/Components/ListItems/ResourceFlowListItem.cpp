// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceFlowListItem.h"
#include "Components/TextBlock.h"
#include "Stardust/Libraries/ListDataLibrary.h"
#include "Components/Image.h"


void UResourceFlowListItem::SetIconToNone()
{
	FlowIcon->SetVisibility(ESlateVisibility::Collapsed);
}

void UResourceFlowListItem::SetText(FText Text)
{
	ValueText->SetText(Text);
}

void UResourceFlowListItem::SetResourceType(EResourceType ResourceType)
{
	if (UEnum* ResourceTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EResourceType"), true))
	{
		ResourceName->SetText(ResourceTypeEnum->GetDisplayNameTextByIndex((int32)ResourceType));
	}
}

void UResourceFlowListItem::OnItemAddedHandle(UObject* Object)
{
	if (UResourceFlowListData* Data = Cast<UResourceFlowListData>(Object))
	{
		EResourceType ResourceType;
		float Amount;
		Data->GetResourceFlowData(Amount, ResourceType);

		if (UEnum* ResourceTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EResourceType"), true))
		{
			ResourceName->SetText(ResourceTypeEnum->GetDisplayNameTextByIndex((int32)ResourceType));
		}

		ValueText->SetText(FText::AsNumber(Amount));
		ValueText->SetColorAndOpacity(FSlateColor(FLinearColor(Amount < 0 ? 255 : 0, Amount > 0 ? 255 : 0, 0)));
	}
}