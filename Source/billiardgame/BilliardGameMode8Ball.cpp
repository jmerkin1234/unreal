#include "BilliardGameMode8Ball.h"

#include "BilliardBallActor.h"
#include "BilliardCueBallActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"

ABilliardGameMode8Ball::ABilliardGameMode8Ball()
{
	PlayerGroups.Init(EBilliardsPlayerGroup::Unassigned, 2);
}

void ABilliardGameMode8Ball::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerGroups.Num() != 2)
	{
		PlayerGroups.Init(EBilliardsPlayerGroup::Unassigned, 2);
	}

	if (bAutoRegisterPlacedBalls)
	{
		AutoRegisterPlacedBalls();
	}
}

void ABilliardGameMode8Ball::AutoRegisterPlacedBalls()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	BallsOnTable.Reset();
	CueBallRef = nullptr;

	for (TActorIterator<ABilliardBallActor> It(World); It; ++It)
	{
		RegisterBall(*It);
	}
}

void ABilliardGameMode8Ball::RegisterBall(ABilliardBallActor* Ball)
{
	if (!Ball)
	{
		return;
	}

	BallsOnTable.AddUnique(Ball);

	if (Ball->GetBallNumber() == 0)
	{
		if (ABilliardCueBallActor* CueBall = Cast<ABilliardCueBallActor>(Ball))
		{
			CueBallRef = CueBall;
		}
	}
}

void ABilliardGameMode8Ball::SetCueBallRef(ABilliardCueBallActor* InCueBallRef)
{
	CueBallRef = InCueBallRef;

	if (CueBallRef)
	{
		RegisterBall(CueBallRef);
	}
}

void ABilliardGameMode8Ball::BallPocketed(ABilliardBallActor* Ball)
{
	if (!Ball)
	{
		return;
	}

	RegisterBall(Ball);
	PocketedThisTurn.AddUnique(Ball);
	Ball->SetPocketedState(true);
	Ball->SetActorHiddenInGame(true);

	if (UStaticMeshComponent* BallMesh = Ball->GetBallMesh())
	{
		BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector, false);
		BallMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector, false);
		BallMesh->PutRigidBodyToSleep();
	}
}

void ABilliardGameMode8Ball::OnCueBallHitBall(ABilliardBallActor* HitBall)
{
	if (!FirstBallHitThisTurn)
	{
		FirstBallHitThisTurn = HitBall;
	}
}

void ABilliardGameMode8Ball::PlayerTakesShot()
{
	PocketedThisTurn.Reset();
	FirstBallHitThisTurn = nullptr;
	FoulThisTurn = EBilliardsFoulReason::None;
	GamePhase = EBilliardsGamePhase::BallsMoving;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BallStopPollTimerHandle);
		World->GetTimerManager().SetTimer(
			BallStopPollTimerHandle,
			this,
			&ABilliardGameMode8Ball::PollBallsStopped,
			BallStopPollInterval,
			true);
	}
}

void ABilliardGameMode8Ball::EvaluateTurnResult()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BallStopPollTimerHandle);
	}

	const bool bPocketedEightBall = HasPocketedBallNumber(8);
	const int32 OtherPlayer = GetOtherPlayerIndex();

	if (CueBallRef && CueBallRef->IsPocketed())
	{
		FoulThisTurn = bPocketedEightBall ? EBilliardsFoulReason::ScratchOnEight : EBilliardsFoulReason::ScratchCueBall;
	}
	else if (!FirstBallHitThisTurn)
	{
		FoulThisTurn = EBilliardsFoulReason::NoBallHit;
	}
	else if (PlayerGroups.IsValidIndex(CurrentPlayer) &&
		PlayerGroups[CurrentPlayer] != EBilliardsPlayerGroup::Unassigned &&
		FirstBallHitThisTurn->GetOwnerGroupValue() != PlayerGroups[CurrentPlayer] &&
		FirstBallHitThisTurn->GetBallNumber() != 8)
	{
		FoulThisTurn = EBilliardsFoulReason::WrongGroupFirst;
	}

	if (bPocketedEightBall)
	{
		if (CountRemainingGroupBalls(CurrentPlayer) > 0)
		{
			FoulThisTurn = EBilliardsFoulReason::EightBallEarlyPocket;
			EndGameForWinner(OtherPlayer, FoulThisTurn);
		}
		else if (FoulThisTurn != EBilliardsFoulReason::None)
		{
			EndGameForWinner(OtherPlayer, FoulThisTurn);
		}
		else
		{
			EndGameForWinner(CurrentPlayer, EBilliardsFoulReason::None);
		}

		bIsBreakShot = false;
		return;
	}

	if (!bIsBreakShot &&
		PlayerGroups.Num() >= 2 &&
		PlayerGroups[0] == EBilliardsPlayerGroup::Unassigned &&
		PocketedThisTurn.Num() > 0 &&
		FoulThisTurn == EBilliardsFoulReason::None)
	{
		if (ABilliardBallActor* FirstScoringBall = FindFirstScoringBall())
		{
			AssignGroupsFromBall(FirstScoringBall);
		}
	}

	if (FoulThisTurn != EBilliardsFoulReason::None)
	{
		ShowFoul(FoulThisTurn);
		SwitchPlayer();
		GamePhase = EBilliardsGamePhase::BallInHand;
		RespawnCueBall();
	}
	else if (HasValidPocketForCurrentPlayer())
	{
		GamePhase = EBilliardsGamePhase::Aiming;
	}
	else
	{
		SwitchPlayer();
		GamePhase = EBilliardsGamePhase::Aiming;
	}

	if (bIsBreakShot)
	{
		bIsBreakShot = false;
	}

	ShowCue();
}

