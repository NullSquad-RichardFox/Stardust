// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCorporation.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/GameObjects/Planet.h"
#include "Stardust/CoreDataStructs.h"



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


int32 APlayerCorporation::AddPlanet(APlanet* Planet)
{
	return Planets.Add(Planet);
}

APlanet* APlayerCorporation::GetClosestPlanet(APlanet* OtherPlanet, float& OutDist)
{
	float Dist = MAX_FLT;
	APlanet* ClosestPlanet = nullptr;

	for (APlanet* Planet : Planets)
	{
		float NewDist = Planet->GetDistanceTo(OtherPlanet);
		if (NewDist < Dist)
		{
			Dist = NewDist;
			ClosestPlanet = Planet;
		}
	}

	OutDist = ClosestPlanet ? Dist : 0;
	return ClosestPlanet;
}

bool APlayerCorporation::IsPlanetOwned(APlanet* Planet)
{
	return Planets.Contains(Planet);
}

void APlayerCorporation::SendTradeRoute(FTradeRoute TradeRoute)
{
	AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode());
	APlanet* Origin = Cast<APlanet>(TradeRoute.Origin);
	if (!GameMode || !Origin) return;

	ACorporation* DestinationCorp = TradeRoute.Destination->GetCorporation();


	if (DestinationCorp == this)
	{
		// Res exchange
	}
	else if (AAICorporation* AICorp = Cast<AAICorporation>(DestinationCorp))
	{
		// AI pays
		// AI gets resources


		for (const auto& [ResType, ResAmount] : TradeRoute.Resources)
		{
			TradeRoute.TradedCredits += GameMode->GetResourcePrice(ResType) * ResAmount;
		}
	}
	else if (ANation* Nation = Cast<ANation>(DestinationCorp))
	{
		// Nation gets resources


		for (const auto& [ResType, ResAmount] : TradeRoute.Resources)
		{
			TradeRoute.TradedCredits += GameMode->GetResourcePrice(ResType) * ResAmount;
		}
	}

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
	const FTradeRoute& TradeRoute = TradeRoutes[0];
	Money += TradeRoute.TradedCredits;

	if (APlanet* Origin = Cast<APlanet>(TradeRoute.Origin))
	{
		Origin->TradeRouteFinished(TradeRoute);
	}

	// Removes trade route info
	TradeRoutes.RemoveAt(0, 1, true);

	UE_LOG(LogTemp, Warning, TEXT("Money: %f"), Money)
}
