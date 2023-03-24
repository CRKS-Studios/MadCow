// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexTiledBuildSurface.generated.h"

class UStaticMesh;

UCLASS()
class MADCOW_API AHexTiledBuildSurface : public AActor
{
	GENERATED_BODY()
	

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	int64 numHexagonWidth = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	int64 numHexagonHeight = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	float interCenterDistance = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	float capsuleHeight = 40;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	UStaticMesh* tileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
	FTransform tileMeshTransform;

	// Sets default values for this actor's properties
	AHexTiledBuildSurface();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called on modify action
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected))
	TArray<FVector> getHexCentersCoords();

private:
	float interLayerDistance = (std::sqrt(3) / 2) * interCenterDistance;

	TArray<FVector> hexCentersLocations;

	void UpdateSetupVariables();

	void SpawnHexTiles();
};
