// Thanks to SovereignDev for the original implementation

// How the K2Node should look: https://i.imgur.com/NDmBWEs.png

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TimelineObject.generated.h"

UENUM(BlueprintType)
enum class ETimelineObjectCurveType : uint8
{
	TIME_Linear UMETA(DisplayName = "Linear"),
	TIME_Ease UMETA(DisplayName = "Ease"),
	TIME_Exponential UMETA(DisplayName = "Exponential"),
	TIME_Circular UMETA(DisplayName = "Circular"),
	TIME_Sine UMETA(DisplayName = "Sine") ,
	TIME_ElasticIn	UMETA(DisplayName = "Elastic In"),
	TIME_ElasticOut	UMETA(DisplayName = "Elastic Out") ,
	TIME_BackIn UMETA(DisplayName = "Back In"),
	TIME_BackOut UMETA(DisplayName = "Back Out"),
	TIME_BounceOut UMETA(DisplayName = "Bounce Out")
};

/*
	UTimelineObject is a utility class that provides timeline functionality for animating values over time.
	It allows you to create and control a timeline with various curve types and durations.
	The timeline can be played, reversed, and stopped, and it provides delegates for updating and finishing events.
	See example C++ usage at the bottom of this header but it is also usable by Blueprints.
*/
UCLASS()
class RANCUTILITIES_API UTimelineObject : public UObject
{
	GENERATED_BODY()

public:
	/**
	* Creates a new instance of UTimelineObject.
	* @param WorldContextObject The world context object.
	* @param InDuration The duration of the timeline in seconds.
	* @param InCurveType The type of curve to use for the timeline.
	* @return The created UTimelineObject instance.
	*/
	static UTimelineObject* Create(UObject* WorldContextObject, float InDuration = 1.0f, ETimelineObjectCurveType InCurveType = ETimelineObjectCurveType::TIME_Ease);

	// Blueprint version
	UFUNCTION(BlueprintCallable, Category = "Timeline", meta = (WorldContext = "WorldContextObject"))
	static UTimelineObject* CreateTimeline(UObject* WorldContextObject);
	
	/**
	 * Ticks the timeline every frame.
	 * This function is called automatically by the timer manager.
	 */
	UFUNCTION()
	void Tick();

	/**
	 * Begins the timeline tick process.
	 * This function sets up the initial tick and broadcasts the first update event.
	 */
	void BeginTick();

	/**
	 * Ticks the timeline animation and returns the current alpha value.
	 * @param DeltaSeconds The time delta since the last tick.
	 * @return The current alpha value of the timeline.
	 */
	float TickAnimation(const float DeltaSeconds);

	/**
	 * Plays the timeline from the current position.
	 */
	UFUNCTION(BlueprintCallable, Category="Timeline")
	void Play();

	/**
	 * Plays the timeline from the start.
	 */
	UFUNCTION(BlueprintCallable, Category="Timeline")
	void PlayFromStart();
	UFUNCTION(BlueprintCallable, Category="Timeline")

	/**
	 * Reverses the timeline from the current position.
	 */
	void Reverse();
	UFUNCTION(BlueprintCallable, Category="Timeline")

	void ReverseFromEnd();

	void OnTimelineFinished();

	virtual UWorld* GetWorld() const override;;

	/**
	 * Timeline Data
	 **/

	// The amount of time passed. During the Animation
	float TimePassed = 0;
	// Length on the Animation in Seconds.
	float AnimationLength = 1.f;

	float CurveExponent = 2.f;

	bool bIsPlaying = false;
	bool bIsReverse = false;
	bool bIsFinished = false;
	// The Position of the Timeline. 0 is the start, 1 is the end.
	float CurrentAlpha = 0.f;
	FORCEINLINE bool IsAtStart() const { return CurrentAlpha <= 0.f; }
	FORCEINLINE bool IsAtEnd() const { return CurrentAlpha >= 1.f; }

	/**
	 * Curves
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Curve")
	ETimelineObjectCurveType CurveType = ETimelineObjectCurveType::TIME_Ease; // Unchanged in assets

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Curve", meta=(ExposeOnSpawn="true"))
	UCurveFloat* Curve = nullptr; // Unchanged in assets

private:
	/**
	* Applies the selected curve type or custom curve to the given alpha value.
	*/
	void ApplyCurve(float& Alpha) const;

public:
	/* Delegates */

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBPTimelineObjectUpdatedDelegate, float, Alpha);

	UPROPERTY(BlueprintAssignable)
	FBPTimelineObjectUpdatedDelegate BP_OnUpdatedDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBPTimelineObjectFinishedDelegate);

	UPROPERTY(BlueprintAssignable)
	FBPTimelineObjectFinishedDelegate BP_OnFinishedDelegate;


	// Call this function once the system has fully initialized
	void BroadcastOnUpdated(float In) const;

	// Call this function once the system has fully initialized
	void BroadcastOnFinished() const;

	/**
	 * BP Internal Functionality
	 */

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void SetDuration(const float InDuration);

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void SetCurveType(const ETimelineObjectCurveType InCurveType);

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void SetCurveAsset(UCurveFloat* InCurve);

	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void SetCurveExponent(const float InExponent);

	UFUNCTION(BlueprintPure, Category = "Timeline")
	float GetDuration() const;

	UFUNCTION(BlueprintPure, Category = "Timeline")
	ETimelineObjectCurveType GetCurveType() const;

	UFUNCTION(BlueprintPure, Category = "Timeline")
	UCurveFloat* GetCurveAsset() const;

	UFUNCTION(BlueprintPure, Category = "Timeline")
	float GetCurveExponent() const;

	UFUNCTION(BlueprintPure, Category = "Timeline")
	bool IsPlaying() const;

	UFUNCTION(BlueprintPure, Category = "Timeline")
	bool IsReverse() const;
};

/****** EXAMPLE USAGE ******

void AMyActor::BlendLocation()
{
	if (!TimelineObject)
	{
		TimelineObject = UTimelineObject::Create(this, 1.0f, ETimelineObjectCurveType::TIME_Linear);
		TimelineObject->BP_OnUpdatedDelegate.AddDynamic(this, &AMyActor::TickBlendLocation);
	}

	TimelineObject->Play();
}

void AMyActor::TickBlendLocation(float Alpha)
{
	FVector Location1;
	FVector Location2;

	FVector Location = FMath::Lerp(Location1, Location2, Alpha);
	// Do something with the blended location
}

void AMyActor::ReverseTimeline()
{
	if (TimelineObject)
	{
		TimelineObject->Reverse();
	}
}
*/
