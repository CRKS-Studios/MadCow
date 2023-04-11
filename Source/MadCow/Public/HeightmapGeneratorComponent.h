// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeightmapGeneratorComponent.generated.h"

UCLASS( Blueprintable, ClassGroup=(BuildSurface), meta=(BlueprintSpawnableComponent) )
class MADCOW_API UHeightmapGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Global)
	bool bDisabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation", meta = (EditCondition = "!bDisabled && !bUseCustomHeightmap", DisplayPriority = "1"))
	bool bUseNoise = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Noise", meta = (EditCondition = "!bDisabled && bUseNoise", DisplayPriority = "4"))
	int64 mapHeight = 64;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Noise", meta = (EditCondition = "!bDisabled && bUseNoise", DisplayPriority = "4"))
	int64 mapWidth = 64;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Noise", meta = (EditCondition = "!bDisabled && bUseNoise"))
	float noiseMapScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Noise", meta = (EditCondition = "!bDisabled && bUseNoise"))
	int seed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Noise", meta = (EditCondition = "!bDisabled && bUseNoise"))
	FVector2D offset = FVector2D(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Noise", meta = (EditCondition = "!bDisabled && bUseNoise"))
	int octaves = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Noise", meta = (EditCondition = "!bDisabled && bUseNoise"))
	float persistance = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Noise", meta = (EditCondition = "!bDisabled && bUseNoise"))
	float lacunarity = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Noise", meta = (EditCondition = "!bDisabled && bUseNoise"))
	bool bUseFalloffMap = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|General", meta = (EditCondition = "!bDisabled", DisplayPriority = "3"))
	float heightMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|General", meta = (EditCondition = "!bDisabled"))
	bool bUseMeshHeightCurve = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|General", meta = (EditCondition = "!bDisabled"))
	UCurveFloat* meshHeightCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|General", meta = (EditCondition = "!bDisabled"))
	float smallestScale = 0.001;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|General", meta = (EditCondition = "!bDisabled"))
	float curveSlope = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|General", meta = (EditCondition = "!bDisabled"))
	float curveOffset = 2.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation", meta = (EditCondition = "!bDisabled && !bUseNoise", DisplayPriority = "2"))
	bool bUseCustomHeightmap = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Custom Map", meta = (EditCondition = "!bDisabled && bUseCustomHeightmap", DisplayPriority = "5"))
	UTexture2D* customHeightmap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Custom Map", meta = (EditCondition = "!bDisabled && bUseCustomHeightmap"))
	UTextureRenderTarget2D* renderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heightmap Generation|Custom Map", meta = (EditCondition = "!bDisabled && bUseCustomHeightmap"))
	UMaterial* renderTargetMaterial;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	TArray<float> GetCustomHeightMap(UTextureRenderTarget2D* renderTargetToWrite, UMaterial* renderTargetMaterialToWrite, UTexture2D* texture);
	virtual TArray<float> GetCustomHeightMap_Implementation(UTextureRenderTarget2D* renderTargetToWrite, UMaterial* renderTargetMaterialToWrite, UTexture2D* texture);

	TArray<float> GenerateNoiseMap(int width, int height, int seed, FVector2D mapOffset, float scale, int octaves, float persistance, float lacunarity);
	TArray<float> GenerateFalloffMap(int width, int height, float curveSlope, float curveOffset);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Sets default values for this component's properties
	UHeightmapGeneratorComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<float> GetOutputHeightmap();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetHeightmapSizeX();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetHeightmapSizeY();
};
