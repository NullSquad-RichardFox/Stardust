// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCorporation.h"

bool APlayerCorporation::Purchase(float Cost)
{
	if (Money >= Cost)
	{
		Money -= Cost;
		return true;
	}
	
	return false;
}


void APlayerCorporation::AddPlanet(APlanet* Planet)
{

}