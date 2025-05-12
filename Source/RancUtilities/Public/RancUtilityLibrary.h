// Copyright Rancorous Games, 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RancUtilityLibrary.generated.h"

class UActorComponent;

UENUM(BlueprintType)
enum class EBoolState : uint8
{
	WasTrue UMETA(DisplayName = "Was True"),
	WasFalse UMETA(DisplayName = "Was False")
};

UENUM(BlueprintType)
enum class EThrottleActionState : uint8
{
	Ready UMETA(DisplayName = "Ready"),
	Throttled UMETA(DisplayName = "Throttled")
};

static AActor* DebugCubeActor = nullptr; // Global variable to store the debug cube actor

UCLASS()
class RANCUTILITIES_API URancUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/* An explicit "Do nothing" node for better blueprint node layout. */
	UFUNCTION(BlueprintCallable, Meta = (DevelopmentOnly, CompactNodeTitle = "Do Nothing"))
	static void DoNothing()
	{
	}

	/* A Should not happen node for assertion/debugging purposes. */
	UFUNCTION(BlueprintCallable,  Meta = (DevelopmentOnly, CompactNodeTitle = "Bad Event"))
	static void ShouldNotHappen(FString Message = "Should not Happen node ran!");


	/* * Checks if the current game is running on a dedicated server.
	 * @return True if the game is running on a dedicated server, false otherwise.
	 * Equivalent to existing IsDedicatedServer for actors but more generally available.
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utilities")
	static bool IsDedicatedServerWorldContext(const UObject* WorldContextObject);

	/* * Checks if the current game is running as a listen server.
	 * @return True if the game is running as a listen server, false otherwise. */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utilities")
	static bool IsListenServerWorldContext(const UObject* WorldContextObject);

	/* * Checks if the current game is running as a client.
	 * @return True if the game is running as a client, false otherwise.
	 * Note that standalone and listen server is not client */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utilities")
	static bool IsClientWorldContext(const UObject* WorldContextObject);

	/* Destroys the specified component from its owner actor.
	 * @param Component - The component to be destroyed. */
	UFUNCTION(BlueprintCallable, Category = "Component")
	static void ForceDestroyComponent(UActorComponent* Component);

	/* 
	 * Logs a message but throttles it so it is only logged every desired period even if called more often.
	 * Useful for reducing spam in the log.
	 * @param Message - The message to be logged.
	 * @param ThrottlePeriod - The minimum time interval between consecutive logs of this message.
	 * @param Key - A unique key to identify this log message. Different keys allow for independent throttling.
	 */
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void ThrottledLog(const FString& Message, float ThrottlePeriod = 1.0f,
	                         const FString& Key = "DefaultThrottleLogKey");

	/* 
	 * Executes an action but throttles it so it is only executed once per the specified period even if called more often.
	 * Useful for reducing frequency of operations that should not occur too frequently (like logging, network requests, etc.).
	 * @param ThrottlePeriod - The minimum time interval between consecutive executions of the action.
	 * @param Branches - Outputs the state of the throttle, either Ready (action can be executed) or Throttled (action should wait).
	 * @param Key - A unique key to identify this action. Different keys allow for independent throttling of different actions.
	 *              Default value is "DefaultKey" if not specified.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (ExpandEnumAsExecs = "Branches"))
	static void ThrottledAction(float ThrottlePeriod, EThrottleActionState& Branches,
	                            const FString& Key = "UniqueKeyHere");

	/* Calculates a location in front of the actor by a specified distance.	 */
	UFUNCTION(BlueprintPure, Category="Actor")
	static FVector GetLocationInFrontOfActor(AActor* Actor, float Distance);

	/* Calculates a location above the actor by a specified distance. */
	UFUNCTION(BlueprintPure, Category="Actor")
	static FVector GetLocationAboveActorOrigin(AActor* Actor, float Distance);

	/* Toggles the state of a boolean variable and returns the previous state as an enum. */
	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (ExpandEnumAsExecs = "Branches"))
	static void ToggleBool(UPARAM(ref) bool& BoolToToggle, EBoolState& Branches);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utility", meta = (CompactNodeTitle = "1-"))
	static float OneMinus(float Value);
	
	/* Increments an integer variable without requiring a set */
	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (CompactNodeTitle = "++"))
	static void IncrementInt(UPARAM(ref) int32& Value, const int32 MaxValue);

	/* Decrements an integer variable without requiring a set */
	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (CompactNodeTitle = "--"))
	static void DecrementInt(UPARAM(ref) int32& Value, const int32 MinValue);

	/* * Adds to one vector the scaled value of another vector. */
	UFUNCTION(BlueprintPure, Category = "Math")
	static FVector AddScaledVector(FVector VectorA, FVector VectorB, float ScaleFactor);

	/* Gets a random 3D unit vector but with Z = 0. */
	UFUNCTION(BlueprintPure, Category = "Math|Random")
	static FVector GetRandomWorldPlaneUnitVector();

	/* Given a trace and a height, find the intersection point between the trace and the plane at that height */
	UFUNCTION(BlueprintPure, Category = "Math|Random")
	static FVector GetIntersectionPointWithPlane(const FVector& StartPoint, const FVector& EndPoint, float PlaneZ);

	/* Given an actor and a target point, get the yaw between the actors forward vector towards the given point*/
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static float GetYawDifferenceActorFacingToPoint(AActor* Actor, FVector TargetPoint);
	
	/* Given two direction vectors in world space, calculate the yaw angle between them */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static float GetYawAngleDifference(FVector Dir1, FVector Dir2);
	
	/*
	 * Calculates a point on a circle at a specific angle from the source to the target.
	 * Useful for positioning around a central target point.
	 *
	 * @param SourcePosition The starting position for the angle calculation.
	 * @param TargetPosition The center of the circle for the calculation.
	 * @param Radius Radius of the circle from the target position.
	 * @param AngleDegrees Angle in degrees for the calculation (0 degrees is direct line from source to target).
	 * @return FVector The calculated point on the circle.
	 *
	 * Example:
	 * // Calculate a position 45 degrees around an enemy, 50 units away
	 * FVector PlayerPosition = FVector(100, 100, 0);
	 * FVector EnemyPosition = FVector(200, 200, 0);
	 * FVector NewPosition = GetPointOnCircleAroundTarget(PlayerPosition, EnemyPosition, 50.0f, 45.0f);
	 */
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static FVector GetPointOnCircleAroundTarget(const FVector& SourcePosition, const FVector& TargetPosition, float Radius, float AngleDegrees);
	
	
	/* Equivalent to PlayerControllers GetHitResultUnderCursorByChannel but with a capsule trace instead of line trace */
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static bool GetCapsuleTraceHitResultUnderCursorByChannel(APlayerController* PlayerController, ECollisionChannel TraceChannel, float TraceRadius, bool bTraceComplex, FHitResult& HitResult);
	static bool GetCapsuleTraceHitResultAtScreenPosition(const APlayerController* PlayerController, const FVector2D ScreenPosition, const ECollisionChannel TraceChannel, float TraceRadius, bool bTraceComplex, FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static bool GetCapsuleMultiTraceHitResultUnderCursorByChannel(APlayerController* PlayerController, ECollisionChannel TraceChannel, float TraceRadius, bool bTraceComplex, TArray<FHitResult>& OutHits, bool DrawDebug);
	static bool GetCapsuleMultiTraceHitResultsAtScreenPosition(const APlayerController* PlayerController, const FVector2D ScreenPosition, const ECollisionChannel TraceChannel, float TraceRadius, bool bTraceComplex, TArray<FHitResult>& OutHits, bool
	                                                           DebugDraw);

	// Creates a floating text message at the specified location. uses a custom K2 node, see RancEditorUtilities/K2NodeCreationHelper.h for details
	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (WorldContext = "WorldContextObject", CompactNodeTitle = "CreateFloatingText", BlueprintInternalUseOnly = "true"))
	static void CreateFloatingText(const UObject* WorldContextObject, const FString& Text, const FVector Location, const FRotator Rotation, const FLinearColor Color = FLinearColor::Red,
	                               const double Scale = 1.0f, const double LifeTime = 5.0f,  UFont* Font = nullptr, bool AlwaysFaceCamera = true);

	UFUNCTION(BlueprintPure, Category = "Utility")
	static FGameplayTag StringToGameplayTag(FName TagName);
	
	/*
	 * Visualizes a point in the world by moving a reusable debug cube to the specified location.
	 * Optionally performs a line trace from above to adjust the cube's position.
	 *
	 * @param Location - The target location for the debug cube.
	 * @param CubeSize - The size of the debug cube (default is 0.1, 0.1, 0.1).
	 * @param bLineTraceFromAbove - Whether to perform a line trace from above before placing the cube.
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	static void VisualizePoint(FVector Location, FVector CubeSize = FVector(0.1f, 0.1f, 0.1f), bool bLineTraceFromAbove = false);

private:
	// map that contains the last time a message was logged and the throttle period
	inline static TMap<FString, TTuple<double, float>> ThrottleLogMap = {};

	inline static TMap<FString, double> ThrottleMap = {};
};

/* Further ideas

"Snap to Grid": A function that takes a vector location and snaps it to the nearest point on a grid with a specified grid size.

"Is Point Inside Sphere": A function that checks if a given point is inside a sphere defined by its center and radius.

"Lerp Rotator": A function that performs linear interpolation between two rotators based on a provided alpha value.

"Get Closest Point on Line": A function that returns the closest point on a line segment to a given point in 3D space.

"Normalize Angle": A function that normalizes an angle to be within the range of -180 to 180 degrees.

"Is Point on Screen": A function that checks if a given 3D point in world space is visible on the screen.

"Get Mouse Position in World": A function that returns the mouse cursor position in world space based on the current camera view.

"Get Random Point in Cone": A function that generates a random point within a cone defined by its origin, direction, and angle.

"Is Point Inside Box": A function that checks if a given point is inside an axis-aligned bounding box (AABB) defined by its min and max vectors.

"Get Relative Rotation": A function that returns the relative rotation between two rotators, useful for calculating the difference in orientation between two objects.

*/