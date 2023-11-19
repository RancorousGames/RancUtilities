#pragma once


class RANCUTILITIES_API ARAIController : public AActor
{
	
};

/**
 * TArrayWrapper is a utility wrapper around Unreal's TArray.
 * 
 * Purpose:
 * - To provide a standardized way to store TArray within other data structures 
 *   like TMap, while still offering TArray-like functionality, which is otherwise not possible
 * 
 * Features:
 * - Num() to get the number of elements in the array.
 * - Operator[] to access elements at a specific index.
 * - Add() to append elements to the array.
 * - RemoveAt() to remove elements at a specific index.
 * 
 * Note:
 * This is not a UObject or USTRUCT, so it won't be accessible from Blueprints.
 * It's intended for C++ usage only.
 */
template <typename T>
struct TArrayWrapper
{
	TArray<T> Data;

	int32 Num() const
	{
		return Data.Num();
	}

	T& operator[](int32 Index)
	{
		return Data[Index];
	}

	const T& operator[](int32 Index) const
	{
		return Data[Index];
	}

	void Add(const T& Element)
	{
		Data.Add(Element);
	}

	void RemoveAt(int32 Index)
	{
		Data.RemoveAt(Index);
	}
	
	void Remove(T Element)
	{
		Data.Remove(Element);
	}
};
