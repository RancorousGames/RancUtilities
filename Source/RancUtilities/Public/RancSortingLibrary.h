#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RancSortingLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FCompareDelegate, const UObject*, ElementA, const UObject*, ElementB);

UCLASS()
class RANCUTILITIES_API URancSortingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Sorting")
	static void SortSortableArray(UPARAM(ref) TArray<UObject*>& ArrayToSort);

	UFUNCTION(BlueprintCallable, Category = "Sorting", BlueprintPure)
	static TArray<UObject*> GetSortedArrayCopy(const TArray<UObject*>& ArrayToSort);

	UFUNCTION(BlueprintCallable, Category = "Sorting", BlueprintPure)
	static TArray<UObject*> GetSortedArrayCopyWithDelegate(const TArray<UObject*>& ArrayToSort, const FCompareDelegate& ComparisonFunction);
};