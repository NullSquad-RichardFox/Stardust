// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Stardust/CoreDataStructs.h"
#include "DistrictListItem.generated.h"


class UImage;
class UButton;
class UTextBlock;

UCLASS()
class STARDUST_API UDistrictListItem : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnItemAddedHandle(UObject* Object);

	virtual void NativeOnInitialized() override;

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* Background;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* DistrictIcon;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UImage* ColorShade;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UTextBlock* DistrictName;
	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	UButton* HitBox;

private:
	UFUNCTION()
	void ItemClickedHandle();

	int32 BuildSlotIndex;
	EDistrictType DistrictType;
	AActor* OwningActor;
};
