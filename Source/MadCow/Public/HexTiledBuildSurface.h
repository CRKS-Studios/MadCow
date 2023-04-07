// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HexTiledBuildSurface.generated.h"

class UStaticMesh;

UCLASS()
class MADCOW_API AHexTiledBuildSurface : public AActor
{
	GENERATED_BODY()
	

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SmallTiles)
	int64 numHexagonWidth = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SmallTiles)
	int64 numHexagonHeight = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SmallTiles)
	float interCenterDistance = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	float noiseMapScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	int seed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	FVector2D offset = FVector2D(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	int octaves = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	float persistance = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	float lacunarity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	TArray<float> biomeRanges;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	TArray<UMaterial*> biomeMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	float heightMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	bool useMeshHeightCurve = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	UCurveFloat* meshHeightCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	float smallestScale = 0.001;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	bool useFalloffMap = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	float curveSlope = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Noise)
	float curveOffset = 2.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomMap)
	bool useCustomHeightmap = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomMap)
	UTexture2D* customHeightmap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomMap)
	UTextureRenderTarget2D* renderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomMap)
	UMaterial* renderTargetMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SmallTiles)
	float capsuleHeight = 40;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SmallTiles)
	UStaticMesh* smallTileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SmallTiles)
	FTransform tileMeshTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug)
	bool bDisplayCapsules = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BigTiles)
	float bigTileSpawnRate = 10.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BigTiles)
	UStaticMesh* bigTileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BigTiles)
	double bigTileScalingParameter = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Biomes)
	TArray<UStaticMesh*> trees;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Biomes)
	UStaticMesh* mountain;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Biomes)
	FTransform detailMeshTransform;


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

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	TArray<float> getCustomHeightMap(UTextureRenderTarget2D* renderTargetToWrite, UMaterial* renderTargetMaterialToWrite, UTexture2D* texture);
	virtual TArray<float> getCustomHeightMap_Implementation(UTextureRenderTarget2D* renderTargetToWrite, UMaterial* renderTargetMaterialToWrite, UTexture2D* texture);

private:
	float interLayerDistance = (UE_DOUBLE_SQRT_3 / 2) * interCenterDistance;
	float centerCornerDistance = interCenterDistance / UE_DOUBLE_SQRT_3;

	TArray<FVector> hexCentersLocations;

	void UpdateSetupVariables();

	void SpawnHexTiles();

	static TArray<float> GenerateNoiseMap(int mapWidth, int mapHeight, int seed, FVector2D offset, float scale, int octaves, float persistance, float lacunarity);
	static TArray<float> GenerateFalloffMap(int mapWidth, int mapHeight, float curveSlope, float curveOffset);

	UMaterial* assignBiome(float heightMapValue);
	UStaticMesh* assignBiomeDetails(float heightMapValue);

	TArray<AActor*> spawnedDetails;

	void WipeOnStart();
};
