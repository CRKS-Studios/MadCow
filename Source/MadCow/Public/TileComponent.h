// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "TileComponent.generated.h"

/**
 * 
 */
UCLASS()
class MADCOW_API UTileComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
private:
	bool isOccupied;

public:
	UFUNCTION(BlueprintCallable)
	void setIsOccupied(bool val);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool getIsOccupied();
};
