// Fill out your copyright notice in the Description page of Project Settings.


#include "SnapGridVolumeComponent.h"

void USnapGridVolumeComponent::BeginPlay()
{
	this->interLayerDistance = (std::sqrt(3) / 2) * interCenterDistance;

	this->XSize = interCenterDistance * this->numHexagonWidth;
	this->YSize = interLayerDistance * this->numHexagonHeight;
	this->ZLoc = this->GetUnscaledBoxExtent().Z / 2;

	this->SetBoxExtent(FVector(XSize, YSize, this->GetUnscaledBoxExtent().Z));
}

TArray<FVector> USnapGridVolumeComponent::getHexCentersCoords()
{
	TArray<FVector> toReturn = TArray<FVector>();
	
	for (int i = 0; i < numHexagonWidth; i++) {
		for (int j = 0; j < numHexagonHeight; j++) {
			if (j % 2 == 0) {
				toReturn.Add(FVector(i * this->interCenterDistance, j * interLayerDistance, ZLoc));
			}
			else {
				if (i == numHexagonWidth - 1) continue;

				toReturn.Add(FVector((i + 0.5) * this->interCenterDistance, j * interLayerDistance, ZLoc));
			}
		}
	}

	return toReturn;
}
