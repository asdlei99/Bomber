// Fill out your copyright notice in the Description page of Project Settings.

#include "GeneratedMap.h"

#include "Components/StaticMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"

#include "Bomber.h"
#include "Cell.h"
#include "MyCharacter.h"
#include "MyGameInstance.h"
#include "SingletonLibrary.h"

/* ---------------------------------------------------
 *					Level map public functions
 * --------------------------------------------------- */

// Sets default values
AGeneratedMap::AGeneratedMap()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITOR  //[Editor]
	// Should not call OnConstruction on drag events
	bRunConstructionScriptOnDrag = false;
#endif  //WITH_EDITOR [Editor]

	// Initialize the Root Component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent->SetRelativeScale3D(FVector(5.f, 5.f, 1.f));

	// Find blueprint class of the background
	BackgroundBlueprintComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("BackgroundBlueprintComponent"));
	BackgroundBlueprintComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FClassFinder<AActor> BackgroundClassFinder(TEXT("/Game/Bomber/Assets/BackgroundBlueprintAsset"));
	if (BackgroundClassFinder.Succeeded())
	{
		BackgroundBlueprintClass = BackgroundClassFinder.Class;  // Default class of the PlatformComponent
	}
}

TSet<FCell> AGeneratedMap::IntersectionCellsByTypes_Implementation(
	const TSet<FCell>& Cells,
	const int32& ActorsTypesBitmask,
	const AMyCharacter* ExcludePlayer) const
{
	return Cells;
}

TSet<FCell> AGeneratedMap::GetSidesCells_Implementation(
	const FCell& Cell,
	const int32& SideLength,
	EPathTypesEnum Pathfinder) const
{
	TSet<FCell> FoundedLocations;
	return FoundedLocations;
}

void AGeneratedMap::AddActorToGridArray(const FCell& Cell, AActor* UpdateActor)
{
	if (IS_VALID(UpdateActor) == false			// Updating actor is not valid
		|| GridArray_.Contains(Cell) == false)  // Not existing cell
	{
		return;
	}

	// Add actor to specific array
	AMyCharacter* const UpdateCharacter = Cast<AMyCharacter>(UpdateActor);
	if (UpdateCharacter != nullptr)  // if it is character, add to array of characters
	{
		if (CharactersOnMap.Contains(UpdateCharacter) == false)
		{
			CharactersOnMap.Add(UpdateCharacter);  // Add this character
			USingletonLibrary::PrintToLog(UpdateActor, "AddActorToGridArray: \t Summary characters:", FString::FromInt(CharactersOnMap.Num()));
		}
	}
	else  // else if this class can be added
		if (USingletonLibrary::IsActorInTypes(UpdateActor, TO_FLAG(EActorTypeEnum::All)))
	{
		RemoveActorFromGridArray(UpdateActor);
		GridArray_.Add(Cell, UpdateActor);  // Add this actor to his new cell
	}

	UpdateActor->GetRootComponent()->SetAbsolute(false, false, true);

	// Locate actor on cell
	FRotator ActorRotation{GetActorRotation()};
	ActorRotation.Yaw += FMath::RandRange(int32(1), int32(4)) * 90.f;
	const FVector ActorLocation{Cell.Location.X, Cell.Location.Y, Cell.Location.Z + 100.f};
	const FVector Scale{1.f, 1.f, 1.f};
	UpdateActor->SetActorTransform(FTransform(ActorRotation, ActorLocation, Scale));

	// Attach to the Level Map actor
	UpdateActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	USingletonLibrary::PrintToLog(UpdateActor, "AddActorToGridArray \t ADDED:", Cell.Location.ToString());
}

void AGeneratedMap::RemoveActorFromGridArray(const AActor* Actor)
{
	const FCell* Cell = GridArray_.FindKey(Actor);
	if (Cell == nullptr)  // The actor was not found on anyone cell
	{
		return;
	}

	GridArray_.Add(*Cell);  // remove this actor from this cell
	USingletonLibrary::PrintToLog(this, "RemoveActorFromGridArray", Actor->GetName());
}

void AGeneratedMap::DestroyActorsFromMap_Implementation(const TSet<FCell>& Keys)
{
	USingletonLibrary::PrintToLog(this, "DestroyActorsFromMap \t Keys will be destroyed:", FString::FromInt(Keys.Num()));
}

/* ---------------------------------------------------
 *					Level map protected functions
 * --------------------------------------------------- */

void AGeneratedMap::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (IS_TRANSIENT(this) == true)  // the level map is transient
	{
		return;
	}
	USingletonLibrary::PrintToLog(this, "----- OnConstruction -----", "");

#if WITH_EDITOR  // [Editor]
	USingletonLibrary::SetLevelMap(this);