void ABilliardGameMode8Ball::SwitchPlayer()
{
	CurrentPlayer = GetOtherPlayerIndex();
}

int32 ABilliardGameMode8Ball::GetOtherPlayerIndex() const
{
	return CurrentPlayer == 0 ? 1 : 0;
}

void ABilliardGameMode8Ball::PollBallsStopped()
{
	if (AreActiveBallsSettled())
	{
		EvaluateTurnResult();
	}
}

bool ABilliardGameMode8Ball::AreActiveBallsSettled() const
{
	for (ABilliardBallActor* Ball : BallsOnTable)
	{
		if (!Ball || Ball->IsPocketed())
		{
			continue;
		}

		if (!Ball->IsSettled())
		{
			return false;
		}
	}

	return true;
}

bool ABilliardGameMode8Ball::HasPocketedBallNumber(int32 BallNumber) const
{
	for (ABilliardBallActor* Ball : PocketedThisTurn)
	{
		if (Ball && Ball->GetBallNumber() == BallNumber)
		{
			return true;
		}
	}

	return false;
}

ABilliardBallActor* ABilliardGameMode8Ball::FindFirstScoringBall() const
{
	for (ABilliardBallActor* Ball : PocketedThisTurn)
	{
		if (!Ball)
		{
			continue;
		}

		const int32 BallNumber = Ball->GetBallNumber();
		if (BallNumber != 0 && BallNumber != 8)
		{
			return Ball;
		}
	}

	return nullptr;
}

bool ABilliardGameMode8Ball::HasValidPocketForCurrentPlayer() const
{
	if (!PlayerGroups.IsValidIndex(CurrentPlayer))
	{
		return false;
	}

	const EBilliardsPlayerGroup CurrentGroup = PlayerGroups[CurrentPlayer];

	for (ABilliardBallActor* Ball : PocketedThisTurn)
	{
		if (!Ball)
		{
			continue;
		}

		const int32 BallNumber = Ball->GetBallNumber();
		if (BallNumber == 0 || BallNumber == 8)
		{
			continue;
		}

		if (CurrentGroup == EBilliardsPlayerGroup::Unassigned || Ball->GetOwnerGroupValue() == CurrentGroup)
		{
			return true;
		}
	}

	return false;
}

int32 ABilliardGameMode8Ball::CountRemainingGroupBalls(int32 PlayerIndex) const
{
	if (!PlayerGroups.IsValidIndex(PlayerIndex))
	{
		return 0;
	}

	const EBilliardsPlayerGroup TargetGroup = PlayerGroups[PlayerIndex];
	if (TargetGroup == EBilliardsPlayerGroup::Unassigned)
	{
		return 0;
	}

	int32 RemainingCount = 0;

	for (ABilliardBallActor* Ball : BallsOnTable)
	{
		if (!Ball || Ball->IsPocketed())
		{
			continue;
		}

		const int32 BallNumber = Ball->GetBallNumber();
		if (BallNumber == 0 || BallNumber == 8)
		{
			continue;
		}

		if (Ball->GetOwnerGroupValue() == TargetGroup)
		{
			++RemainingCount;
		}
	}

	return RemainingCount;
}

void ABilliardGameMode8Ball::AssignGroupsFromBall(const ABilliardBallActor* ScoringBall)
{
	if (!ScoringBall || PlayerGroups.Num() < 2)
	{
		return;
	}

	if (ScoringBall->GetBallNumber() <= 7)
	{
		PlayerGroups[CurrentPlayer] = EBilliardsPlayerGroup::Solids;
		PlayerGroups[GetOtherPlayerIndex()] = EBilliardsPlayerGroup::Stripes;
	}
	else
	{
		PlayerGroups[CurrentPlayer] = EBilliardsPlayerGroup::Stripes;
		PlayerGroups[GetOtherPlayerIndex()] = EBilliardsPlayerGroup::Solids;
	}
}

void ABilliardGameMode8Ball::EndGameForWinner(int32 WinnerPlayer, EBilliardsFoulReason EndReason)
{
	GamePhase = EBilliardsGamePhase::GameOver;
	const int32 LoserPlayer = WinnerPlayer == 0 ? 1 : 0;
	OnGameEnded(WinnerPlayer, LoserPlayer, EndReason);
}
