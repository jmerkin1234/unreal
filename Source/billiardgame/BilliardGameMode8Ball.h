#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BilliardsTypes.h"
#include "BilliardGameMode8Ball.generated.h"

class ABilliardBallActor;
class ABilliardCueBallActor;

UCLASS(Blueprintable)
class BILLIARDGAME_API ABilliardGameMode8Ball : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABilliardGameMode8Ball();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Billiards|GameMode")
	void RegisterBall(ABilliardBallActor* Ball);

	UFUNCTION(BlueprintCallable, Category = "Billiards|GameMode")
	void SetCueBallRef(ABilliardCueBallActor* InCueBallRef);

	UFUNCTION(BlueprintCallable, Category = "Billiards|GameMode")
	void BallPocketed(ABilliardBallActor* Ball);

	UFUNCTION(BlueprintCallable, Category = "Billiards|GameMode")
	void OnCueBallHitBall(ABilliardBallActor* HitBall);

	UFUNCTION(BlueprintCallable, Category = "Billiards|GameMode")
	void PlayerTakesShot();

	UFUNCTION(BlueprintCallable, Category = "Billiards|GameMode")
	void EvaluateTurnResult();

	UFUNCTION(BlueprintCallable, Category = "Billiards|GameMode")
	void SwitchPlayer();

	UFUNCTION(BlueprintPure, Category = "Billiards|GameMode")
	int32 GetOtherPlayerIndex() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Billiards|GameMode")
	void ShowCue();

	UFUNCTION(BlueprintImplementableEvent, Category = "Billiards|GameMode")
	void ShowFoul(EBilliardsFoulReason Reason);

	UFUNCTION(BlueprintImplementableEvent, Category = "Billiards|GameMode")
	void RespawnCueBall();

	UFUNCTION(BlueprintImplementableEvent, Category = "Billiards|GameMode")
	void OnGameEnded(int32 WinnerPlayer, int32 LoserPlayer, EBilliardsFoulReason EndReason);

protected:
	UFUNCTION()
	void PollBallsStopped();

	void AutoRegisterPlacedBalls();
	bool AreActiveBallsSettled() const;
	bool HasPocketedBallNumber(int32 BallNumber) const;
	ABilliardBallActor* FindFirstScoringBall() const;
	bool HasValidPocketForCurrentPlayer() const;
	int32 CountRemainingGroupBalls(int32 PlayerIndex) const;
	void AssignGroupsFromBall(const ABilliardBallActor* ScoringBall);
	void EndGameForWinner(int32 WinnerPlayer, EBilliardsFoulReason EndReason);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|GameMode")
	int32 CurrentPlayer = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|GameMode")
	TArray<EBilliardsPlayerGroup> PlayerGroups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|GameMode")
	EBilliardsGamePhase GamePhase = EBilliardsGamePhase::Aiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|GameMode")
	TArray<TObjectPtr<ABilliardBallActor>> PocketedThisTurn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|GameMode")
	TObjectPtr<ABilliardBallActor> FirstBallHitThisTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|GameMode")
	EBilliardsFoulReason FoulThisTurn = EBilliardsFoulReason::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|GameMode")
	TArray<TObjectPtr<ABilliardBallActor>> BallsOnTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|GameMode")
	bool bIsBreakShot = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|GameMode")
	TObjectPtr<ABilliardCueBallActor> CueBallRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|GameMode", meta = (ClampMin = "0.01"))
	float BallStopPollInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|GameMode")
	bool bAutoRegisterPlacedBalls = true;

	FTimerHandle BallStopPollTimerHandle;
};
