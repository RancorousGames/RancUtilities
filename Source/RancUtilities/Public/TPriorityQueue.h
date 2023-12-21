#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TPriorityQueue.generated.h"

USTRUCT(BlueprintType)
struct FPriorityQueueNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PriorityQueue")
	int DataInteger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PriorityQueue")
	float Cost;

	FPriorityQueueNode();
	FPriorityQueueNode(int DataInteger, float Priority);
	bool operator<(const FPriorityQueueNode& Other) const;
};

UCLASS(Blueprintable, EditInlineNew)
class UPriorityQueue : public UObject
{
	GENERATED_BODY()
    
public:
	UPriorityQueue();

	UFUNCTION(BlueprintCallable, Category = "PriorityQueue")
	void SetInitialCapacity(int InitialCapacity);
	
	UFUNCTION(BlueprintCallable, Category = "PriorityQueue")
	void Clear();

	UFUNCTION(BlueprintCallable, Category = "PriorityQueue")
	int Pop(bool& Success);

	UFUNCTION(BlueprintCallable, Category = "PriorityQueue")
	void Push(int DataInteger, float Cost);
	
	UFUNCTION(BlueprintCallable, Category = "PriorityQueue")
	void Remove(int DataInteger);

	UFUNCTION(BlueprintCallable, Category = "PriorityQueue")
	void Replace(int DataInteger, float NewCost);
	
	UFUNCTION(BlueprintCallable, Category = "PriorityQueue")
	bool Contains(int DataInteger);

	UFUNCTION(BlueprintCallable, Category = "PriorityQueue")
	bool IsEmpty() const;

private:
	TArray<FPriorityQueueNode> Array;
	TSet<int> ContainsSet;
};