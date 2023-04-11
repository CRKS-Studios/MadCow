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

	float tileHeight;

	TSet<AActor*> spawnedDetail;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Tile)
	bool isPrimaryTile = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Tile)
	float spawnRate = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Biome)
	TMap<UStaticMesh*, FFloatRange> detailsMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Biome)
	TMap<UMaterial*, FFloatRange> materialsMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Biome)
	FVector detailMeshScale = FVector(1.0, 1.0, 1.0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GridSnapping)
	int64 heightDivisor = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GridSnapping)
	int64 widthDivisor = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GridSnapping)
	int64 evenHeightOffset = 0;


private:
	UMaterialInterface* GetBiomeMaterial(float heightMapValue);

	UStaticMesh* GetBiomeDetail(float heightMapValue);

public:
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	UFUNCTION(BlueprintCallable)
	void SetIsOccupied(bool val);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsOccupied();

	UFUNCTION(BlueprintCallable)
	void SetTileHeight(float newHeight);

	UFUNCTION(BlueprintCallable)
	void ApplyBiome();

	bool ShouldSpawnOnCoordinates(int64 widthCoordinate, int64 heightCoordinate);
};
