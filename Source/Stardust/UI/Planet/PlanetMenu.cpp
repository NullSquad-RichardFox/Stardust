// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetMenu.h"
#include "Components/ListView.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/GameObjects/Planet.h"
#include "Stardust/CoreDataStructs.h"
#include "Stardust/Libraries/ListDataLibrary.h"
#include "Stardust/UI/Components/ListItems/BuildQueueListItem.h"
#include "Stardust/UI/TradeRoutePicker.h"
#include "Stardust/UI/Planet/PlanetWidget.h"
#include "Components/Button.h"



UPlanetMenu::UPlanetMenu(const FObjectInitializer& ObjectInitializer) :UUserWidget(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UTradeRoutePicker> TradeRoutePickerClassObject(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_TradeRoutePicker.WBP_TradeRoutePicker_C'"));
	TradeRoutePickerClass = TradeRoutePickerClassObject.Class;
}

void UPlanetMenu::NativeOnInitialized()
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->MonthUpdateEvent.AddDynamic(this, &UPlanetMenu::MonthlyUpdate);
		AddTradeRouteButton->OnClicked.AddDynamic(this, &UPlanetMenu::AddTradeRoute);
	}
}

void UPlanetMenu::AddTradeRoute()
{
	if (!TradeRoutePickerClass) return;

	UTradeRoutePicker* TradeRouteUI = CreateWidget<UTradeRoutePicker>(GetWorld(), TradeRoutePickerClass);
	TradeRouteUI->Load(OwningActor);
	TradeRouteUI->AddToViewport();

	if (UPlanetWidget* PlanetWidget = Cast<UPlanetWidget>(GetOuter()->GetOuter()))
	{
		PlanetWidget->SetVisibility(ESlateVisibility::Collapsed);
		PlanetWidget->EnablePlayerMovement(true);
	}
}

void UPlanetMenu::MonthlyUpdate()
{
	UpdateResourceList();
}

void UPlanetMenu::Preload(AGameActor* ParentActor)
{
	if (!ParentActor) return;
	OwningActor = ParentActor;

	UpdateResourceList();
	UpdateTradeRouteList();
}

void UPlanetMenu::UpdateTradeRouteList()
{
	APlanet* OwningPlanet = Cast<APlanet>(OwningActor);
	if (!OwningPlanet) return;

	TradeRouteList->ClearListItems();

	for (const FTradeRoute& TradeRoute : OwningPlanet->GetTradeRoutes())
	{
		UTradeRouteListData* Item = NewObject<UTradeRouteListData>(this);
		Item->MakeTradeRouteData(TradeRoute.Origin->GetName(), TradeRoute.Destination->GetName(), TradeRoute.DangerLevel, TradeRoute.Time, TradeRoute.FuelCosts, TradeRoute.Resources);
		TradeRouteList->AddItem(Item);
	}
}

void UPlanetMenu::UpdateResourceList()
{
	APlanet* OwningPlanet = Cast<APlanet>(OwningActor);
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

void UPlanetMenu::AddQueueItem(UObject* Item)
{
	BuildQueueList->AddItem(Item);
}

void UPlanetMenu::RemoveQueueItem(int32 Index)
{
	UObject* Item = BuildQueueList->GetItemAt(Index);
	if (!Item) return;

	BuildQueueList->RemoveItem(Item);

	for (UObject* ListItem : BuildQueueList->GetListItems())
	{
		UBuildQueueListItem* Widget = BuildQueueList->GetEntryWidgetFromItem<UBuildQueueListItem>(ListItem);
		Widget->SetQueueIndex(BuildQueueList->GetIndexForItem(ListItem));
	}
}