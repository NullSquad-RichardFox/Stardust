// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCorporation.generated.h"

/**
 * 
 */
UCLASS()
class STARDUST_API APlayerCorporation : public AActor
{
	GENERATED_BODY()
	
public:
	bool Purchase(float Cost);

private:
	float Money;
};
