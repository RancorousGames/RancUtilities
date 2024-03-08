#pragma once
// Header file: AlwaysFaceCameraComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AlwaysFaceCameraComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RANCUTILITIES_API UAlwaysFaceCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	// Sets default values for this component's properties
	UAlwaysFaceCameraComponent();

public:    
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
