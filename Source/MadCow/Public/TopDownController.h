// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "TopDownController.generated.h"

/**
 * 
 */
UCLASS()
class MADCOW_API ATopDownController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping;

private:
	int64 moneyCount = 100;
public:
	void BeginPlay();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int64 getMoneyCount();

	UFUNCTION(BlueprintCallable)
	void setMoneyCount(int64 money);

	UFUNCTION(BlueprintCallable)
	void increaseMoneyCount(int64 money);
};
