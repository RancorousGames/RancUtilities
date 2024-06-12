// Copyright Rancorous Games, 2024

#include "RancUtilityLibrary.h"

#include "AlwaysFaceCameraComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Font.h"

void URancUtilityLibrary::ShouldNotHappen(FString Message)
{
	ensureMsgf(false, TEXT("%s"), *Message);
}

void URancUtilityLibrary::ForceDestroyComponent(UActorComponent* Component)
{
	if (Component && Component->IsValidLowLevel())
	{
		Component->DestroyComponent();
	}
}

void URancUtilityLibrary::ThrottledLog(const FString& Message, float ThrottlePeriod,
                                       const FString& Key)
{
	if (ThrottlePeriod <= 0.0f)
	{
		UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
		return;
	}

	TTuple<double, float>& ThrottleLogTuple = ThrottleLogMap.FindOrAdd(Key);
	double& LastLogTime = ThrottleLogTuple.Get<0>();
	float& ThrottlePeriodRef = ThrottleLogTuple.Get<1>();

	const double CurrentTime = FPlatformTime::Seconds();
	if (CurrentTime - LastLogTime > ThrottlePeriodRef)
	{
		LastLogTime = CurrentTime;
		ThrottlePeriodRef = ThrottlePeriod;
		UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
	}
	else
	{
		ThrottlePeriodRef -= CurrentTime - LastLogTime;
	}
}

void URancUtilityLibrary::ThrottledAction(float ThrottlePeriod, EThrottleActionState& Branches, const FString& Key)
{
	double& LastActionTime = ThrottleMap.FindOrAdd(Key);
	const double CurrentTime = FPlatformTime::Seconds();

	if (CurrentTime - LastActionTime > ThrottlePeriod)
	{
		LastActionTime = CurrentTime;
		Branches = EThrottleActionState::Ready;
	}
	else
	{
		Branches = EThrottleActionState::Throttled;
	}
}

FVector URancUtilityLibrary::GetLocationInFrontOfActor(AActor* Actor, float Distance)
{
	if (!Actor)
	{
		return FVector::ZeroVector;
	}

	// Get the actor's forward vector
	const FVector ForwardVector = Actor->GetActorForwardVector();

	// Scale the vector by the desired distance
	const FVector ScaledVector = ForwardVector * Distance;

	// Get the actor's current location
	const FVector ActorLocation = Actor->GetActorLocation();

	// Calculate the new location in front of the actor
	const FVector NewLocation = ActorLocation + ScaledVector;

	return NewLocation;
}

void URancUtilityLibrary::ToggleBool(bool& BoolToToggle, EBoolState& Branches)
{
	BoolToToggle = !BoolToToggle;

	Branches = BoolToToggle ? EBoolState::WasFalse : EBoolState::WasTrue;
}

void URancUtilityLibrary::IncrementInt(int32& Value, const int32 MaxValue)
{
	++Value;
}

void URancUtilityLibrary::DecrementInt(int32& Value, const int32 MinValue)
{
	--Value;
}

FVector URancUtilityLibrary::AddScaledVector(FVector VectorA, FVector VectorB, float ScaleFactor)
{
	return VectorA + (VectorB * ScaleFactor);
}

FVector URancUtilityLibrary::GetRandomWorldPlaneUnitVector()
{
	// Generate a random angle in radians
	const float RandAngle = FMath::RandRange(0.0f, 2.0f * PI);

	// Calculate the x and y components using cosine and sine
	float X = FMath::Cos(RandAngle);
	float Y = FMath::Sin(RandAngle);

	// Return the unit vector
	return FVector(X, Y, 0.0f);
}

FVector URancUtilityLibrary::GetPointOnCircleAroundTarget(const FVector& SourcePosition, const FVector& TargetPosition,
	float Radius, float AngleDegrees)
{
	FVector Direction = (TargetPosition - SourcePosition).GetSafeNormal();
	FVector RotatedDirection = Direction.RotateAngleAxis(AngleDegrees + 180, FVector::UpVector);
	return TargetPosition + Radius * RotatedDirection;
}

void URancUtilityLibrary::CreateFloatingText(const UObject* WorldContextObject, const FString& Text, const FVector Location, const FRotator Rotation, const FLinearColor Color, const double Scale, const double LifeTime, UFont* Font, bool AlwaysFaceCamera)
{
	if (!WorldContextObject) return;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return;

	// Create a new Actor
	AActor* TextActor = World->SpawnActor<AActor>(AActor::StaticClass(), Location, Rotation);
	if (!TextActor) return;

	// Create and set as the root component a SceneComponent
	USceneComponent* RootComponent = NewObject<USceneComponent>(TextActor);
	if (!RootComponent) return;
	RootComponent->SetMobility(EComponentMobility::Movable);
	RootComponent->RegisterComponent();
	TextActor->SetRootComponent(RootComponent);

	// Set the root component's transform to the desired location, rotation, and scale
	RootComponent->SetWorldLocationAndRotation(Location, Rotation);

	// Now attach your TextRenderComponent to this properly positioned root component
	UTextRenderComponent* TextComp = NewObject<UTextRenderComponent>(TextActor);
	if (!TextComp) return;
	TextComp->RegisterComponent();
	TextComp->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	// Setup the TextRenderComponent properties
	TextComp->SetText(FText::FromString(Text));
	TextComp->SetTextRenderColor(Color.ToFColor(true));
	TextComp->SetWorldSize(Scale*25);
	TextComp->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextComp->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	
	if (Font)
		TextComp->SetFont(Font);

	// Set the lifetime of the text
	if (LifeTime > 0.00001)
	{
		TextActor->SetLifeSpan(LifeTime);
	}

	if (AlwaysFaceCamera)
	{
		UAlwaysFaceCameraComponent* AlwaysFaceCameraComponent = NewObject<UAlwaysFaceCameraComponent>(TextActor);
		if (!AlwaysFaceCameraComponent) return;
		AlwaysFaceCameraComponent->RegisterComponent();
	}
}

FGameplayTag URancUtilityLibrary::StringToGameplayTag(FName TagName)
{
	return UGameplayTagsManager::Get().RequestGameplayTag(TagName, false);
}