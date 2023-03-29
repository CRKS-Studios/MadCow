// Fill out your copyright notice in the Description page of Project Settings.

#include "HexTiledBuildSurface.h"
#include "Components/SceneComponent.h"
#include "TileComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/CapsuleComponent.h"
#include "Math/TransformCalculus3D.h"

// Sets default values
AHexTiledBuildSurface::AHexTiledBuildSurface()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	this->hexCentersLocations = TArray<FVector>();

	USceneComponent* sceneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scene Root"));
	sceneComponent->RegisterComponent();
	this->SetRootComponent(sceneComponent);
}

// Called when the game starts or when spawned
void AHexTiledBuildSurface::BeginPlay()
{
	Super::BeginPlay();	
	UpdateSetupVariables();
	SpawnHexTiles();
}

void AHexTiledBuildSurface::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateSetupVariables();
	SpawnHexTiles();
}

// Called every frame
void AHexTiledBuildSurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<FVector> AHexTiledBuildSurface::getHexCentersCoords()
{
	return this->hexCentersLocations;
}

void AHexTiledBuildSurface::UpdateSetupVariables()
{
	this->interLayerDistance = (UE_DOUBLE_SQRT_3 / 2) * interCenterDistance;
	this->centerCornerDistance = interCenterDistance / UE_DOUBLE_SQRT_3;

	hexCentersLocations.Empty();
	hexCentersLocations = TArray<FVector>();

	for (int i = 0; i < numHexagonWidth; i++) {
		for (int j = 0; j < numHexagonHeight; j++) {
			if (j % 2 == 0) {
				hexCentersLocations.Add(FVector(i * this->interCenterDistance, j * interLayerDistance, 0.0));
			}
			else {
				// if (i == numHexagonWidth - 1) continue;

				hexCentersLocations.Add(FVector((i + 0.5) * this->interCenterDistance, j * interLayerDistance, 0.0));
			}
		}
	}
}

void AHexTiledBuildSurface::SpawnHexTiles()
{
	TArray<USceneComponent*, FDefaultAllocator> children = TArray<USceneComponent*, FDefaultAllocator>();
	GetRootComponent()->GetChildrenComponents(true, children);

	for (auto c : children) {
		c->DetachFromParent();
		c->DestroyComponent();
	}

	// Generate noise map
	TArray<float> noiseMap;

	if (!useCustomHeightmap) {
		noiseMap = GenerateNoiseMap(numHexagonWidth, numHexagonHeight, seed, offset, noiseMapScale, octaves, persistance, lacunarity);
	}
	else {
		// set height and width to texture size 
		numHexagonWidth = customHeightmap->GetSizeX();
		numHexagonHeight = customHeightmap->GetSizeY();
		// sample texture at each point
		noiseMap = SampleExistingNoiseMap(customHeightmap, numHexagonWidth, numHexagonHeight);
	}

	for (int i = 0; i < numHexagonWidth; i++) {
		for (int j = 0; j < numHexagonHeight; j++) {
			FVector loc = hexCentersLocations[i * numHexagonHeight + j];
			
			// Spawn small tiles

			//loc.Z += FMath::FRandRange(-tileMeshTransform.GetLocation().Z, tileMeshTransform.GetLocation().Z);
			float noiseSample = noiseMap[j * numHexagonWidth + i];
			//loc.Z += noiseSample;
			UE_LOG(LogTemp, Display, TEXT("buka zbuka je %f"), noiseSample)

			FVector scale = tileMeshTransform.GetScale3D();
			scale.Z = noiseSample * heightMultiplier;
			if (useMeshHeightCurve) {
				scale.Z *= meshHeightCurve->GetFloatValue(noiseSample);
			}

			FTransform newTileTransform = FTransform(tileMeshTransform.GetRotation(), loc, scale);

			FName tileName = FName(*FString::Printf(TEXT("Tile%d"), j * numHexagonWidth + i));

			UMaterial* chosenMaterial = assignBiome(noiseSample);

			UTileComponent* newTileComponent = NewObject<UTileComponent>(this, UTileComponent::StaticClass(), tileName);
			newTileComponent->RegisterComponent();
			newTileComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			newTileComponent->SetRelativeTransform(newTileTransform);
			newTileComponent->SetStaticMesh(smallTileMesh);
			newTileComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1); // set object type to PlacingZone
			newTileComponent->SetGenerateOverlapEvents(true);
			newTileComponent->SetMaterial(0, chosenMaterial);

			// Spawn the capsule component for each tile

			float radius = interCenterDistance / 2;
			FTransform newerCompTransform = FTransform(FRotator(), FVector(0, 0, capsuleHeight), FVector(1 / tileMeshTransform.GetScale3D().X, 1 / tileMeshTransform.GetScale3D().Y, 1 / tileMeshTransform.GetScale3D().Z));

			FName capsuleName = FName(*FString::Printf(TEXT("TileCapsule%d"), j * numHexagonWidth + i));

			UCapsuleComponent* newCapsuleComponent = NewObject<UCapsuleComponent>(this, UCapsuleComponent::StaticClass(), capsuleName);
			newCapsuleComponent->RegisterComponent();
			newCapsuleComponent->SetCapsuleSize(radius, capsuleHeight);
			newCapsuleComponent->AttachToComponent(newTileComponent, FAttachmentTransformRules::KeepRelativeTransform);
			newCapsuleComponent->SetRelativeTransform(newerCompTransform);
			newCapsuleComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3); // set object type to BuildCollision
			newCapsuleComponent->SetGenerateOverlapEvents(true);

			if (!bDisplayCapsules) {
				newCapsuleComponent->SetVisibility(false);
			}

			bool expression = (j % 4 == 0) ? (i % 2 == 0) : (i % 2 == 1);
			// Spawn big hex tiles with a probability
			if (j % 2 == 0 && expression && FMath::FRandRange(0.0, 100.0) < bigTileSpawnRate) {
				FVector v = loc;
				v.Y -= centerCornerDistance;
				v.Z = tileMeshTransform.GetLocation().Z + 1;

				FVector scaleBig = tileMeshTransform.GetScale3D() * (2.0 + bigTileScalingParameter);
				scaleBig.Z = noiseSample * heightMultiplier;
				if(useMeshHeightCurve) {
					scaleBig.Z *= meshHeightCurve->GetFloatValue(noiseSample);
				}

				FTransform newBigTileTransform = FTransform(tileMeshTransform.GetRotation(), v, scaleBig);

				FName bigTileName = FName(*FString::Printf(TEXT("BigTile%d"), j * numHexagonWidth + i));

				UTileComponent* newBigTileComponent = NewObject<UTileComponent>(this, UTileComponent::StaticClass(), bigTileName);
				newBigTileComponent->RegisterComponent();
				newBigTileComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				newBigTileComponent->SetRelativeTransform(newBigTileTransform);
				newBigTileComponent->SetStaticMesh(bigTileMesh);
				newBigTileComponent->SetMaterial(0, chosenMaterial);

				/*if (noiseSample >= biomeRanges[1]) {
					UStaticMesh* meshToSpawn = assignBiomeDetails(noiseSample);

					UTileComponent* spawnedDetail = NewObject<UTileComponent>(this, UTileComponent::StaticClass(), bigTileName);
					spawnedDetail->RegisterComponent();
					spawnedDetail->AttachToComponent(newBigTileComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
					spawnedDetail->SetRelativeTransform(detailMeshTransform);
					spawnedDetail->SetStaticMesh(meshToSpawn);
				}*/

				// ((ovo za big nije bazirano na sve tri nego samo jednoj malo varam))
			}
		}
	}
}

