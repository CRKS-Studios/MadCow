// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SceneComponent.h"
#include "TileComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/CapsuleComponent.h"
#include "Math/TransformCalculus3D.h"
#include "HexTiledBuildSurface.h"

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

	for (int i = 0; i < numHexagonWidth; i++) {
		for (int j = 0; j < numHexagonHeight; j++) {
			FVector loc = hexCentersLocations[i * numHexagonHeight + j];

			// Spawn small tiles

			loc.Z += FMath::FRandRange(-tileMeshTransform.GetLocation().Z, tileMeshTransform.GetLocation().Z);
			FTransform newTileTransform = FTransform(tileMeshTransform.GetRotation(), loc, tileMeshTransform.GetScale3D());

			FName tileName = FName(*FString::Printf(TEXT("Tile%d"), j * numHexagonWidth + i));

			UTileComponent* newTileComponent = NewObject<UTileComponent>(this, UTileComponent::StaticClass(), tileName);
			newTileComponent->RegisterComponent();
			newTileComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			newTileComponent->SetRelativeTransform(newTileTransform);
			newTileComponent->SetStaticMesh(smallTileMesh);
			newTileComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1); // set object type to PlacingZone
			newTileComponent->SetGenerateOverlapEvents(true);

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
				FTransform newBigTileTransform = FTransform(tileMeshTransform.GetRotation(), v, tileMeshTransform.GetScale3D() * (2.0 + bigTileScalingParameter));


				FName bigTileName = FName(*FString::Printf(TEXT("BigTile%d"), j * numHexagonWidth + i));

				UTileComponent* newBigTileComponent = NewObject<UTileComponent>(this, UTileComponent::StaticClass(), bigTileName);
				newBigTileComponent->RegisterComponent();
				newBigTileComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				newBigTileComponent->SetRelativeTransform(newBigTileTransform);
				newBigTileComponent->SetStaticMesh(bigTileMesh);
			}
		}
	}
}

