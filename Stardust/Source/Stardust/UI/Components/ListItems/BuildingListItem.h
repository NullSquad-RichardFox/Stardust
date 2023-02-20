// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Stardust/CoreDataStructs.h"
#include "BuildingListItem.generated.h"


class UImage;
class UTextBlock;
class UButton;

UCLASS()
class STARDUST_API UBuildingListItem : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnItemAddedHandle(UObject* Object);

	virtual void NativeOnInitialized() override;

private:	
	UFUNCTION()
	void OnClicked();

protected:
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* Background;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UButton* HitBox;

private:
	AActor* OwningActor;
	int32 BuildSlotIndex;
	EBuildingType BuildingType;
};
