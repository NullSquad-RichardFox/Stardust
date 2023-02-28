// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActor.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/Libraries/InputFunctionLibrary.h"
#include "Stardust/UI/TradeRoutePicker.h"



AGameActor::AGameActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ActorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = ActorMeshComponent;
}

bool AGameActor::IsConnectedTo(AGameActor* OtherActor, int32& OutConnectionIndex)
{
	AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
		return false;

	for (const TObjectPtr<AConnection>& Connection : OtherActor->Connections)
	{
		int32 Index;
		if (!Connections.Find(Connection, Index)) continue;

		if (GameMode->FindActorConnection(Connections[Index], OutConnectionIndex))
		{
			return true;
		}
	}

	OutConnectionIndex = -1;
	return false;
}

void AGameActor::SetFindRouteActive(UTradeRoutePicker* TradeRoutePicker)
{
	TradeRouteUI = TradeRoutePicker;
}

ACorporation* AGameActor::GetCorporation()
{
	return Corporation.Get(nullptr);
}

void AGameActor::BeginPlay()
{
	Super::BeginPlay();

	AGameFramework* GameMode = Cast<AGameFramework>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	GameMode->DayUpdateEvent.AddDynamic(this, &AGameActor::OnDayUpdate);
	GameMode->MonthUpdateEvent.AddDynamic(this, &AGameActor::OnMonthUpdate);

	OnBeginCursorOver.AddDynamic(this, &AGameActor::OnMouseHovered);
	OnEndCursorOver.AddDynamic(this, &AGameActor::OnMouseUnhovered);

	GameActorMappingContext = NewObject<UInputMappingContext>(this);

	ClickAction = NewObject<UInputAction>(this);
	ClickAction->ValueType = EInputActionValueType::Boolean;
	ClickAction->bConsumeInput = false;

	UInputFunctionLibrary::BindInputAction(this, TEXT("OnLeftMouseClick"), ETriggerEvent::Started, GameActorMappingContext, ClickAction, EKeys::LeftMouseButton);
	UInputFunctionLibrary::AddInputMapping(GameActorMappingContext);
}

void AGameActor::OnMouseHovered(AActor* TouchedActor)
{
	if (TouchedActor == this)
		bMouseOver = true;
}

void AGameActor::OnMouseUnhovered(AActor* TouchedActor)
{
	if (TouchedActor == this)
		bMouseOver = false;
}

void AGameActor::OnLeftMouseClick()
{
	if (bMouseOver)
		OnClicked();
}

void AGameActor::OnClicked()
{
	if (UTradeRoutePicker* TradeRoutePicker = Cast<UTradeRoutePicker>(TradeRouteUI.Get(nullptr)))
	{
		TradeRoutePicker->ActorClicked(this);
	}
}

void AGameActor::OnDayUpdate() {}
void AGameActor::OnMonthUpdate() {}