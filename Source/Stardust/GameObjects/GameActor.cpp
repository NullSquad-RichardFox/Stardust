// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActor.h"
#include "Stardust/GameModes/GameFramework.h"
#include "Stardust/Libraries/InputFunctionLibrary.h"



AGameActor::AGameActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ActorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = ActorMeshComponent;
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

void AGameActor::OnClicked() {}
void AGameActor::OnDayUpdate() {}
void AGameActor::OnMonthUpdate() {}