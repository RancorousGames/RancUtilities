#include "TimelineObject.h"

constexpr float S = 1.70158f;

UTimelineObject* UTimelineObject::Create(UObject* WorldContextObject, float InDuration, ETimelineObjectCurveType InCurveType)
{
	UTimelineObject* NewTimeline = NewObject<UTimelineObject>(WorldContextObject);
	NewTimeline->AnimationLength = InDuration;
	NewTimeline->CurveType = InCurveType;
	return NewTimeline;
}

void UTimelineObject::Tick()
{
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();

	CurrentAlpha = TickAnimation(DeltaSeconds);

	BroadcastOnUpdated(CurrentAlpha);

	/* If still playing then Set Tick */
	if (bIsPlaying) GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UTimelineObject::Tick);
}

void UTimelineObject::Play()
{
	bIsFinished = false;
	bIsReverse = false;
	bIsPlaying = true;

	BeginTick();
}

void UTimelineObject::PlayFromStart()
{
	bIsFinished = false;
	bIsReverse = false;
	bIsPlaying = true;

	TimePassed = 0.f;

	BeginTick();
}

void UTimelineObject::Reverse()
{
	bIsFinished = false;
	bIsReverse = true;
	bIsPlaying = true;

	BeginTick();
}

void UTimelineObject::ReverseFromEnd()
{
	bIsFinished = false;
	bIsReverse = true;
	bIsPlaying = true;

	TimePassed = AnimationLength;

	BeginTick();
}

void UTimelineObject::OnTimelineFinished()
{
	bIsPlaying = false;
	bIsFinished = true;

	if (bIsReverse) TimePassed = 0.f;
	else TimePassed = AnimationLength;

	// Call Finished Timeline Delegate.
	BroadcastOnFinished();
}

UWorld* UTimelineObject::GetWorld() const
{
	return GetOuter()->GetWorld();
}

void UTimelineObject::ApplyCurve(float& Alpha) const
{
	if (Curve)
	{
		Alpha = Curve->GetFloatValue(Alpha);
		return;
	}

	switch (CurveType)
	{
	case ETimelineObjectCurveType::TIME_Ease:
			
		Alpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, CurveExponent);
		return;

	case ETimelineObjectCurveType::TIME_Exponential:
		Alpha = FMath::InterpExpoInOut(0.f, 1.f, Alpha);
		return;

	case ETimelineObjectCurveType::TIME_Circular:
		Alpha = FMath::InterpCircularInOut(0.f, 1.f, Alpha);
		return;

	/* Remap a sine wave to act as a curve */
	case ETimelineObjectCurveType::TIME_Sine:
		Alpha = FMath::Sin(PI * (Alpha*AnimationLength - 0.5f)) / 2.f + 0.5f;
		return;

	case ETimelineObjectCurveType::TIME_ElasticIn:
		Alpha = -FMath::Pow(2, 10 * (Alpha - 1)) * FMath::Sin((Alpha - 1.1f) * 5 * PI);
		return;

	case ETimelineObjectCurveType::TIME_ElasticOut:
		Alpha = FMath::Pow(2, -10 * Alpha) * FMath::Sin((Alpha - 0.1f) * 5 * PI) + 1.f;
		return;
	case ETimelineObjectCurveType::TIME_BackIn:
		Alpha = Alpha * Alpha * ((S + 1) * Alpha - S);
		return;

	case ETimelineObjectCurveType::TIME_BackOut:
		Alpha -= 1;
		Alpha = Alpha * Alpha * ((S + 1) * Alpha + S) + 1;
		return;
	case ETimelineObjectCurveType::TIME_BounceOut:
		if (Alpha < (1 / 2.75f)) {
			Alpha = 7.5625f * Alpha * Alpha;
		} else if (Alpha < (2 / 2.75f)) {
			Alpha -= (1.5f / 2.75f);
			Alpha = 7.5625f * Alpha * Alpha + 0.75f;
		} else if (Alpha < (2.5f / 2.75f)) {
			Alpha -= (2.25f / 2.75f);
			Alpha = 7.5625f * Alpha * Alpha + 0.9375f;
		} else {
			Alpha -= (2.625f / 2.75f);
			Alpha = 7.5625f * Alpha * Alpha + 0.984375f;
		}
		return;

	/* If Linear then don't apply any curve */
	case ETimelineObjectCurveType::TIME_Linear:
		return;
	}
}

void UTimelineObject::BroadcastOnUpdated(float In) const
{
	BP_OnUpdatedDelegate.Broadcast(In);
}

void UTimelineObject::BroadcastOnFinished() const
{
	BP_OnFinishedDelegate.Broadcast();
}

UTimelineObject* UTimelineObject::CreateTimeline(UObject* WorldContextObject)
{
	return NewObject<UTimelineObject>(WorldContextObject);
}

void UTimelineObject::SetDuration(const float InDuration)
{
	AnimationLength = InDuration;
}

void UTimelineObject::SetCurveType(const ETimelineObjectCurveType InCurveType)
{
	CurveType = InCurveType;
}

void UTimelineObject::SetCurveAsset(UCurveFloat* InCurve)
{
	Curve = InCurve;
}

void UTimelineObject::SetCurveExponent(const float InExponent)
{
	CurveExponent = InExponent;
}

float UTimelineObject::GetDuration() const
{
	return AnimationLength;
}

ETimelineObjectCurveType UTimelineObject::GetCurveType() const
{
	return CurveType;
}

UCurveFloat* UTimelineObject::GetCurveAsset() const
{
	return Curve;
}

float UTimelineObject::GetCurveExponent() const
{
	return CurveExponent;
}

bool UTimelineObject::IsPlaying() const
{
	return bIsPlaying;
}

bool UTimelineObject::IsReverse() const
{
	return bIsReverse;
}

void UTimelineObject::BeginTick()
{
	const float Alpha = TimePassed / AnimationLength;

	BroadcastOnUpdated(Alpha);

	/* If still playing then Set Tick */
	if (bIsPlaying) GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UTimelineObject::Tick);
}

float UTimelineObject::TickAnimation(const float DeltaSeconds)
{
	if (!bIsPlaying) return TimePassed / AnimationLength;

	if (bIsReverse) TimePassed = -TimePassed;
	else TimePassed += DeltaSeconds;

	if (bIsReverse)
	{
		if (TimePassed > 0)
		{
			float Alpha = TimePassed / AnimationLength;
			ApplyCurve(Alpha);
			return Alpha;
		}
	}
	else
	{
		if (TimePassed < AnimationLength)
		{
			float Alpha = TimePassed / AnimationLength;
			ApplyCurve(Alpha);
			return Alpha;
		}
	}

	OnTimelineFinished();

	return bIsReverse ? 0 : 1;
}
