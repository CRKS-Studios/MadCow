// Fill out your copyright notice in the Description page of Project Settings.

#include "HexTiledBuildSurface.h"
#include "Components/SceneComponent.h"
#include "TileComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/CapsuleComponent.h"
#include "Math/TransformCalculus3D.h"
#include "HeightmapGeneratorComponent.h"
#include "Engine/StaticMeshActor.h" 

// Sets default values
AHexTiledBuildSurface::AHexTiledBuildSurface()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	this->hexCentersLocations = TArray<FVector>();
	this->tileTemplates = TArray<UTileComponent*>();

	USceneComponent* sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	sceneComponent->RegisterComponent();
	this->SetRootComponent(sceneComponent);

	this->templatesRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Templates Root"));
	this->templatesRoot->RegisterComponent();
	this->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	this->spawnedTileRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Spawned Tiles Root"));
	this->spawnedTileRoot->RegisterComponent();
	this->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AHexTiledBuildSurface::BeginPlay()
{
	Super::BeginPlay();	
	UpdateTileList();
	UpdateSetupVariables();
	WipeOnStart();
	SpawnHexTiles();
}

void AHexTiledBuildSurface::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateTileList();
	UpdateSetupVariables();
	WipeOnStart();
	if (!bDisplayTemplates) {
		SpawnHexTiles();
	}
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
	if (IsValid(heightmapGenerator) && !heightmapGenerator->bDisabled) {
		this->numHexagonWidth = heightmapGenerator->GetHeightmapSizeX();
		this->numHexagonHeight = heightmapGenerator->GetHeightmapSizeY();
	}

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
	TArray<float> heightmap = TArray<float>();
	if (IsValid(heightmapGenerator) && !heightmapGenerator->bDisabled) {
		heightmap = this->heightmapGenerator->GetOutputHeightmap();
	}
	else {
		heightmap.Init(1.0, hexCentersLocations.Num());
	}

	// For each position in the array of tile locations
	for (int j = 0; j < numHexagonHeight; j++) {
		for (int i = 0; i < numHexagonWidth; i++) {
			// Get the new location of the tile
			FVector loc = hexCentersLocations[j * numHexagonWidth + i];
			// Get the noise value at that location
			float noiseValue = heightmap[j * numHexagonWidth + i];
			
			if (IsValid(primaryTileTemplate)) {
				FString tileName = FString("PrimaryTile");
				FName(*FString::Printf(TEXT("%d"), j * numHexagonWidth + i)).AppendString(tileName);
				UTileComponent* spawnedTile = SpawnTileByTemplate(primaryTileTemplate, FName(tileName), loc, noiseValue);
				SpawnCapsuleForTile(spawnedTile);
			}

			// For each template that we have
			for (auto tileT : tileTemplates) {

				if (tileT->ShouldSpawnOnCoordinates(i, j)) {
					// Create name and spawn tile
					FString tileName;
					tileT->GetName(tileName);
					FName(*FString::Printf(TEXT("%d"), j * numHexagonWidth + i)).AppendString(tileName);
					UTileComponent* spawnedTile = SpawnTileByTemplate(tileT, FName(tileName), loc, noiseValue);
				}
				// ((ovo za big nije bazirano na sve tri nego samo jednoj malo varam))
			}
		}
	}
}


void AHexTiledBuildSurface::WipeOnStart() {
	TArray<USceneComponent*, FDefaultAllocator> children = TArray<USceneComponent*, FDefaultAllocator>();
	this->spawnedTileRoot->GetChildrenComponents(true, children);

	for (auto c : children) {
		c->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		c->DestroyComponent();
	}
}

void AHexTiledBuildSurface::UpdateTileList()
{
	this->tileTemplates.Empty();
	this->tileTemplates = TArray<UTileComponent*>();

	TArray<USceneComponent*> arr = TArray<USceneComponent*>();
	this->templatesRoot->GetChildrenComponents(true, arr);

	for (USceneComponent* p : arr) {
		auto t = Cast<UTileComponent>(p);

		if (!IsValid(t)) continue;

		t->SetHiddenInGame(!bDisplayTemplates);
		t->SetVisibility(bDisplayTemplates);
		if (t->isPrimaryTile) {
			this->primaryTileTemplate = t;
		}
		else {
			this->tileTemplates.Add(t);
		}
	}

	this->heightmapGenerator = Cast<UHeightmapGeneratorComponent>(GetComponentByClass(UHeightmapGeneratorComponent::StaticClass()));
}

UTileComponent* AHexTiledBuildSurface::SpawnTileByTemplate(UTileComponent* tileTemplate, FName name, FVector relativeLocation, float noiseMultiplier)
{
	FTransform templateTileTransform = tileTemplate->GetRelativeTransform();
	FVector scale = tileTemplate->GetRelativeScale3D();

	// Spawn small tiles
	scale.Z = noiseMultiplier;

	if (IsValid(heightmapGenerator) && !heightmapGenerator->bDisabled) {
		scale.Z *= heightmapGenerator->heightMultiplier;
	}

	FTransform newTileTransform = FTransform(templateTileTransform.GetRotation(), relativeLocation + templateTileTransform.GetLocation(), scale);

	// UTileComponent* newTileComponent = NewObject<UTileComponent>(this, UTileComponent::StaticClass(), name);
	UTileComponent* newTileComponent = DuplicateObject<UTileComponent>(tileTemplate, this, name);
	newTileComponent->RegisterComponent();
	newTileComponent->AttachToComponent(this->spawnedTileRoot, FAttachmentTransformRules::KeepRelativeTransform);
	newTileComponent->SetVisibility(!bDisplayTemplates);
	newTileComponent->SetHiddenInGame(bDisplayTemplates);
	newTileComponent->SetRelativeTransform(newTileTransform);
	newTileComponent->SetTileHeight(noiseMultiplier);
	newTileComponent->ApplyBiome();

	return newTileComponent;
}

void AHexTiledBuildSurface::SpawnCapsuleForTile(UTileComponent* parentTile)
{
	// Spawn the capsule component for each tile
	float radius = interCenterDistance / 2;
	FVector parentScale = parentTile->GetRelativeScale3D();

	FTransform newerCompTransform = FTransform(FRotator(), FVector(0, 0, capsuleHeight), FVector(1 / parentScale.X, 1 / parentScale.Y, 1 / parentScale.Z));

	FString capsuleName;
	parentTile->GetName(capsuleName);
	FName("Capsule").AppendString(capsuleName);

	UCapsuleComponent* newCapsuleComponent = NewObject<UCapsuleComponent>(this, UCapsuleComponent::StaticClass(), FName(capsuleName));
	newCapsuleComponent->RegisterComponent();
	newCapsuleComponent->SetCapsuleSize(radius, capsuleHeight);
	newCapsuleComponent->AttachToComponent(parentTile, FAttachmentTransformRules::KeepRelativeTransform);
	newCapsuleComponent->SetRelativeTransform(newerCompTransform);
	newCapsuleComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3); // set object type to BuildCollision
	newCapsuleComponent->SetGenerateOverlapEvents(true);

	if (!bDisplayCapsules) {
		newCapsuleComponent->SetVisibility(false);
	}
}
