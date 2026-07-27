// Copyright Rancorous Games, 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AgentQueryableInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UAgentQueryableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors or objects to expose semantic state to the automation agent.
 */
class RANCUTILITIES_API IAgentQueryableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Returns a JSON string of custom key-value pairs representing the actor's state.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Agent")
	FString GetAgentProperties();
};
