// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameActor.h"
#include "Stardust/CoreDataStructs.h"
#include "Planet.generated.h"


class UCameraComponent;

UCLASS()
class STARDUST_API APlanet : public AGameActor
{
	GENERATED_BODY()
	
public:
	APlanet();

protected:
	virtual void BeginPlay() override;
	virtual void OnDayUpdate() override;
	virtual void OnMonthUpdate() override;
	virtual void OnClicked() override;

private:
	void CreatePlanet();
	void GenerateFeature(int32 FeatureIndex, FBuildSlot& BuildSlot);
	void GeneratePlanetaryFeature(int32 FeatureIndex);

private:
	UPROPERTY(EditAnywhere)
	UCameraComponent* WidgetCamera;

	UPROPERTY(VisibleAnywhere, Category = "Infrastructure")
	int32 MaxBuildStackCount;
	UPROPERTY(VisibleAnywhere, Category = "Infrastructure")
	TArray<FBuildSlot> BuildSlots;

	UPROPERTY(VisibleAnywhere, Category = "Infrastructure|Population")
	int32 Population;
	UPROPERTY(VisibleAnywhere, Category = "Infrastructure|Population")
	float PopulationGrowth;
	UPROPERTY(VisibleAnywhere, Category = "Infrastructure|Population")
	float PopulationGrowthThreshold = MAX_FLT;

	UPROPERTY(VisibleAnywhere, Category = "Planet")
	int32 PlanetSize;
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	EPlanetType PlanetType;
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	TArray<EFeatureType> PlanetaryFeatures;
	UPROPERTY(VisibleAnywhere, Category = "Planet")
	TArray<EPlanetModifier> PlanetaryModifiers;

	float PartialPopulation;
	float UnemployedPopulation;
};
