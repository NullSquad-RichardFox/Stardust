// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCorporation.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/GameObjects/Planet.h"



bool APlayerCorporation::Purchase(float Cost)
{
	if (Money >= Cost)
	{
		Money -= Cost;
		return true;
	}
	
	return false;
}

void APlayerCorporation::Refund(float Cost)
{
	Money += Cost;
}


void APlayerCorporation::AddPlanet(APlanet* Planet)
{
	Planets.Add(Planet);
}

void APlayerCorporation::SendTradeRoute(FTradeRoute TradeRoute)
{
	AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode());
	APlanet* Origin = Cast<APlanet>(TradeRoute.Origin);
	if (!GameMode || !Origin) return;

	// Sets timer for payment
	const FGameTimerData* Data = GameMode->SetTimer(this, TEXT("GetMoney"), TradeRoute.Rate);

	// Sets current time ptr
	TradeRoute.Time = Data->GetTimePtr();

	int32 Index = TradeRoutes.Add(TradeRoute);

	// Updates widgets & stores trade route data
	Origin->TradeRouteSent(TradeRoutes[Index]);
}

void APlayerCorporation::GetMoney()
{
	// Payment

	Money += 0.f;

	// Syncs planet data and updates widgets
	const FTradeRoute& TradeRoute = TradeRoutes[0];
	if (APlanet* Origin = Cast<APlanet>(TradeRoute.Origin))
	{
		Origin->TradeRouteFinished(TradeRoute);
	}

	// Removes trade route info
	TradeRoutes.RemoveAt(0, 1, true);
}
