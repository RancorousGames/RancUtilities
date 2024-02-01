// Copyright Rancorous Games, 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementUtilityComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RANCUTILITIES_API UMovementUtilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	// Sets default values for this component's properties
	UMovementUtilityComponent();

	// Function to start a smooth rotation to a target yaw
	UFUNCTION(BlueprintCallable, Category="Movement|Rotation")
	void SmoothRotateToYaw(float TargetYaw, float RotationSpeed = 90.f);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Update rotation every frame
	void UpdateRotation();

	// Complete the rotation
	void FinishRotation();

	// Target yaw rotation
	float TargetYaw;

	// Rotation speed in degrees per second
	float RotationSpeed;

	// Timer handle for rotation update
	FTimerHandle TimerHandle_RotationUpdate;
};