// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexTiledBuildSurface.generated.h"

class UStaticMesh;
class UHeightmapGeneratorComponent;
class UTileComponent;

UCLASS()
class MADCOW_API AHexTiledBuildSurface : public AActor
{
	GENERATED_BODY()
	
private:
	// Components
	TArray<UTileComponent*> tileTemplates;

	UTileComponent* primaryTileTemplate;

	float interLayerDistance = (UE_DOUBLE_SQRT_3 / 2) * interCenterDistance;

	float centerCornerDistance = interCenterDistance / UE_DOUBLE_SQRT_3;

	TArray<FVector> hexCentersLocations;

	USceneComponent* templatesRoot;
	USceneComponent* spawnedTileRoot;

public:	
	UPROPERTY(BlueprintReadOnly)
	UHeightmapGeneratorComponent* heightmapGenerator;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapSetup, meta = (EditCondition = "!IsValid(heightmapGenerator) || heightmapGenerator->disabled"))
	int64 numHexagonWidth = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapSetup, meta = (EditCondition = "!IsValid(heightmapGenerator) || heightmapGenerator->disabled"))
	int64 numHexagonHeight = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapSetup)
	float interCenterDistance = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapSetup)
	float capsuleHeight = 40;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug)
	bool bDisplayTemplates = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug)
	bool bDisplayCapsules = false;



	// Sets default values for this actor's properties
	AHexTiledBuildSurface();

private:
	void UpdateSetupVariables();

	void SpawnHexTiles();

	void WipeOnStart();

	void UpdateTileList();

	UTileComponent* SpawnTileByTemplate(UTileComponent* tileTemplate, FName name, FVector relativeLocation, float noiseMultiplier);

	void SpawnCapsuleForTile(UTileComponent* parentTile);

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
};
