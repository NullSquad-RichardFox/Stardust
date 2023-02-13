// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlanetMenu.generated.h"


class UListView;
class UButton;
class AGameActor;
class UTradeRoutePicker;

UCLASS()
class STARDUST_API UPlanetMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPlanetMenu(const FObjectInitializer& ObjectInitializer);

	void Preload(AGameActor* ParentActor);

	void AddQueueItem(UObject* Item);
	void RemoveQueueItem(int32 Index);

protected:

	virtual void NativeOnInitialized() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UListView* ResourceList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UListView* TradeRouteList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UListView* BuildQueueList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UButton* AddTradeRouteButton;

private:
	UFUNCTION()
	void MonthlyUpdate();

	UFUNCTION()
	void TradeRouteUpdate();
	UFUNCTION()
	void BuildingUpdate(int32 BuildSlotIndex);

	UFUNCTION()
	void AddTradeRoute();

	void UpdateTradeRouteList();
	void UpdateResourceList();

	TSubclassOf<UTradeRoutePicker> TradeRoutePickerClass;

	AGameActor* OwningActor;
};
