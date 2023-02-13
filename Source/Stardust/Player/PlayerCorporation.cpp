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
	float TotalValue = 0.f;

	for (const auto& [Type, Amount] : TradeRoutes[0].Resources)
	{
		if (const FResourcePrice* Price = UStructDataLibrary::GetData(Type))
		{
			TotalValue += Price->Price * Amount;
		}
	}
		


	// Payment

	Money += TotalValue;

	UE_LOG(LogTemp, Warning, TEXT("Money: %f"), Money)

	// Syncs planet data and updates widgets
	const FTradeRoute& TradeRoute = TradeRoutes[0];
	if (APlanet* Origin = Cast<APlanet>(TradeRoute.Origin))
	{
		Origin->TradeRouteFinished(TradeRoute);
	}

	// Removes trade route info
	TradeRoutes.RemoveAt(0, 1, true);
}
