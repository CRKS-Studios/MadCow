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

//void AHexTiledBuildSurface::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
//{
//	UTileComponent* tile = Cast<UTileComponent>(OverlappedComponent->GetAttachParent());
//	if (tile->getIsOccupied() == true) {
//
//	}
//	else {
//	tile->setIsOccupied(true);
//	}
//}

void AHexTiledBuildSurface::UpdateSetupVariables()
{
	this->interLayerDistance = (std::sqrt(3) / 2) * interCenterDistance;

	hexCentersLocations.Empty();
	hexCentersLocations = TArray<FVector>();

	for (int i = 0; i < numHexagonWidth; i++) {
		for (int j = 0; j < numHexagonHeight; j++) {
			if (j % 2 == 0) {
				hexCentersLocations.Add(FVector(i * this->interCenterDistance, j * interLayerDistance, 0.0));
			}
			else {
				if (i == numHexagonWidth - 1) continue;

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

	int i = 0;
	for (FVector loc : hexCentersLocations) {
		loc.Z += FMath::FRandRange(-tileMeshTransform.GetLocation().Z, tileMeshTransform.GetLocation().Z);
		FTransform newCompTransform = FTransform(tileMeshTransform.GetRotation(), loc, tileMeshTransform.GetScale3D());

		FName tileName = FName(*FString::Printf(TEXT("Tile%d"), i));

		UTileComponent* newTileComponent = NewObject<UTileComponent>(this, UTileComponent::StaticClass(), tileName);
		newTileComponent->RegisterComponent();
		newTileComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		newTileComponent->SetRelativeTransform(newCompTransform);
		newTileComponent->SetStaticMesh(tileMesh);
		newTileComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1); // set object type to PlacingZone
		newTileComponent->SetGenerateOverlapEvents(true);

		float radius = interCenterDistance / 2;
		float height = 200;
		FTransform newerCompTransform = FTransform(FRotator(),FVector(0, 0, height), FVector(1 / tileMeshTransform.GetScale3D().X, 1 / tileMeshTransform.GetScale3D().Y, 1 / tileMeshTransform.GetScale3D().Z));

		FName capsuleName = FName(*FString::Printf(TEXT("TileCapsule%d"), i));

		UCapsuleComponent* newCapsuleComponent = NewObject<UCapsuleComponent>(this, UCapsuleComponent::StaticClass(), capsuleName);
		newCapsuleComponent->RegisterComponent();
		newCapsuleComponent->SetCapsuleSize(radius, height);
		newCapsuleComponent->AttachToComponent(newTileComponent, FAttachmentTransformRules::KeepRelativeTransform);
		newCapsuleComponent->SetRelativeTransform(newerCompTransform);
		newCapsuleComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2); // set object type to BuildCollision
		newCapsuleComponent->SetGenerateOverlapEvents(true);

		i++;
	}
}

