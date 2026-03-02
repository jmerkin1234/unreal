#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BilliardCueActor.generated.h"

class UArrowComponent;
class UStaticMeshComponent;
class ABilliardCueBallActor;

UCLASS(Blueprintable)
class BILLIARDGAME_API ABilliardCueActor : public AActor
{
	GENERATED_BODY()

public:
	ABilliardCueActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "Billiards|Cue")
	FVector GetAimDirection() const { return AimDirection; }

	UFUNCTION(BlueprintPure, Category = "Billiards|Cue")
	float GetCurrentPower() const { return CurrentPower; }

	UFUNCTION(BlueprintPure, Category = "Billiards|Cue")
	FVector2D GetHitOffset() const { return HitOffset; }

	UFUNCTION(BlueprintPure, Category = "Billiards|Cue")
	bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintPure, Category = "Billiards|Cue")
	ABilliardCueBallActor* GetCueBallRef() const { return CueBallRef; }

	UFUNCTION(BlueprintCallable, Category = "Billiards|Cue")
	void SetCueBallRef(ABilliardCueBallActor* InCueBallRef);

	UFUNCTION(BlueprintCallable, Category = "Billiards|Cue")
	void SetAimDirection(const FVector& InAimDirection);

	UFUNCTION(BlueprintCallable, Category = "Billiards|Cue")
	void UpdateAimFromWorldHit(const FVector& WorldHitPoint);

	UFUNCTION(BlueprintCallable, Category = "Billiards|Cue")
	void AdjustPower(float DeltaPower);

	UFUNCTION(BlueprintCallable, Category = "Billiards|Cue")
	void SetCurrentPower(float NewPower);

	UFUNCTION(BlueprintCallable, Category = "Billiards|Cue")
	void AdjustHitOffset(const FVector2D& DeltaOffset);

	UFUNCTION(BlueprintCallable, Category = "Billiards|Cue")
	void SetHitOffset(const FVector2D& NewHitOffset);

	UFUNCTION(BlueprintCallable, Category = "Billiards|Cue")
	void ReleaseShot();

	UFUNCTION(BlueprintCallable, Category = "Billiards|Cue")
	void SetCueVisible(bool bVisible);

protected:
	virtual void BeginPlay() override;

private:
	void RefreshCueTransform();
	void FirePendingShot();
	FVector GetClampedAimDirection(const FVector& Candidate) const;
	FVector2D ClampHitOffset(const FVector2D& Candidate) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|Cue", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|Cue", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> CueMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|Cue", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> AimArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (AllowPrivateAccess = "true"))
	FVector AimDirection = FVector(1.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (ClampMin = "0.0", ClampMax = "100.0", AllowPrivateAccess = "true"))
	float CurrentPower = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (AllowPrivateAccess = "true"))
	FVector2D HitOffset = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (AllowPrivateAccess = "true"))
	bool bIsAiming = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float MaxPower = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ABilliardCueBallActor> CueBallRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float CueLength = 147.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float HitOffsetLimit = 2.625f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float ShootAnimationDuration = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float HideCueDelay = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (AllowPrivateAccess = "true"))
	bool bPendingShot = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (AllowPrivateAccess = "true"))
	bool bShotFired = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue", meta = (AllowPrivateAccess = "true"))
	float PendingShotElapsed = 0.0f;
};
