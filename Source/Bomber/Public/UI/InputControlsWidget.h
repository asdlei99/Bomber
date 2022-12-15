// Copyright (c) Yevhenii Selivanov

#pragma once

#include "UI/SettingSubWidget.h"
//---
#include "EnhancedActionKeyMapping.h"
#include "InputCoreTypes.h"
//---
#include "InputControlsWidget.generated.h"

/* Widgets hierarchy:
 *
 * ╔UInputControlsWidget
 * ╚════╦UInputCategoryWidget
 *		╚════UInputButtonWidget
 */

/**
 * Input Key Selector wrapper, represents each input button to remap.
 */
UCLASS()
class BOMBER_API UInputButtonWidget final : public USettingSubWidget
{
	GENERATED_BODY()

public:
	/** Sets the style of the button and its text. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (AutoCreateRefTerm = "TextStyle,ButtonStyle"))
	void SetInputKeySelectorStyle(const FTextBlockStyle& TextStyle, const FButtonStyle& ButtonStyle);

	/** Sets this button to let player remap input specified in mappable data. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void InitButton(const FEnhancedActionKeyMapping& InMappableData, const UMyInputMappingContext* InInputMappingContext);

	/** Returns last selected key for the current input selector. */
	UFUNCTION(BlueprintPure, Category = "C++")
	const FORCEINLINE FKey& GetCurrentKey() const { return MappableDataInternal.Key; }

	/** Sets specified key for the current input key selector. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void SetCurrentKey(const FKey& NewKey);

protected:
	/** ---------------------------------------------------
	 *		Protected properties
	 * --------------------------------------------------- */

	/** A widget for remapping a single key. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UInputKeySelector> InputKeySelector = nullptr;

	/** Contains mappable data for this button. */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Mappable Data"))
	FEnhancedActionKeyMapping MappableDataInternal;

	/** An input context that is owns this input button. */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Input Context"))
	TObjectPtr<const class UMyInputMappingContext> InputContextInternal = nullptr;

	/** ---------------------------------------------------
	 *		Protected functions
	 * --------------------------------------------------- */

	/** Called after the underlying slate widget is constructed. */
	virtual void NativeConstruct() override;

	/** Sets the style for the Input Key Selector. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void UpdateStyle();

	/** Called whenever a new key is selected by the user. */
	UFUNCTION()
	void OnKeySelected(FInputChord SelectedKey);

	/** Called whenever the key selection mode starts or stops. */
	UFUNCTION()
	void OnIsSelectingKeyChanged();
};

/**
 * Contains inputs for along own input context.
 */
UCLASS()
class BOMBER_API UInputCategoryWidget final : public USettingSubWidget
{
	GENERATED_BODY()

public:
	/** Sets the input context to be represented by this widget. */
	UFUNCTION(BlueprintCallable, Category = "C++")
	void CreateInputButtons(const class UMyInputMappingContext* InInputMappingContext);

protected:
	/** ---------------------------------------------------
	 *		Protected properties
	 * --------------------------------------------------- */

	/** Is parent widget of all dynamically created buttons. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UHorizontalBox> HorizontalBoxInputButtons = nullptr;

	/** The class of the Input Button Widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Input Button Class"))
	TSubclassOf<UInputButtonWidget> InputButtonClassInternal = UInputButtonWidget::StaticClass();

	/** Owned input context that is represented by this widget. */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Input Context"))
	TObjectPtr<const class UMyInputMappingContext> InputContextInternal = nullptr;

	/** All dynamically created input button for each mappable input in own Input Context. */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Input Buttons"))
	TArray<TObjectPtr<UInputButtonWidget>> InputButtonsInternal;

	/** ---------------------------------------------------
	 *		Protected functions
	 * --------------------------------------------------- */

	/** Called after the underlying slate widget is constructed. */
	virtual void NativeConstruct() override;

	/** Adds all input buttons to the root of this widget. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void AttachInputButtons();
};

/**
 * Allows player to rebind input mappings.
 */
UCLASS()
class BOMBER_API UInputControlsWidget final : public USettingCustomWidget
{
	GENERATED_BODY()

protected:
	/** ---------------------------------------------------
	 *		Protected properties
	 * --------------------------------------------------- */

	/** Is parent widget of all dynamically created categories. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<class UScrollBox> ScrollBoxInputCategories = nullptr;

	/** The class of the Input Button Widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "C++", meta = (BlueprintProtected, DisplayName = "Input Category Class"))
	TSubclassOf<UInputCategoryWidget> InputCategoryClassInternal = UInputCategoryWidget::StaticClass();

	/** Contains all dynamically created categories, where every category represents own mapping context. */
	UPROPERTY(BlueprintReadWrite, Transient, Category = "C++", meta = (BlueprintProtected, DisplayName = "Input Categories"))
	TArray<TObjectPtr<UInputCategoryWidget>> InputCategoriesInternal;

	/** ---------------------------------------------------
	 *		Protected functions
	 * --------------------------------------------------- */

	/**
	 * Called after the underlying slate widget is constructed.
	 * May be called multiple times due to adding and removing from the hierarchy.
	 */
	virtual void NativeConstruct() override;

	/** Adds input categories for each mapping context. */
	UFUNCTION(BlueprintCallable, Category = "C++", meta = (BlueprintProtected))
	void CreateInputCategories();
};
