// Inspired by https://forums.unrealengine.com/t/destroy-component-does-not-work/136428/5

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RancUtilityLibrary.generated.h"

class UActorComponent;

UENUM(BlueprintType)
enum class ETrueFalse : uint8
{
	IsTrue UMETA(DisplayName = "True"),
	IsFalse UMETA(DisplayName = "False")
};

UENUM(BlueprintType)
enum class EBoolState : uint8
{
	WasTrue UMETA(DisplayName = "Was True"),
	WasFalse UMETA(DisplayName = "Was False")
};

UCLASS()
class RANCUTILITIES_API URancUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Destroys the specified component from its owner actor.
	 * @param Component - The component to be destroyed. */
	UFUNCTION(BlueprintCallable, Category = "Component")
	static void ForceDestroyComponent(UActorComponent* Component);

	/* 
	 * Logs a message but throttles it so it is only logged every desired period even if called more often.
	 * Useful for reducing spam in the log.
	 * @param Message - The message to be logged.
	 * @param ThrottlePeriod - The minimum time interval between consecutive logs of this message.
	 * @param Key - A unique key to identify this log message. Different keys allow for independent throttling.
	 */
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void ThrottledLog(const FString& Message, float ThrottlePeriod = 1.0f,
	                         const FString& Key = "DefaultThrottleLogKey");

	/*
	 * Calculates a location in front of the actor by a specified distance.
	 */
	UFUNCTION(BlueprintPure, Category="Actor")
	static FVector GetLocationInFrontOfActor(AActor* Actor, float Distance);

	/* Toggles the state of a boolean variable and returns the previous state as an enum. */
	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (ExpandEnumAsExecs = "Branches"))
	static void ToggleBool(UPARAM(ref) bool& BoolToToggle, EBoolState& Branches);

	/* Increments an integer variable without requiring a set */
	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (CompactNodeTitle = "++"))
	static void IncrementInt(UPARAM(ref) int32& Value, const int32 MaxValue);

	/* Decrements an integer variable without requiring a set */
	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (CompactNodeTitle = "--"))
	static void DecrementInt(UPARAM(ref) int32& Value, const int32 MinValue);

	/* * Adds to one vector the scaled value of another vector. */
	UFUNCTION(BlueprintPure, Category = "Math")
	static FVector AddScaledVector(FVector VectorA, FVector VectorB, float ScaleFactor);

	UFUNCTION(BlueprintCallable, meta = (CompactNodeTitle = "And", DisplayName = "Branch And", ExpandEnumAsExecs = "Branches", CommutativeAssociativeBinaryOperator = "true", Category="Math|Boolean"))
	static void BranchAnd(const bool A, const bool B, ETrueFalse& Branches);

private:
	// map that contains the last time a message was logged and the throttle period
	inline static TMap<FString, TTuple<float, float>> ThrottleLogMap = {};
};
