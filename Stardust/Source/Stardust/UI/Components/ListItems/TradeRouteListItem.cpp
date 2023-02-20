// Fill out your copyright notice in the Description page of Project Settings.


#include "TradeRouteListItem.h"
#include "Stardust/Libraries/ListDataLibrary.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Components/TextBlock.h"


void UTradeRouteListItem::NativeOnInitialized()
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->DayUpdateEvent.AddDynamic(this, &UTradeRouteListItem::OnDayUpdate);
	}
}

void UTradeRouteListItem::OnItemAddedHandle(UObject* Object)
{
	UTradeRouteListData* Data = Cast<UTradeRouteListData>((Object));
	if (!Data) return;

	FString OriginName;
	FString DestinationName;
	int32 DangerLevel;
	float FuelConsumption;
	TMap<EResourceType, float> Payload;

	Data->GetTradeRouteData(OriginName, DestinationName, DangerLevel, TimeRemaining, FuelConsumption, Payload);

	OriginPlanetNameText->SetText(FText::FromString(OriginName));
	DestinationPlanetNameText->SetText(FText::FromString(DestinationName));
	RemainingTravelTimeText->SetText(FText::AsNumber(*TimeRemaining));
	DangerLevelText->SetText(FText::AsNumber(DangerLevel));
	FuelConsumptionText->SetText(FText::AsNumber(FuelConsumption));
}

void UTradeRouteListItem::OnDayUpdate()
{
	RemainingTravelTimeText->SetText(FText::AsNumber(*TimeRemaining));
}