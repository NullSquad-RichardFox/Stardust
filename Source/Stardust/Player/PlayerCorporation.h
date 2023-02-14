// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Stardust/CoreDataStructs.h"
#include "PlayerCorporation.generated.h"


class APlanet;
class AGameActor;

UCLASS()
class STARDUST_API APlayerCorporation : public AActor
{
	GENERATED_BODY()
	
public:
	bool Purchase(float Cost);
	void Refund(float Cost);

	int32 AddPlanet(APlanet* Planet);
	APlanet* GetClosestPlanet(APlanet* OtherPlanet, float& OutDist);
	bool IsPlanetOwned(APlanet* Planet);

	void SendTradeRoute(FTradeRoute TradeRoute);

	float GetPlayerMoney() { return Money; }

private:
	UFUNCTION()
	void GetMoney();

	TArray<FTradeRoute> TradeRoutes;
	TArray<APlanet*> Planets;
	float Money = 10000.0f;
};
