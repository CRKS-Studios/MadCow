// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/TextureRenderTarget2D.h"
#include "HeightmapGeneratorComponent.h"

// Sets default values for this component's properties
UHeightmapGeneratorComponent::UHeightmapGeneratorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

TArray<float> UHeightmapGeneratorComponent::GetOutputHeightmap()
{
	// Generate noise map
	TArray<float> noiseMap;
	TArray<float> falloffMap;

	TArray<float> outputMap = TArray<float>();

	if (!bUseCustomHeightmap) {
		noiseMap = GenerateNoiseMap(mapWidth, mapHeight, seed, offset, noiseMapScale, octaves, persistance, lacunarity);
	}
	else if (IsValid(customHeightmap) && IsValid(renderTargetMaterial) && IsValid(renderTarget)) {
		// sample texture at each point
		noiseMap = GetCustomHeightMap(renderTarget, renderTargetMaterial, customHeightmap);
	}
	else {
		noiseMap.Init(1.0, mapWidth * mapHeight);
	}

	if (bUseNoise && bUseFalloffMap) falloffMap = GenerateFalloffMap(mapWidth, mapHeight, curveSlope, curveOffset);

	if (noiseMap.Num() == 0) {
		UE_LOG(LogTemp, Display, TEXT("noise map did not load"));
		return outputMap;
	}

	if (mapWidth * mapHeight >= 1024 * 1024) {
		UE_LOG(LogTemp, Display, TEXT("the computer is going to explode"));
		return outputMap;
	}

	for (int i = 0; i < ((bUseNoise && bUseFalloffMap) ? FMath::Min(noiseMap.Num(), falloffMap.Num()) : noiseMap.Num()); i++) {
		float noiseSample = noiseMap[i];
		if (bUseNoise && bUseFalloffMap) noiseSample -= falloffMap[i];

		outputMap.Add(noiseSample);
	}

	return outputMap;
}

int32 UHeightmapGeneratorComponent::GetHeightmapSizeX()
{
	if (bUseCustomHeightmap && IsValid(customHeightmap) && IsValid(renderTargetMaterial) && IsValid(renderTarget)) {
		return customHeightmap->GetSizeX();
	}
	else {
		return mapWidth;
	}
}

int32 UHeightmapGeneratorComponent::GetHeightmapSizeY()
{
	if (bUseCustomHeightmap && IsValid(customHeightmap) && IsValid(renderTargetMaterial) && IsValid(renderTarget)) {
		return customHeightmap->GetSizeY();
	}
	else {
		return mapHeight;
	}
}

float UHeightmapGeneratorComponent::GetNormalizedHeightmapValueAt(int64 index)
{
	if (index < 0 || index >= heightmap.Num()) return -1.0;
	return heightmap[index];
}

float UHeightmapGeneratorComponent::GetScaledHeightmapValueAt(int64 index)
{
	if (index < 0 || index >= heightmap.Num()) return -1.0;

	float returnVal = heightmap[index];

	if (!bDisabled) {
		returnVal *= heightMultiplier;

		if (bUseMeshHeightCurve) {
			returnVal *= meshHeightCurve->GetFloatValue(returnVal);
		}

		if (returnVal <= smallestScale) {
			returnVal = smallestScale;
		}
	}

	return returnVal;
}


// Called when the game starts
void UHeightmapGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHeightmapGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<float> UHeightmapGeneratorComponent::GetCustomHeightMap_Implementation(UTextureRenderTarget2D* renderTargetToWrite, UMaterial* renderTargetMaterialToWrite, UTexture2D* texture)
{
	return TArray<float>();
}

/*
Generate perlin noise map from chosen parameters
octaves - number of times sampled / describes level of detail
persistance - influences amplitude of signal, amplitude decreases with octave R[0, 1]
lacunarity - influences frequency of signal, frequency increases with octave R 1+
*/
TArray<float> UHeightmapGeneratorComponent::GenerateNoiseMap(int width, int height, int seedParam, FVector2D mapOffset, FVector2D scaleParam, int octavesParam, float persistanceParam, float lacunarityParam) {
	TArray<float> NoiseMap = TArray<float>();

	// seed
	FMath::RandInit(seedParam);
	// sample octaves at random points
	TArray<FVector2D> octaveOffsets = TArray<FVector2D>(); //size = nr of octaves
	for (int i = 0; i < octavesParam; i++) {
		float offsetX = FMath::RandRange(-10000, 10000) + mapOffset.X;
		float offsetY = FMath::RandRange(-10000, 10000) + mapOffset.Y;
		octaveOffsets.Add(FVector2D(offsetX, offsetY));
	}

	scaleParam.X = FMath::Max(0.0001f, scaleParam.X);
	scaleParam.Y = FMath::Max(0.0001f, scaleParam.Y);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			float amplitude = 1;
			float frequency = 1;
			float noiseHeight = 0;

			for (int i = 0; i < octavesParam; i++) {
				FVector2D samplePos = FVector2D(x / scaleParam.X * frequency + octaveOffsets[i].X, y / scaleParam.Y * frequency + octaveOffsets[i].Y);

				float perlinValue = FMath::PerlinNoise2D(samplePos);
				noiseHeight += perlinValue * amplitude;

				amplitude *= persistanceParam;
				frequency *= lacunarityParam;
			}
			// add height in range 0 - 1
			// nisam sigurna da li je to bolje al avaj problem za poslije hihi
			NoiseMap.Add((noiseHeight + 1.0f) / 2.0f);
		}
	}

	return NoiseMap;
}

/*
Subtract this from the noise map to create an "island" noise map, surrounded with water
*/
TArray<float> UHeightmapGeneratorComponent::GenerateFalloffMap(int mapWidthParam, int mapHeightParam, float curveSlopeParam, float curveOffsetParam) {
	TArray<float> FalloffMap = TArray<float>();

	for (int y = 0; y < mapHeightParam; y++) {
		for (int x = 0; x < mapWidthParam; x++) {
			// range -1 to 1
			float xCoord = (x / (mapWidthParam * 1.0f)) * 2 - 1;
			float yCoord = (y / (mapHeightParam * 1.0f)) * 2 - 1;

			float closestToEdge = FMath::Max(FMath::Abs(xCoord), FMath::Abs(yCoord));

			// apply curve (function), resulting in a smoother map with more regions / less water
			float finalValue = FMath::Pow(closestToEdge, curveSlopeParam) / (FMath::Pow(closestToEdge, curveSlopeParam)
				+ FMath::Pow(curveOffsetParam * (1 - closestToEdge), curveSlopeParam));

			FalloffMap.Add(finalValue);
		}
	}

	return FalloffMap;
}

void UHeightmapGeneratorComponent::UpdateHeightmap()
{
	this->heightmap = GetOutputHeightmap();
}
