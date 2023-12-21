#include "TPriorityQueue.h"

FPriorityQueueNode::FPriorityQueueNode() : DataInteger(0), Cost(0.0f) {}

FPriorityQueueNode::FPriorityQueueNode(int InDataInteger, float InPriority)
	: DataInteger(InDataInteger), Cost(InPriority) {}

bool FPriorityQueueNode::operator<(const FPriorityQueueNode& Other) const
{
	return Cost < Other.Cost;
}

UPriorityQueue::UPriorityQueue()
{
	Array.Heapify();
}

void UPriorityQueue::SetInitialCapacity(int InitialCapacity)
{
	ContainsSet.Reserve(InitialCapacity);
	Array.Reserve(InitialCapacity);
}

void UPriorityQueue::Clear()
{
	Array.Empty();
	ContainsSet.Empty();
}

int UPriorityQueue::Pop(UPARAM(ref) bool& Success)
{
	if (ContainsSet.IsEmpty())
	{
		Success = false;
		return -80085;
	}
	
	FPriorityQueueNode Node;
	Array.HeapPop(Node);
	ContainsSet.Remove(Node.DataInteger);

	Success = true;
	return Node.DataInteger;
}

void UPriorityQueue::Push(int DataInteger, float Cost)
{
	ContainsSet.Add(DataInteger);
	Array.HeapPush(FPriorityQueueNode(DataInteger, Cost));
}

void UPriorityQueue::Remove(int DataInteger)
{
	for (int i = 0; i < Array.Num(); i++)
	{
		if (Array[i].DataInteger == DataInteger)
		{
			Array.HeapRemoveAt(i);
			ContainsSet.Remove(DataInteger);
			break;
		}
	}
}

void UPriorityQueue::Replace(int DataInteger, float NewCost)
{
	for (int i = 0; i < Array.Num(); i++)
	{
		if (Array[i].DataInteger == DataInteger)
		{
			Array[i].Cost = NewCost;
			break;
		}
	}

	Array.HeapSort();
}

bool UPriorityQueue::Contains(int DataInteger)
{
	return ContainsSet.Contains(DataInteger);
}

bool UPriorityQueue::IsEmpty() const
{
	return Array.Num() == 0;
}