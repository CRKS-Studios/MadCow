// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "SnapGridVolumeComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MADCOW_API USnapGridVolumeComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	int64 numHexagonWidth = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	int64 numHexagonHeight = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	float interCenterDistance = 10.0;

private:
	float interLayerDistance = (std::sqrt(3) / 2) * interCenterDistance;

	float XSize;
	float YSize;
	float ZLoc;

public:
	void BeginPlay();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FVector> getHexCentersCoords();
};
