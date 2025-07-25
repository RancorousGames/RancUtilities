﻿// Copyright Rancorous Games, 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "WeightedRandomSelector.generated.h"

// Delegate to get the weight of an item at a specified index
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FGetWeightDelegate, int, ItemIndex);

USTRUCT(BlueprintType)
struct FSWeightedItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeightedRandomSelector")
	UObject* Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeightedRandomSelector")
	float Weight = 1.0f;
};

// Interface for items that have a weight associated with them
UINTERFACE(BlueprintType)
class RANCUTILITIES_API UWeightedItem : public UInterface
{
    GENERATED_BODY()
};

class RANCUTILITIES_API IWeightedItem
{
    GENERATED_BODY()

public:
    // Function to get the weight of the item
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeightedRandomSelector")
    float GetWeight() const;
};

UCLASS()
class RANCUTILITIES_API UWeightedRandomSelector  : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	// Selects a random item from an array of weighted items.
	// @param Items - Array of FSWeightedItem containing the items and their respective weights.
	// @return UObject* - The selected UObject item.
	UFUNCTION(BlueprintCallable, Category = "WeightedRandomSelector")
	static UObject* SelectRandomWeightedItem(const TArray<FSWeightedItem>& Items);

	// Selects a random item from an array of IWeightedItem interface objects.
	// @param Items - Array of IWeightedItem interface pointers.
	// @return IWeightedItem* - The selected IWeightedItem interface pointer.
	UFUNCTION(BlueprintCallable, Category = "WeightedRandomSelector")
	static TScriptInterface<IWeightedItem> SelectRandomIWeightedItem(const TArray<TScriptInterface<IWeightedItem>>& Items);
	
	// Selects a random index from an array using a delegate to get weights.
	// @param Items - Array of UObject items.
	// @param GetWeight - Delegate to provide the weight for each item by index.
	// @return int - The index of the selected item.
	UFUNCTION(BlueprintCallable, Category = "WeightedRandomSelector")
	static int SelectRandomItemIndex(const TArray<UObject*>& Items, const FGetWeightDelegate& GetWeight);

	// Selects a random index from an array of weights.
	// @param Weights - Array of floats representing the weights.
	// If the array is empty, the function will return index 0.
	// If the array is just 0s then we will have an even distribution
	// @return int - The index of the selected weight.
	UFUNCTION(BlueprintCallable, Category = "WeightedRandomSelector")
	static int SelectRandomWeightedIndex(const TArray<float>& Weights);

	// Gets a random dice roll based on the number of dice and sides.
	// @param DiceCount - The number of dice to roll.
	// @param DiceSides - The number of sides on each die.
	// @param bDiceHas0 - Whether the dice can roll a 0.
	// @return int - The result of the dice roll.
	UFUNCTION(BlueprintCallable, Category = "WeightedRandomSelector")
	static int RollDice(int DiceCount, int DiceSides, bool bDiceHas0 = false);
	
};