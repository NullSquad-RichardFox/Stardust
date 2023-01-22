// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildQueueListItem.h"
#include "Stardust/Libraries/ListDataLibrary.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"



void UBuildQueueListItem::NativeOnInitialized()
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->DayUpdateEvent.AddDynamic(this, &UBuildQueueListItem::DayUpdate);
	}
}

void UBuildQueueListItem::OnItemAddedHandle(UObject* Object)
{
	UBuildQueueListData* Data = Cast<UBuildQueueListData>(Object);
	if (!Data) return;

	Data->GetBuildQueueData(Name, Progress, Total, QueueIndex);

	if (!Progress) return;


	BuildingNameText->SetText(Name);
	QueueIndexText->SetText(FText::FromString(FString::FormatAsNumber(QueueIndex + 1) + "."));
	BuildProgress->SetPercent(1 - *Progress / Total);
}

void UBuildQueueListItem::DayUpdate()
{
	if (Progress)
		BuildProgress->SetPercent(1 - *Progress / Total);
}
