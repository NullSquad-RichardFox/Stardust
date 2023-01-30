// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCorporation.generated.h"


class APlanet;

UCLASS()
class STARDUST_API APlayerCorporation : public AActor
{
	GENERATED_BODY()
	
public:
	bool Purchase(float Cost);
	void Refund(float Cost);
	void AddPlanet(APlanet* Planet);

private:
	float Money;
};
