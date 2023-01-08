// Fill out your copyright notice in the Description page of Project Settings.


#include "InputFunctionLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"



static void MapKey(UInputMappingContext* InputMappingContext, UInputAction* InputAction, FKey Key, bool bNegate = false, bool bSwizzle = false, EInputAxisSwizzle SwizzleOrder = EInputAxisSwizzle::YXZ)
{
	FEnhancedActionKeyMapping& Mapping = InputMappingContext->MapKey(InputAction, Key);
	UObject* Outer = InputMappingContext->GetOuter();

	if (bNegate)
	{
		UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(Outer);
		Mapping.Modifiers.Add(Negate);
	}

	if (bSwizzle)
	{
		UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(Outer);
		Swizzle->Order = SwizzleOrder;
		Mapping.Modifiers.Add(Swizzle);
	}
}

void UInputFunctionLibrary::AddInputMapping(UInputMappingContext* MappingContext, int32 Priority)
{
	if (!MappingContext) return;

	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(MappingContext->GetOuter()->GetWorld(), 0));
	if (!PlayerController) return;

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		Subsystem->AddMappingContext(MappingContext, Priority);
	}
}

void UInputFunctionLibrary::RemoveInputMapping(UInputMappingContext* MappingContext)
{
	if (!MappingContext) return;

	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(MappingContext->GetOuter()->GetWorld(), 0));
	if (!PlayerController) return;

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		Subsystem->RemoveMappingContext(MappingContext);
	}
}

void UInputFunctionLibrary::BindInputAction(UObject* Object, FName FunctionName, ETriggerEvent TriggerEvent, UInputMappingContext* InputMappingContext, UInputAction* Action, FKey Key, bool bNegate, bool bSwizzle, EInputAxisSwizzle SwizzleOrder)
{
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(Object->GetWorld(), 0));
	if (!PlayerController) return;

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		MapKey(InputMappingContext, Action, Key, bNegate, bSwizzle, SwizzleOrder);
		EnhancedInputComponent->BindAction(Action, TriggerEvent, Object, FunctionName);
	}
}

bool UInputFunctionLibrary::IsKeyDown(UWorld* World, FKey Key)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(World, 0)))
		return PlayerController->IsInputKeyDown(Key);
	
	return false;
}