#include "RancUtilityLibrary.h"
#include "Components/ActorComponent.h"

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
