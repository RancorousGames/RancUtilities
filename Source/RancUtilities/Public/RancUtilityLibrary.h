// Inspired by https://forums.unrealengine.com/t/destroy-component-does-not-work/136428/5

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RancUtilityLibrary.generated.h"

class UActorComponent;

UCLASS()
class RANCUTILITIES_API URancUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Component")
	static void ForceDestroyComponent(UActorComponent* Component);

	/* Log a message but throttle it so it is only logged every desired period even if called more often */
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void ThrottledLog(const FString& Message, float ThrottlePeriod = 1.0f, const FString& Key = "DefaultThrottleLogKey");

private:

	// map that contains the last time a message was logged and the throttle period
	inline static TMap<FString, TTuple<float, float>> ThrottleLogMap = {};
	
};
