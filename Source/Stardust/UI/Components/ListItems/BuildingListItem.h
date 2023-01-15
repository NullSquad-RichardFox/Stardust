// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingListItem.generated.h"


class UImage;
class UTextBlock;

UCLASS()
class STARDUST_API UBuildingListItem : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnItemAddedHandle(UObject* Object);

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* Background;
};
