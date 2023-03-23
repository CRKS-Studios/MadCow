// Fill out your copyright notice in the Description page of Project Settings.


#include "TileComponent.h"

void UTileComponent::setIsOccupied(bool val)
{
	this->isOccupied = val;
}

bool UTileComponent::getIsOccupied()
{
	return this->isOccupied;
}
