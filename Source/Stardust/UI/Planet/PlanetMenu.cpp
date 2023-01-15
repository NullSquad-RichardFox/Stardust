// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetMenu.h"
#include "Components/ListView.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/GameObjects/Planet.h"
#include "Stardust/CoreDataStructs.h"
#include "Stardust/Libraries/ListDataLibrary.h"



void UPlanetMenu::NativeOnInitialized()
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->MonthUpdateEvent.AddDynamic(this, &UPlanetMenu::MonthlyUpdate);
	}
}

void UPlanetMenu::MonthlyUpdate()
{
	Preload(OwningActor);
}

void UPlanetMenu::Preload(AActor* ParentActor)
{
	if (!ParentActor) return;

	APlanet* OwningPlanet = Cast<APlanet>(ParentActor);
	OwningActor = ParentActor;

	if (!OwningPlanet) return;

	ResourceList->ClearListItems();

	TMap<EResourceType, float> ResourceGeneration = OwningPlanet->GetResourceProduction();

	for (const auto& [ResourceType, ResourceAmount] : OwningPlanet->GetResourceUpkeep())
		ResourceGeneration.FindOrAdd(ResourceType) -= ResourceAmount;

	for (const auto& [ResourceType, ResourceStorage] : OwningPlanet->GetResourceStorage())
	{
		UResourceListData* Item = NewObject<UResourceListData>(this);
		Item->MakeResourceData(ResourceType, ResourceStorage, ResourceGeneration.Contains(ResourceType) ? ResourceGeneration[ResourceType] : 0);
		ResourceList->AddItem(Item);
	}
}