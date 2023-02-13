// Fill out your copyright notice in the Description page of Project Settings.


#include "JobListItem.h"
#include "Components/TextBlock.h"
#include "Stardust/Libraries/ListDataLibrary.h"
#include "Stardust/GameObjects/Planet.h"
#include "Blueprint/WidgetBlueprintLibrary.h"



void UJobListItem::UpdateOccupiedJobs(int32 Change)
{
	OccupiedJobs += Change;

	WorkedJobsText->SetText(FText::AsNumber(OccupiedJobs));
}

void UJobListItem::OnItemAddedHandle(UObject* Object)
{
	if (UJobListData* Data = Cast<UJobListData>(Object))
	{
		int32 TotalJobs;
		Data->GetJobData(JobType, TotalJobs, OccupiedJobs, BuildSlotIndex, OwningActor);

		if (UEnum* JobTypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EJobType"), true))
		{
			JobName->SetText(JobTypeEnum->GetDisplayNameTextByIndex((int32)JobType));
		}

		WorkedJobsText->SetText(FText::AsNumber(OccupiedJobs));
		TotalJobsText->SetText(FText::AsNumber(TotalJobs));
	}
}

FReply UJobListItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UJobListItem::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (OccupiedJobs == 0) return;
	if (!PopWidgetClass) return;

	UUserWidget* Widget = CreateWidget(GetWorld(), PopWidgetClass);
	UDragDropOperation* Operation = UWidgetBlueprintLibrary::CreateDragDropOperation(UListItemDragOperation::StaticClass());

	if (UListItemDragOperation* ItemOperation = Cast<UListItemDragOperation>(Operation))
	{
		ItemOperation->DefaultDragVisual = Widget;
		ItemOperation->OriginPtr = this;

		UE_LOG(LogTemp, Warning, TEXT("Drag detected"))

		OutOperation = ItemOperation;
	}
}

bool UJobListItem::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UListItemDragOperation* ItemOperation = Cast<UListItemDragOperation>(InOperation);
	if (OwningActor && ItemOperation && ItemOperation->OriginPtr)
	{
		if (ItemOperation->OriginPtr->JobType != JobType)
		{
			UJobListItem* OtherJobItem = ItemOperation->OriginPtr;

			if (APlanet* OwningPlanet = Cast<APlanet>(OwningActor))
			{
				OwningPlanet->FreeJob(BuildSlotIndex, OtherJobItem->JobType);
				OwningPlanet->PopulateJob(BuildSlotIndex, JobType);
			}

			UpdateOccupiedJobs(1);
			OtherJobItem->UpdateOccupiedJobs(-1);

			UE_LOG(LogTemp, Warning, TEXT("Updated"))

			return true;
		}
		UE_LOG(LogTemp, Warning, TEXT("Same type"))
	}

	UWidgetBlueprintLibrary::CancelDragDrop();
	return false;
}
