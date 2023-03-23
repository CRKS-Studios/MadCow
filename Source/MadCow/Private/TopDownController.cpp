// Fill out your copyright notice in the Description page of Project Settings.

#include "TopDownController.h"
#include "EnhancedInputSubsystems.h"

void ATopDownController::BeginPlay()
{
    if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (!InputMapping.IsNull())
            {
                InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 0);
            }
        }
    }
}
