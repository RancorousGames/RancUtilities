#pragma once

UENUM(BlueprintType)
enum class EGroundType : uint8
{
	Ground UMETA(DisplayName = "Ground"),
	Water UMETA(DisplayName = "Water")
};


UENUM(BlueprintType)
enum class ERelationship : uint8
{
	Friendly,
	Same,
	Hostile,
	Neutral
};
