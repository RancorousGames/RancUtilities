#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ISortableElement.generated.h"

UINTERFACE(BlueprintType)
class RANCUTILITIES_API USortableElement : public UInterface
{
	GENERATED_BODY()
};

class RANCUTILITIES_API ISortableElement
{
	GENERATED_BODY()

public:
	// Function to compare this object with another sortable object
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Sorting")
	bool IsLessThan(const UObject* Other) const;
};