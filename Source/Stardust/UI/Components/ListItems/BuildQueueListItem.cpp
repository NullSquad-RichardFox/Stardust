// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildQueueListItem.h"
#include "Stardust/Libraries/ListDataLibrary.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/GameObjects/Planet.h"
#include "Stardust/UI/Planet/PlanetMenu.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"



void UBuildQueueListItem::NativeOnInitialized()
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->DayUpdateEvent.AddDynamic(this, &UBuildQueueListItem::DayUpdate);
		CancelButton->OnClicked.AddDynamic(this, &UBuildQueueListItem::CancelBuilding);
	}
}

void UBuildQueueListItem::SetQueueIndex(int32 Index)
{
	QueueIndex = Index;
	QueueIndexText->SetText(FText::FromString(FString::FormatAsNumber(QueueIndex + 1) + "."));
}

void UBuildQueueListItem::OnItemAddedHandle(UObject* Object)
{
	UBuildQueueListData* Data = Cast<UBuildQueueListData>(Object);
	if (!Data) return;

	Data->GetBuildQueueData(Name, Progress, Total, QueueIndex, OwningActor);

	if (!Progress) return;
	if (!OwningActor) return;

	BuildingNameText->SetText(Name);
	QueueIndexText->SetText(FText::FromString(FString::FormatAsNumber(QueueIndex + 1) + "."));
	BuildProgress->SetPercent(1 - *Progress / Total);
}

void UBuildQueueListItem::DayUpdate()
{
	if (Progress)
		BuildProgress->SetPercent(1 - *Progress / Total);
}

void UBuildQueueListItem::CancelBuilding()
{
	if (APlanet* OwningPlanet = Cast<APlanet>(OwningActor))
	{
		OwningPlanet->CancelBuildingProcess(QueueIndex);
	}

	if (UPlanetMenu* PlanetMenu = Cast<UPlanetMenu>(GetOuter()->GetOuter()))
	{
		PlanetMenu->RemoveQueueItem(QueueIndex);
	}
}