#pragma once

#include "CoreMinimal.h"
#include "BilliardsTypes.generated.h"

UENUM(BlueprintType)
enum class EBilliardsPlayerGroup : uint8
{
	Unassigned UMETA(DisplayName = "Unassigned"),
	Solids UMETA(DisplayName = "Solids"),
	Stripes UMETA(DisplayName = "Stripes")
};

UENUM(BlueprintType)
enum class EBilliardsGamePhase : uint8
{
	BallInHand UMETA(DisplayName = "Ball In Hand"),
	Aiming UMETA(DisplayName = "Aiming"),
	BallsMoving UMETA(DisplayName = "Balls Moving"),
	RoundResolved UMETA(DisplayName = "Round Resolved"),
	GameOver UMETA(DisplayName = "Game Over")
};

UENUM(BlueprintType)
enum class EBilliardsFoulReason : uint8
{
	None UMETA(DisplayName = "None"),
	ScratchCueBall UMETA(DisplayName = "Scratch Cue Ball"),
	NoBallHit UMETA(DisplayName = "No Ball Hit"),
	WrongGroupFirst UMETA(DisplayName = "Wrong Group First"),
	EightBallEarlyPocket UMETA(DisplayName = "Eight Ball Early Pocket"),
	ScratchOnEight UMETA(DisplayName = "Scratch On Eight")
};
