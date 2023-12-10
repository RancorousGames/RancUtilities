// Copyright Rancorous Games, 2023

#include "RancSortingLibrary.h"

#include "ISortableElement.h"

void URancSortingLibrary::SortSortableArray(TArray<UObject*>& ArrayToSort)
{
	ArrayToSort.Sort([](const UObject& A, const UObject& B) {
		if (const ISortableElement* SortableA = Cast<ISortableElement>(&A))
		{
			return SortableA->IsLessThan(&B);
		}
		return false;
	});
}

TArray<UObject*> URancSortingLibrary::GetSortedArrayCopy(const TArray<UObject*>& ArrayToSort)
{
	TArray<UObject*> SortedArray = ArrayToSort;
	SortedArray.Sort([](const UObject& A, const UObject& B) {
		if (const ISortableElement* SortableA = Cast<ISortableElement>(&A))
		{
			return SortableA->IsLessThan(&B);
		}
		return false;
	});
	return SortedArray;
}


TArray<UObject*> URancSortingLibrary::GetSortedArrayCopyWithDelegate(const TArray<UObject*>& ArrayToSort, const FCompareDelegate& ComparisonFunction)
{
    TArray<UObject*> SortedArray = ArrayToSort;
    SortedArray.Sort([&](const UObject& A, const UObject& B) {
        return ComparisonFunction.Execute(&A, &B);
    });
    return SortedArray;
}