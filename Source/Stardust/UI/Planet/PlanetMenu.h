// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlanetMenu.generated.h"


class UListView;

UCLASS()
class STARDUST_API UPlanetMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Preload(AActor* ParentActor);

protected:
	UFUNCTION()
	void MonthlyUpdate();

	virtual void NativeOnInitialized() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UListView* ResourceList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UListView* TradeRouteList;

private:
	AActor* OwningActor;
};
