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

int64 ATopDownController::getMoneyCount()
{
    return this->moneyCount;
}

void ATopDownController::setMoneyCount(int64 money)
{
    this->moneyCount = money;
}

void ATopDownController::increaseMoneyCount(int64 money)
{
    this->moneyCount += money;
}

