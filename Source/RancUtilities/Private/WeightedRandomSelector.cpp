// Copyright Rancorous Games, 2024

#include "WeightedRandomSelector.h"

UObject* UWeightedRandomSelector::SelectRandomWeightedItem(const TArray<FSWeightedItem>& Items)
{
	float TotalWeight = 0.f;
	for (const FSWeightedItem& Item : Items)
	{
		TotalWeight += Item.Weight;
	}

	float RandomWeight = FMath::RandRange(0.f, TotalWeight);
	float CurrentWeight = 0.f;

	for (const FSWeightedItem& Item : Items)
	{
		CurrentWeight += Item.Weight;
		if (CurrentWeight >= RandomWeight)
		{
			return Item.Item;
		}
	}

	return nullptr; // Return null if no item is selected (e.g., if Items is empty or all weights are zero)
}


TScriptInterface<IWeightedItem> UWeightedRandomSelector::SelectRandomIWeightedItem(
	TArray<TScriptInterface<IWeightedItem>> Items)
{
	float TotalWeight = 0.f;
	for (auto Item : Items)
	{
		UObject* obj = Item.GetObject();
		IWeightedItem* WeightedItem = (IWeightedItem*)obj;
		if (!obj || !WeightedItem)
		{
			continue;
		}

		Item.SetInterface(WeightedItem);
		TotalWeight += Item->Execute_GetWeight(obj);
	}
	const float RandomWeight = FMath::RandRange(0.f, TotalWeight);
	float CurrentWeight = 0.f;

	for (const TScriptInterface<IWeightedItem> Item : Items)
	{
		CurrentWeight += Item->Execute_GetWeight(Item.GetObject());
		if (CurrentWeight >= RandomWeight)
		{
			return Item;
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
		return 0;
	}
	
	float TotalWeight = 0.f;
	for (float Weight : Weights)
	{
		TotalWeight += Weight;
	}

	const float RandomWeight = FMath::RandRange(0.f, TotalWeight);
	float CurrentWeight = 0.f;

	if (TotalWeight == 0.f)
	{
		return FMath::RandRange(0, Weights.Num() - 1);
	}
	
	for (int32 i = 0; i < Weights.Num(); ++i)
	{
		CurrentWeight += Weights[i];
		if (CurrentWeight >= RandomWeight)
		{
			return i;
		}
	}

	return INDEX_NONE; // Return -1 (INDEX_NONE) if no index is selected (e.g., if Weights is empty or all weights are zero)
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
