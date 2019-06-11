// Fill out your copyright notice in the Description page of Project Settings.

#include "GeneratedMap.h"
#include "Bomber.h"


FCell::FCell(const AActor* actor)
{
	if (!ISVALID(actor) || !ISVALID(USingletonLibrary::GetLevelMap())) return;
	if (USingletonLibrary::GetLevelMap()->GeneratedMap_.Num() == 0) return;

	this->location = USingletonLibrary::GetLevelMap()->GetNearestCell(actor).location;

}


// Sets default values
AGeneratedMap::AGeneratedMap()
{
	// Shouldt call OnConsturction on drag events
	bRunConstructionScriptOnDrag = false;
}

TSet<FCell> AGeneratedMap::GetSidesCells_Implementation(const FCell& cell, int32 sideLength, EPathTypesEnum pathfinder) const
{

	TSet<FCell> foundedLocations;
	return foundedLocations;
}

TSet<FCell> AGeneratedMap::FilterCellsByTypes_Implementation(const TSet<FCell>& keys, const TArray<EActorTypeEnum>& filterTypes, const ACharacter* excludePlayer) const
{
	TSet<FCell> foundedLocations;
	return foundedLocations;
}

AActor* AGeneratedMap::AddActorOnMap_Implementation(const FCell& cell, EActorTypeEnum actorType)
{

	return nullptr;
}

void AGeneratedMap::AddActorOnMapByObj_Implementation(const FCell& cell, const AActor* updateActor)
{

}

void AGeneratedMap::DestroyActorFromMap_Implementation(const FCell& cell)
{

}

// Called when the game starts or when spawned
void AGeneratedMap::BeginPlay()
{
	Super::BeginPlay();
}

void AGeneratedMap::OnConstruction(const FTransform& Transform)
{
	//Regenerate map;
	GenerateLevelMap();
}

void AGeneratedMap::Destroyed()
{
	// Destroying attached actors 
	TArray<AActor*> attachedActors;
	GetAttachedActors(attachedActors);
	for (AActor* attachedActor : attachedActors)
	{
		attachedActor->Destroy();
	}

	Super::Destroyed();
}

void AGeneratedMap::GenerateLevelMap_Implementation()
{
	// Update LevelMap obj before generating child actors;
	if (!ISVALID(USingletonLibrary::GetSingleton())) return;
	if (!ISVALID(USingletonLibrary::GetLevelMap()))
	{
		USingletonLibrary::GetSingleton()->levelMap_ = this;
	}

	GeneratedMap_.Empty();
	charactersOnMap_.Empty();

}
