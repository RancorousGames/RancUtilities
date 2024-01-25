#include "MovementUtilityComponent.h"

#include "MovementUtilityComponent.h"

UMovementUtilityComponent::UMovementUtilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMovementUtilityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMovementUtilityComponent::SmoothRotateToYaw(float NewTargetYaw, float NewRotationSpeed)
{
	TargetYaw = NewTargetYaw;
	RotationSpeed = NewRotationSpeed;

	// Start rotation immediately on the next tick
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RotationUpdate, this, &UMovementUtilityComponent::UpdateRotation, GetWorld()->GetDeltaSeconds(), true);
}

void UMovementUtilityComponent::UpdateRotation()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		FinishRotation();
		return;
	}

	// Get the delta time from the world
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	// Get current rotation as a quaternion
	FQuat CurrentQuat = Owner->GetActorRotation().Quaternion();
	// Create target rotation as a quaternion
	FQuat TargetQuat = FQuat(FRotator(0.f, TargetYaw, 0.f));

	// Perform spherical interpolation between the current and target rotations
	FQuat NewQuat = FQuat::Slerp(CurrentQuat, TargetQuat, RotationSpeed * DeltaTime);

	// Ensure the quaternion is normalized (SLERP can sometimes return a non-normalized quaternion)
	NewQuat.Normalize();

	// Set the new rotation based on the interpolated quaternion
	Owner->SetActorRotation(NewQuat);

	// Check if the target yaw has been reached within a small error margin
	if (NewQuat.Equals(TargetQuat, 0.01f))
	{
		FinishRotation();
	}
}

void UMovementUtilityComponent::FinishRotation()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RotationUpdate);
}
