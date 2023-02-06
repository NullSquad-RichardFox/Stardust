// Fill out your copyright notice in the Description page of Project Settings.


#include "TradeRoutePicker.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/UI/Planet/PlanetWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Stardust/UI/Components/ListItems/ResourceFlowListItem.h"
#include "Stardust/GameObjects/Planet.h"



void UTradeRoutePicker::NativeOnInitialized()
{
	AddTradeRoute->OnClicked.AddDynamic(this, &UTradeRoutePicker::AddTradeRouteClicked);
	CancelTradeRoute->OnClicked.AddDynamic(this, &UTradeRoutePicker::RemoveWidget);
	MoreItems->OnClicked.AddDynamic(this, &UTradeRoutePicker::AddResourceItems);
	LessItems->OnClicked.AddDynamic(this, &UTradeRoutePicker::RemoveResourceItems);
	PayloadHitBox->OnClicked.AddDynamic(this, &UTradeRoutePicker::PickResource);
}

void UTradeRoutePicker::Load(AGameActor* RouteStartActor)
{
	if (!RouteStartActor) return;

	StartingActor = RouteStartActor;

	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->GetPathfinder().SetStartingActor(StartingActor);
		GameMode->PauseTime();
		GameMode->GetPathfinder().OnRouteUpdate.AddDynamic(this, &UTradeRoutePicker::OnRouteUpdate);

		OnRouteUpdate(0.f, 0.f);

		Payload->SetIconToNone();
		Payload->SetText(FText::AsNumber(0));
	}

	ResourceIndex = 0;
}

void UTradeRoutePicker::AddTradeRouteClicked()
{
	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		TArray<AGameActor*> Actors;
		GameMode->GetPathfinder().GetRouteActors(Actors);

		if (Actors.Num() < 2) return;
		if (Resources.Num() == 0) return;

		FTradeRoute TradeRoute;
		TradeRoute.Origin = Actors[0];
		TradeRoute.Destination = Actors[Actors.Num() - 1];
		TradeRoute.Time = nullptr;
		TradeRoute.Rate = TravelTime;
		TradeRoute.FuelCosts = TotalFuel;
		TradeRoute.DangerLevel = 0;
		TradeRoute.Resources = Resources;

		GameMode->SendTradeRoute(TradeRoute);
	}

	RemoveWidget();
}

void UTradeRoutePicker::RemoveWidget()
{
	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, UPlanetWidget::StaticClass());

	for (UUserWidget* Widget : Widgets)
	{
		if (UPlanetWidget* PlanetWidget = Cast<UPlanetWidget>(Widget))
		{
			PlanetWidget->SetVisibility(ESlateVisibility::Visible);
			PlanetWidget->EnablePlayerMovement(false);
		}
	}

	if (AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->ResumeTime();
		GameMode->GetPathfinder().CleanUp();
	}

	RemoveFromParent();
}

void UTradeRoutePicker::AddResourceItems()
{
	if (APlanet* StartingPlanet = Cast<APlanet>(StartingActor))
	{
		for (auto& [Type, Amount] : Resources)
		{
			 if (StartingPlanet->GetResourceStorage().FindRef(Type) > Amount)
				 Amount++;

			Payload->SetText(FText::AsNumber(Amount));
		}
	}
}

void UTradeRoutePicker::RemoveResourceItems()
{

	for (auto& [Type, Amount] : Resources)
	{
		Amount = FMath::Clamp(Amount--, 0, MAX_FLT);

		Payload->SetText(FText::AsNumber(Amount));
	}
}

void UTradeRoutePicker::PickResource()
{
	if (APlanet* StartingPlanet = Cast<APlanet>(StartingActor))
	{
		TArray<EResourceType> Keys;
		StartingPlanet->GetResourceStorage().GetKeys(Keys);

		if (Keys.IsValidIndex(ResourceIndex)) return;

		EResourceType ChoosenType = Keys[ResourceIndex];

		ResourceIndex++;
		if (ResourceIndex == Keys.Num())
			ResourceIndex = 0;

		Payload->SetResourceType(ChoosenType);
		Resources.Add(ChoosenType);
	}
}

void UTradeRoutePicker::OnRouteUpdate(float Length, float DangerLevel)
{
	constexpr float TravelSpeed = 10.f;
	constexpr float	FuelConsumption = 0.02f;
	constexpr float FuelCost = 10.f;

	TravelTime = FMath::CeilToInt32(Length / TravelSpeed);
	TotalFuel = FuelConsumption * Length;

	RouteFuelConsumptionText->SetText(FText::AsNumber(TotalFuel));
	RouteTravelTimeText->SetText(FText::AsNumber(TravelTime));
	RouteDangerLevelText->SetText(FText::AsNumber(DangerLevel));
}