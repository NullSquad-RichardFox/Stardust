// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TradeRouteListItem.generated.h"


class UImage;
class UTextBlock;

UCLASS()
class STARDUST_API UTradeRouteListItem : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnItemAddedHandle(UObject* Object);

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* Background;
};
