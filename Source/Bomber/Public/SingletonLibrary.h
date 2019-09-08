// Copyright 2019 Yevhenii Selivanov.

#pragma once

#include "Bomber.h"
#include "Cell.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SingletonLibrary.generated.h"

/**
 * 	The static functions library  
 */
UCLASS(Blueprintable, BlueprintType)
class BOMBER_API USingletonLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateAI);
	/** Calls to update movements of each AI controller  */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "C++")
	FUpdateAI OnAIUpdatedDelegate;

	/** Sets default values for this actor's properties */
	USingletonLibrary();

	/* ---------------------------------------------------
	 *		Editor development functions
	 * --------------------------------------------------- */

#if WITH_EDITOR  // [Editor]
	DECLARE_MULTICAST_DELEGATE(FPushNongeneratedToMap);
	/* Owners Map Components binds to updating on the Level Map to this delegate */
	FPushNongeneratedToMap OnActorsUpdatedDelegate;
#endif  //WITH_EDITOR [IsEditorNotPieWorld]

	/** Calls before generation preview actors to updating of all dragged to the Level Map actors*/
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (DevelopmentOnly))
	static void BroadcastActorsUpdating();

	/** Checks, that this actor placed in the editor world and the game is not started yet */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++", meta = (DevelopmentOnly))
	static bool IsEditorNotPieWorld();

	/** Blueprint debug function, that prints messages to the log */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (DevelopmentOnly, AutoCreateRefTerm = "FunctionName,Message", WorldContext = "UObj", CallableWithoutWorldContext))
	static void PrintToLog(const UObject* UObj, const FString& FunctionName, const FString& Message = "");

	/** Remove all text renders of the Owner */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (DevelopmentOnly, DefaultToSelf = "Owner"))
	static void ClearOwnerTextRenders(class AActor* Owner);

	/** Debug visualization by text renders
	 * @warning Has blueprint implementation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure = false, Category = "C++", meta = (DevelopmentOnly, AdvancedDisplay = 2, AutoCreateRefTerm = "TextColor,RenderText,CoordinatePosition", DefaultToSelf = "Owner"))
	void AddDebugTextRenders(
		class AActor* Owner,
		const TSet<struct FCell>& Cells,
		const struct FLinearColor& TextColor,
		bool& bOutHasCoordinateRenders,
		TArray<class UTextRenderComponent*>& OutTextRenderComponents,
		float TextHeight,
		float TextSize,
		const FText& RenderText,
		const FVector& CoordinatePosition) const;

#if WITH_EDITOR  // AddDebugTextRenders
	/** Shortest static overloading of debugging visualization without outer params */
	static void AddDebugTextRenders(
		class AActor* Owner,
		const FCells& Cells,
		const struct FLinearColor& TextColor = FLinearColor::Black,
		float TextHeight = 261.0f,
		float TextSize = 124.0f,
		const FString& RenderString = "",
		const FVector& CoordinatePosition = FVector::ZeroVector);
#endif  //WITH_EDITOR AddDebugTextRenders

	/* ---------------------------------------------------
	 *		Static library functions
	 * --------------------------------------------------- */

	/** Returns the singleton, nullptr otherwise */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	static USingletonLibrary* GetSingleton();

	/** The Level Map getter, nullptr otherwise */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	static class AGeneratedMap* GetLevelMap();

	/** The Level Map setter. If the specified Level Map is not valid or is transient, find and set another one
	 * 
	 * @param LevelMap The level map to set in the Library
	 */
	UFUNCTION(BlueprintCallable, Category = "C++")
	static void SetLevelMap(const class AGeneratedMap* LevelMap);

	/** Contains a data of standalone and PIE games, nullptr otherwise */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++", meta = (WorldContext = "WorldContextObject"))
	static class UMyGameInstance* GetMyGameInstance(const UObject* WorldContextObject);

	/* ---------------------------------------------------
	 *		FCell blueprint functions
	 * --------------------------------------------------- */

	/** Returns the length of the one cell (a floor bound) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	static FORCEINLINE float GetCellSize()
	{
		return FCell::CellSize;
	}

	/** Returns the zero cell (0,0,0) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	static FORCEINLINE FCell GetZeroCell()
	{
		return FCell::ZeroCell;
	}

	/** The custom make node of the FCell constructor.
	 * Used as a blueprint implementation of the default MakeStruct node
	 * 
	 * @param MapComponent Finding the closest cell to the Map Component's owner
	 * @return The found cell
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++", meta = (CompactNodeTitle = "MakeCell"))
	static FORCEINLINE struct FCell MakeCell(const class UMapComponent* MapComponent)
	{
		return FCell(MapComponent);
	}

	/** Rotation of the input vector around the center of the Level Map to the same yaw degree
	 * 
	 * @param Cell The cell, that will be rotated
	 * @param AxisZ The Z param of the axis to rotate around
	 * @return Rotated to the Level Map cell
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++", meta = (AutoCreateRefTerm = "VectorToRotate,AxisZ"))
	static FORCEINLINE struct FCell RotateCellAngleAxis(const FCell& Cell, const float& AxisZ)
	{
		return Cell.RotateAngleAxis(AxisZ);
	}

	/** Calculate the length between two cells
	 *
	 * @param C1 The first cell
	 * @param C2 The other cell
	 * @return The distance between to cells
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	static FORCEINLINE float CalculateCellsLength(const struct FCell& C1, const struct FCell& C2)
	{
		return fabsf((C1.Location - C2.Location).Size()) / GetCellSize();
	}

	/** Find the average of an set of cells */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++")
	static FCell GetCellArrayAverage(const TSet<struct FCell>& Cells);

	/* ---------------------------------------------------
	*		EActorType bitmask functions
	* --------------------------------------------------- */

	/** Bitwise and(&) operation with bitmasks of actors types.
	 * Checks the actors types among each other between themselves */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++", meta = (AutoCreateRefTerm = "LBitmask,RBitmask", CompactNodeTitle = "&"))
	static FORCEINLINE bool BitwiseActorTypes(
		UPARAM(meta = (Bitmask, BitmaskEnum = EActorType)) const int32& LBitmask,
		UPARAM(meta = (Bitmask, BitmaskEnum = EActorType)) const int32& RBitmask)
	{
		return (LBitmask & RBitmask) != 0;
	}

	/** Finding the actor type associated with the class */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++", meta = (AutoCreateRefTerm = "ActorClass"))
	static FORCEINLINE EActorType GetActorTypeByClass(const TSubclassOf<AActor>& ActorClass)
	{
		const EActorType* FoundActorType = GetSingleton()->ActorTypesByClasses_.FindKey(ActorClass);
		return FoundActorType ? *FoundActorType : EActorType::None;
	}

	/** Finding the class associated with the actor type */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "C++", meta = (AutoCreateRefTerm = "ActorType"))
	static FORCEINLINE TSubclassOf<AActor> GetClassByActorType(const EActorType& ActorType)
	{
		const TSubclassOf<AActor>* ActorClass = GetSingleton()->ActorTypesByClasses_.Find(ActorType);
		return ActorClass ? *ActorClass : nullptr;
	}

protected:
	/* ---------------------------------------------------
	*		Protected properties
	* --------------------------------------------------- */

	/** The reference to the AGeneratedMap actor. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected))
	TSoftObjectPtr<class AGeneratedMap> LevelMap_;  //[B]

	/** Type and its class as associated pair. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "C++", meta = (BlueprintProtected))
	TMap<EActorType, TSubclassOf<AActor>> ActorTypesByClasses_;  //[B]
};
