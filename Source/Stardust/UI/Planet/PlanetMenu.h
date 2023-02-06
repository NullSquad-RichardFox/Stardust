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
	void UpdateTradeRouteList();
	void UpdateResourceList();

	void AddQueueItem(UObject* Item);
	void RemoveQueueItem(int32 Index);

protected:
	UFUNCTION()
	void MonthlyUpdate();

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
	void AddTradeRoute();

	TSubclassOf<UTradeRoutePicker> TradeRoutePickerClass;

	AGameActor* OwningActor;
};
