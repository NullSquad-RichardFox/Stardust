// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Stardust/CoreDataStructs.h"
#include "TradeRoutePicker.generated.h"


class AGameActor;
class UButton;
class UTextBlock;
class UResourceFlowListItem;

UCLASS()
class STARDUST_API UTradeRoutePicker : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Load(AGameActor* RouteStartActor);
	void ActorClicked(AGameActor* InActor);

protected:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void AddTradeRouteClicked();

	UFUNCTION()
	void RemoveWidget();

	UFUNCTION()
	void AddResourceItems();
	UFUNCTION()
	void RemoveResourceItems();
	UFUNCTION()
	void PickResource();

	UFUNCTION()
	void OnRouteUpdate(float Length, float DangerLevel);



protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UButton* AddTradeRoute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UButton* CancelTradeRoute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UTextBlock* RouteTravelTimeText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UTextBlock* RouteFuelConsumptionText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UTextBlock* RouteDangerLevelText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UResourceFlowListItem* Payload;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UButton* PayloadHitBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UButton* MoreItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UButton* LessItems;

private:
	AGameActor* StartingActor;
	TMap<EResourceType, float> Resources;
	int32 TravelTime;
	int32 ResourceIndex;
	float TotalFuel;
};