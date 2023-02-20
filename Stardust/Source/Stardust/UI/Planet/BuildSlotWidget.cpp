// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildSlotWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Stardust/UI/Planet/PlanetWidget.h"
#include "Stardust/UI/Components/ListItems/JobListItem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Stardust/GameObjects/Planet.h"


#if UE_BUILD_DEVELOPMENT
#define LOG(text, ...) UE_LOG(LogTemp, Warning, TEXT("[%s][%i]: "##text), *GetName(), __LINE__, ##__VA_ARGS__)
#else
#define LOG(text, ...)
#endif

void UBuildSlotWidget::NativeOnInitialized()
{
	HitBox->OnClicked.AddDynamic(this, &UBuildSlotWidget::ClickEvent);
}

void UBuildSlotWidget::Preload(AActor* ParentActor, int32 InBuildSlotIndex)
{
	OwningActor = ParentActor;
	BuildSlotIndex = InBuildSlotIndex;

	NotificationCount->SetVisibility(ESlateVisibility::Collapsed);
	NotificationIcon->SetVisibility(ESlateVisibility::Collapsed);
}

void UBuildSlotWidget::ClickEvent()
{
	if (UPlanetWidget* PlanetWidget = Cast<UPlanetWidget>(GetParent()->GetOuter()->GetOuter()))
	{
		PlanetWidget->BuildSlotClicked(BuildSlotIndex);
	}
}

bool UBuildSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UListItemDragOperation* ItemOperation = Cast<UListItemDragOperation>(InOperation);
	if (OwningActor && ItemOperation && ItemOperation->OriginPtr)
	{
		if (ItemOperation->OriginPtr->BuildSlotIndex != BuildSlotIndex)
		{
			UJobListItem* OtherJobItem = ItemOperation->OriginPtr;

			if (APlanet* OwningPlanet = Cast<APlanet>(OwningActor))
			{
				LOG("Hello #1")
				if (OwningPlanet->PopulateJob(BuildSlotIndex))
				{
					OwningPlanet->FreeJob(OtherJobItem->BuildSlotIndex, OtherJobItem->JobType);
					OtherJobItem->UpdateOccupiedJobs(-1);

					LOG("Hello #2")

					return true;
				}
			}
		}
	}

	UWidgetBlueprintLibrary::CancelDragDrop();
	return false;
}