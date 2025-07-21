// Copyright Rancorous Games, 2024

#include "WeightedRandomSelector.h"

UObject* UWeightedRandomSelector::SelectRandomWeightedItem(const TArray<FSWeightedItem>& Items)
{
	float TotalWeight = 0.f;
	for (const FSWeightedItem& Item : Items)
	{
		TotalWeight += Item.Weight;
	}

	if (TotalWeight <= 0.f)
	{
		if (Items.IsEmpty())
		{
			return nullptr;
		}
		// All weights are 0, return a random item with equal probability
		return Items[FMath::RandRange(0, Items.Num() - 1)].Item;
	}

	const float RandomWeight = FMath::RandRange(0.f, TotalWeight);
	float CurrentWeight = 0.f;

	for (const FSWeightedItem& Item : Items)
	{
		CurrentWeight += Item.Weight;
		if (CurrentWeight >= RandomWeight)
		{
			return Item.Item;
		}
	}

	return nullptr; // Return null if no item is selected (e.g., if Items is empty)
}


TScriptInterface<IWeightedItem> UWeightedRandomSelector::SelectRandomIWeightedItem(
	const TArray<TScriptInterface<IWeightedItem>>& Items)
{
	float TotalWeight = 0.f;
	// First loop: calculate total weight, iterating by const reference
	for (const TScriptInterface<IWeightedItem>& Item : Items)
	{
		if (Item) // Check if the interface is valid
		{
			TotalWeight += IWeightedItem::Execute_GetWeight(Item.GetObject());
		}
	}

	// If total weight is 0, all items have an equal chance (or return null if empty)
	if (TotalWeight <= 0.f)
	{
		if (Items.IsEmpty())
		{
			return nullptr;
		}
		// Pick a random one with equal probability
		return Items[FMath::RandRange(0, Items.Num() - 1)];
	}

	const float RandomWeight = FMath::RandRange(0.f, TotalWeight);
	float CurrentWeight = 0.f;

	// Second loop: find the selected item, iterating by const reference
	for (const TScriptInterface<IWeightedItem>& Item : Items) // This is the fix for the compiler error
	{
		if (Item)
		{
			CurrentWeight += IWeightedItem::Execute_GetWeight(Item.GetObject());
			if (CurrentWeight >= RandomWeight)
			{
				return Item;
			}
		}
	}

	return nullptr;
}

int UWeightedRandomSelector::SelectRandomItemIndex(const TArray<UObject*>& Items, const FGetWeightDelegate& GetWeight)
{
	float TotalWeight = 0.f;
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		TotalWeight += GetWeight.Execute(i);
	}

	if (TotalWeight <= 0.f)
	{
		if (Items.IsEmpty())
		{
			return INDEX_NONE;
		}
		return FMath::RandRange(0, Items.Num() - 1);
	}

	const float RandomWeight = FMath::RandRange(0.f, TotalWeight);
	float CurrentWeight = 0.f;

	for (int32 i = 0; i < Items.Num(); ++i)
	{
		CurrentWeight += GetWeight.Execute(i);
		if (CurrentWeight >= RandomWeight)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

int UWeightedRandomSelector::SelectRandomWeightedIndex(const TArray<float>& Weights)
{
	if (Weights.Num() == 0)
	{
		return INDEX_NONE; 
	}
	
	float TotalWeight = 0.f;
	for (float Weight : Weights)
	{
		TotalWeight += Weight;
	}

	// If all weights are zero, pick one with uniform probability
	if (TotalWeight <= 0.f)
	{
		return FMath::RandRange(0, Weights.Num() - 1);
	}
	
	const float RandomWeight = FMath::RandRange(0.f, TotalWeight);
	float CurrentWeight = 0.f;
	
	for (int32 i = 0; i < Weights.Num(); ++i)
	{
		CurrentWeight += Weights[i];
		if (CurrentWeight >= RandomWeight)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

int UWeightedRandomSelector::RollDice(int DiceCount, int DiceSides, bool bDiceHas0)
{
	// roll the dice
	int32 DiceResult = 0;
	for (int32 i = 0; i < DiceCount; ++i)
	{
		DiceResult += FMath::RandRange(bDiceHas0 ? 0 : 1, DiceSides);
	}

	return DiceResult;
}