/*
Generate perlin noise map from chosen parameters
octaves - number of times sampled / describes level of detail
persistance - influences amplitude of signal, amplitude decreases with octave R[0, 1]
lacunarity - influences frequency of signal, frequency increases with octave R 1+
*/
TArray<float> AHexTiledBuildSurface::GenerateNoiseMap(int mapWidth, int mapHeight, int seed, FVector2D offset, float scale, int octaves, float persistance, float lacunarity) {
	TArray<float> NoiseMap = TArray<float>();

	// seed
	FMath::RandInit(seed);
	// sample octaves at random points
	TArray<FVector2D> octaveOffsets = TArray<FVector2D>(); //size = nr of octaves
	for (int i = 0; i < octaves; i++) {
		float offsetX = FMath::RandRange(-10000, 10000) + offset.X;
		float offsetY = FMath::RandRange(-10000, 10000) + offset.Y;
		octaveOffsets.Add(FVector2D(offsetX, offsetY));
	}

	if (scale <= 0) {
		scale = 0.0001f;
	}

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {

			float amplitude = 1;
			float frequency = 1;
			float noiseHeight = 0;

			for (int i = 0; i < octaves; i++) {
				FVector2D samplePos = FVector2D(x / scale * frequency + octaveOffsets[i].X, y / scale * frequency + octaveOffsets[i].Y);

				float perlinValue = FMath::PerlinNoise2D(samplePos);
				noiseHeight += perlinValue * amplitude;

				amplitude *= persistance;
				frequency *= lacunarity;
			}
			// add height in range 0 - 1
			// nisam sigurna da li je to bolje al avaj problem za poslije hihi
			NoiseMap.Add((noiseHeight + 1.0f) / 2.0f);
		}
	}

	return NoiseMap;
}

/*
Za sad ne valja nis 
*/
TArray<float> AHexTiledBuildSurface::SampleExistingNoiseMap(UTexture2D* texture, int mapWidth, int mapHeight) {
	TArray<float> NoiseMap = TArray<float>();
	if (!texture) return NoiseMap;

	if (!texture->PlatformData) {
		UE_LOG(LogTemp, Error, TEXT("SampleExistingNoiseMap: texture's PlatformData is null"));
		return NoiseMap;
	}

	// mipmaps <3
	FTexture2DMipMap* mip = &texture->PlatformData->Mips[0];
	FByteBulkData* raw = &mip->BulkData;
	FColor* mipData = static_cast<FColor*>(raw->Lock(LOCK_READ_ONLY));

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			FColor color = mipData[y * mapWidth + x];

			float sampledColor = (color.R + color.G + color.B) / 3.0f;
			NoiseMap.Add(sampledColor);
		}
	}

	raw->Unlock();

	return NoiseMap;
}

/*
Assigns material reference to specific tile given its height value
*/
UMaterial* AHexTiledBuildSurface::assignBiome(float heightMapValue) {
	if (heightMapValue < biomeRanges[0]) {
		return biomeMaterials[0];
	}
	else if (heightMapValue >= biomeRanges[0] && heightMapValue < biomeRanges[1]) {
		return biomeMaterials[1];
	}
	else if (heightMapValue >= biomeRanges[1] && heightMapValue < biomeRanges[2]) {
		// grass should be on same level so that map is smoother
		bool pick = FMath::RandBool();
		if (pick) {
			return biomeMaterials[2];
		}
		return biomeMaterials[3];
	}
	return biomeMaterials[4];
}

UStaticMesh* AHexTiledBuildSurface::assignBiomeDetails(float heightMapValue) {

	if (heightMapValue >= biomeRanges[1] && heightMapValue < biomeRanges[2]) {
		int random = FMath::RandRange(0, trees.Num() - 1);

		return trees[random];
	}
	
	return mountain;
}