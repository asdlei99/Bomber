// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GeneratedMap.h"
#include "Components/ActorComponent.h"
#include "MapComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BOMBER_API UMapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMapComponent();

	// Callback function to the delegate
	UFUNCTION(BlueprintCallable, Category = "C++")
		void UpdateSelfOnMap();

	// Current location of Actor
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "C++")
		struct FCell cell;

	// Parent actor of component
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "C++")
		AActor* owner;

protected:
	/** Called when a component is created (not loaded). This can happen in the editor or during gameplay */
	virtual void OnComponentCreated() final;

	/**
	 * Called when a component is destroyed
	 *
	 * @param	bDestroyingHierarchy  - True if the entire component hierarchy is being torn down, allows avoiding expensive operations
	 */
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) final;

};