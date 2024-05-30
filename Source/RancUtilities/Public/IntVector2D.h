#pragma once

#include "CoreMinimal.h"
#include "IntVector2D.generated.h"

USTRUCT(BlueprintType)
struct FIntVector2D
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector")
	int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vector")
	int32 Y;

	// Constructors
	FIntVector2D() : X(0), Y(0) {}
	FIntVector2D(int32 InX, int32 InY) : X(InX), Y(InY) {}

	// Operator overloads
	FIntVector2D operator+(const FIntVector2D& Other) const
	{
		return FIntVector2D(X + Other.X, Y + Other.Y);
	}

	FIntVector2D operator-(const FIntVector2D& Other) const
	{
		return FIntVector2D(X - Other.X, Y - Other.Y);
	}

	FIntVector2D& operator+=(const FIntVector2D& Other)
	{
		X += Other.X;
		Y += Other.Y;
		return *this;
	}

	FIntVector2D& operator-=(const FIntVector2D& Other)
	{
		X -= Other.X;
		Y -= Other.Y;
		return *this;
	}

	bool operator==(const FIntVector2D& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	bool operator!=(const FIntVector2D& Other) const
	{
		return !(*this == Other);
	}

	// Utility functions
	float GetMagnitude() const
	{
		return FMath::Sqrt(static_cast<float>(X) * X + Y * Y);
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("X=%d, Y=%d"), X, Y);
	}
};
