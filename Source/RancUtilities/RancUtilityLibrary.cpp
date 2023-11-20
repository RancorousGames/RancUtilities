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

	TTuple<float, float>& ThrottleLogTuple = ThrottleLogMap.FindOrAdd(Key);
	float& LastLogTime = ThrottleLogTuple.Get<0>();
	float& ThrottlePeriodRef = ThrottleLogTuple.Get<1>();

	const float CurrentTime = FPlatformTime::Seconds();
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
