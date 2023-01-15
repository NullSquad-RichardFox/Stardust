// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingSlotWidget.generated.h"


class UButton;

UCLASS()
class STARDUST_API UBuildingSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UButton* HitBox;
};
