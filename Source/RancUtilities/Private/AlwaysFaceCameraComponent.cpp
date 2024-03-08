#include "AlwaysFaceCameraComponent.h"

#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"


UAlwaysFaceCameraComponent::UAlwaysFaceCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	PrimaryComponentTick.bCanEverTick = true;
}

void UAlwaysFaceCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Ensure we have a valid reference to the player camera
	if(const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		AActor* Owner = GetOwner();
		if(!Owner) return;

		FVector CameraLocation;
		FRotator CameraRotation;
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector Direction = CameraLocation - Owner->GetActorLocation();
		//Direction.Z = 0; // Keep the rotation in the horizontal plane
		FRotator NewRotation = Direction.Rotation();

		Owner->SetActorRotation(NewRotation);
	}
}