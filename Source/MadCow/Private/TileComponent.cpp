// Fill out your copyright notice in the Description page of Project Settings.


#include "TileComponent.h"
#include "Engine/StaticMeshActor.h" 


UTileComponent::UTileComponent()
{
	this->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	this->SetGenerateOverlapEvents(true);
}

void UTileComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	for (auto a : spawnedDetail) {
		if (IsValid(a)) {
			a->Destroy();
		}
	}
	spawnedDetail.Empty();
}

void UTileComponent::SetIsOccupied(bool val)
{
	this->isOccupied = val;
}

bool UTileComponent::GetIsOccupied()
{
	return this->isOccupied;
}

void UTileComponent::SetTileHeight(float newHeight)
{
	tileHeight = newHeight;
}

void UTileComponent::ApplyBiome()
{
	if (materialsMap.Num() == 0) {
		SetMaterial(0, GetStaticMesh()->GetMaterial(0));
	}
	else {
		SetMaterial(0, GetBiomeMaterial(tileHeight));
	}
	
	if (detailsMap.Num() > 0) {
		if (!this->isPrimaryTile) {
			// get surface socket transform
			FVector socketPos = GetSocketLocation(FName("Surface_Socket"));

			UStaticMesh* meshToSpawn = GetBiomeDetail(tileHeight);
			FTransform detailTransform = FTransform(FRotator(), socketPos, detailMeshScale);
			this->spawnedDetail = TSet<AActor*>();

			AStaticMeshActor* spawnedActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), detailTransform);
			spawnedActor->GetStaticMeshComponent()->SetStaticMesh(meshToSpawn);
			this->spawnedDetail.Add(spawnedActor);
		}
	}
}

bool UTileComponent::ShouldSpawnOnCoordinates(int64 heightCoordinate, int64 widthCoordinate)
{
	if (!isPrimaryTile && FMath::FRandRange(0.0, 100.0) > spawnRate) return false;

	return (heightCoordinate % this->heightDivisor == 0) && 
		((heightCoordinate % (2 * this->heightDivisor) == 0) ? 
			(widthCoordinate % this->widthDivisor == 0) : 
			(widthCoordinate % this->widthDivisor == this->evenHeightOffset));
}

/*
Assigns material reference to specific tile given its height value
*/
UMaterialInterface* UTileComponent::GetBiomeMaterial(float heightMapValue) {
	
	TSet<UMaterial*> materialKeys;
	materialsMap.GetKeys(materialKeys);

	TArray<UMaterial*> materials = TArray<UMaterial*>();


	for (auto mk : materialKeys) {
		if (!IsValid(mk)) continue;

		FFloatRange* range = materialsMap.Find(mk);
	
		if (range->Contains(heightMapValue)) {
			materials.Add(mk);
		}
	}

	if (materials.Num() > 0) {
		return materials[FMath::Rand() % materials.Num()];
	}
	else {
		return GetStaticMesh()->GetMaterial(0);
	}

}

UStaticMesh* UTileComponent::GetBiomeDetail(float heightMapValue) {
	TSet<UStaticMesh*> detailKeys;
	detailsMap.GetKeys(detailKeys);

	TArray<UStaticMesh*> details = TArray<UStaticMesh*>();

	for (auto mk : detailKeys) {
		if (!IsValid(mk)) continue;

		FFloatRange* range = detailsMap.Find(mk);

		if (range->Contains(heightMapValue)) {
			details.Add(mk);
		}
	}

	if (details.Num() > 0) {
		return details[FMath::Rand() % details.Num()];
	}
	else {
		return nullptr;
	}
}