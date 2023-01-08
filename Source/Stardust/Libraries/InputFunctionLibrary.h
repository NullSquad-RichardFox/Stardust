// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputFunctionLibrary.generated.h"



UCLASS()
class STARDUST_API UInputFunctionLibrary : public UObject
{
	GENERATED_BODY()
	
public:
	static bool IsKeyDown(UWorld* World, FKey Key);

	static void AddInputMapping(UInputMappingContext* MappingContext, int32 Priority = 0);
	static void RemoveInputMapping(UInputMappingContext* MappingContex);

	static void BindInputAction(UObject* Object, FName FunctionName, ETriggerEvent TriggerEvent, UInputMappingContext* MappingContext, UInputAction* Action, FKey Key, bool bNagate = false, bool bSwizzle = false, EInputAxisSwizzle SwizzleOrder = EInputAxisSwizzle::YXZ);
};