#endif  // WITH_EDITOR [Editor]

	// Create the background blueprint child actor
	if (BackgroundBlueprintClass									 // There is some background class
		&& BackgroundBlueprintComponent								 // Is accessible
		&& !IsValid(BackgroundBlueprintComponent->GetChildActor()))  // Is not created yet
	{
		BackgroundBlueprintComponent->SetChildActorClass(BackgroundBlueprintClass);
		BackgroundBlueprintComponent->CreateChildActor();
	}

	// Align the Transform
	SetActorRotation(FRotator(0.f, GetActorRotation().Yaw, 0.f));
	SetActorLocation(GetActorLocation().GridSnap(USingletonLibrary::GetGridSize()));
	FIntVector MapScale(GetActorScale3D());
	if (MapScale.X % 2 != 1)  // Length must be unpaired
	{
		MapScale.X += 1;
	}
	if (MapScale.Y % 2 != 1)  // Weight must be unpaired
	{
		MapScale.Y += 1;
	}
	MapScale.Z = 1;  //Height must be 1
	SetActorScale3D(FVector(MapScale));

	// Clear old grid array
	GridArray_.Empty();

	// Loopy cell-filling of the grid array
	for (int32 Y = 0; Y < MapScale.Y; ++Y)
	{
		for (int32 X = 0; X < MapScale.X; ++X)
		{
			FVector FoundVector(X, Y, 0.f);
			// Calculate a length of iteration cell
			FoundVector *= USingletonLibrary::GetGridSize();
			// Locate the cell relative to the Level Map
			FoundVector += GetActorLocation();
			// Subtract the deviation from the center
			FoundVector -= GetActorScale3D() / 2 * USingletonLibrary::GetGridSize();
			// Snap to the cell
			FoundVector = FoundVector.GridSnap(USingletonLibrary::GetGridSize());
			// Cell was found, add rotated cell to the array
			const FCell FoundCell = USingletonLibrary::CalculateVectorAsRotatedCell(FoundVector, 1.f);
			GridArray_.Add(FoundCell);
		}
	}

#if WITH_EDITOR									   // [IsEditorNotPieWorld] Map's text renders
	if (USingletonLibrary::IsEditorNotPieWorld())  // For editor only
	{
		// Show cell coordinated of the Grid array
		USingletonLibrary::ClearOwnerTextRenders(this);
		if (bShouldShowRenders == true)
		{
			TArray<FCell> ArrayRenders;
			GridArray_.GetKeys(ArrayRenders);
			const TSet<FCell> SetRenders(ArrayRenders);
			USingletonLibrary::AddDebugTextRenders(this, SetRenders);
		}

		// Preview generation
		GenerateLevelActors();
	}
#endif  // WITH_EDITOR [Editor]
}

void AGeneratedMap::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (IS_TRANSIENT(this) == true)  // the level map is transient
	{
		return;
	}

	// Update the gameplay LevelMap reference in the singleton library;
	USingletonLibrary::SetLevelMap(this);

	// Cells regeneration if the level map's size was changed
	UMyGameInstance* MyGameInstance = USingletonLibrary::GetMyGameInstance(this);
	if (MyGameInstance)
	{
		const FVector MapScale = MyGameInstance->LevelMapScale;
		if (MapScale.IsZero() == false  // is not zero
			&& MapScale != GetActorScale3D())
		{
			SetActorScale3D(MapScale);
			RerunConstructionScripts();
		}
	}

	// Actors generation
	GenerateLevelActors();
}

