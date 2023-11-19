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
};