void AGeneratedMap::GenerateLevelActors_Implementation()
{
	if (GridArray_.Num() == 0)  // Is no cell to generate an actor
	{
		return;
	}
	USingletonLibrary::PrintToLog(this, "----- GenerateLevelActors ------", "---- START -----");

	// fix null keys before characters regeneration
	if (CharactersOnMap.Num() > 0)
	{
		for (int32 i = CharactersOnMap.Num() - 1; i >= 0; --i)
		{
			if (IS_VALID(CharactersOnMap[i]) == false)
			{
				CharactersOnMap.RemoveAt(i);
			}
		}
	}

#if WITH_EDITOR  // [Editor]
	//  Destroy editor-only actors that were spawned in the PIE
	USingletonLibrary::PrintToLog(this, "GenerateLevelActors", "-> [Editor]DestroyAttachedActors");
	DestroyAttachedActors(true);

	// After destroying PIE actors and before their generation,
	// calling to updating of all dragged to the Level Map actors
	USingletonLibrary::BroadcastActorsUpdating();  // [IsEditorNotPieWorld]
	USingletonLibrary::PrintToLog(this, "_____ [Editor]BroadcastActorsUpdating _____", "_____ END _____");

#endif  // [Editor]

	int32 SpawnedCharactersN = 0;				   // The numbers of spawned characters in the loop
	TArray<FCell> CellsArray;					   // Access to keys of the dictionary by ...
	GridArray_.GetKeys(CellsArray);				   // ... by indexes
	const FIntVector MapScale(GetActorScale3D());  // Iterating by sizes (strings and columns)
	for (int32 Y = 0; Y < MapScale.Y; ++Y)
	{
		for (int32 X = 0; X < MapScale.X; ++X)
		{
			const FCell CellIt = CellsArray[MapScale.X * Y + X];
			USingletonLibrary::PrintToLog(this, "GenerateLevelActors \t Iterated cell:", CellIt.Location.ToString());
			const AActor** ActorIt = GridArray_.Find(CellIt);
			if (ActorIt != nullptr				// Was found actor
				&& IS_VALID(*ActorIt) == true)  // and this actor is valid
			{
				USingletonLibrary::PrintToLog(this, "GenerateLevelActors \t The actor on the cell has already existed", (*ActorIt)->GetName());
				continue;
			}

			// --- The spawn part ---

			// In case all next conditions will be false
			EActorTypeEnum ActorTypeToSpawn = EActorTypeEnum::None;

			// Wall condition
			if (X % 2 == 1 && Y % 2 == 1)
			{
				USingletonLibrary::PrintToLog(this, "GenerateLevelActors", "WALL will be spawned");
				ActorTypeToSpawn = EActorTypeEnum::Wall;
			}

			// Player condition
			const bool bIsCornerX = (X == 0 || X == MapScale.X - 1);
			const bool bIsCornerY = (Y == 0 || Y == MapScale.Y - 1);
			if (bIsCornerX && bIsCornerY  //
				&& SpawnedCharactersN < CharactersNumber)
			{
				USingletonLibrary::PrintToLog(this, "GenerateLevelActors", "PLAYER will be spawned");
				ActorTypeToSpawn = EActorTypeEnum::Player;
				SpawnedCharactersN++;
			}

			// Box condition
			if (ActorTypeToSpawn == EActorTypeEnum::None				  // all previous conditions are false
				&& FMath::RandRange(int32(0), int32(99)) < BoxesChance_   // Chance of boxes
				&& (!bIsCornerX && X != 1 && X != MapScale.X - 2		  // X corner zone
					   || !bIsCornerY && Y != 1 && Y != MapScale.Y - 2))  // Y corner zone
			{
				USingletonLibrary::PrintToLog(this, "GenerateLevelActors", "BOX will be spawned");
				ActorTypeToSpawn = EActorTypeEnum::Box;
			}

			// --- The spawn part ---
			const auto ActorClass = USingletonLibrary::FindClassByActorType(EActorTypeEnum(ActorTypeToSpawn));
			if (ActorClass != nullptr)  // There is type to spawn
			{
				AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClass, CellIt.Location, FRotator::ZeroRotator);

#if WITH_EDITOR												  // [IsEditorNotPieWorld]
				if (USingletonLibrary::IsEditorNotPieWorld()  // PIE only
					&& SpawnedActor != nullptr)				  // Successfully spawn
				{
					// If PIE world, mark this spawned actor as bIsEditorOnlyActor
					SpawnedActor->bIsEditorOnlyActor = true;
					USingletonLibrary::GetSingleton()->OnActorsUpdatedDelegate.RemoveAll(SpawnedActor);
				}
#endif		   // WITH_EDITOR [IsEditorNotPieWorld]
			}  // End of the spawn part
		}	  // X iterations
	}		   // Y iterations

	USingletonLibrary::PrintToLog(this, "_____ GenerateLevelActors _____", "_____ END _____");
}

/* ---------------------------------------------------
 *					Editor development
 * --------------------------------------------------- */

void AGeneratedMap::DestroyAttachedActors(bool bIsEditorOnly)
{
#if WITH_EDITOR  // [Editor]
	if (IS_TRANSIENT(this))
	{
		return;
	}

	USingletonLibrary::PrintToLog(this, "----- [Editor]DestroyAttachedActors -----", "----- START -----");
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	if (AttachedActors.Num() > 0)
	{
		for (int32 i = AttachedActors.Num() - 1; i >= 0; --i)
		{
			if (bIsEditorOnly == false				  // All attached actors
				|| AttachedActors[i]->IsEditorOnly()  // Should destroy editor only actors
			)

				AttachedActors[i]->Destroy();
		}
		USingletonLibrary::PrintToLog(this, "_____ [Editor]DestroyAttachedActors _____", "_____ END _____");
	}
#endif  //WITH_EDITOR [Editor]
}

#if WITH_EDITOR  // Destroyed() [Editor]
void AGeneratedMap::Destroyed()
{
	DestroyAttachedActors();
	Super::Destroyed();
}
#endif  // Destroyed() [Editor]