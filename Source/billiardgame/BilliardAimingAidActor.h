#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BilliardAimingAidActor.generated.h"

class ABilliardBallActor;
class ABilliardCueActor;
class ABilliardCueBallActor;
class UArrowComponent;
class UMaterialInterface;
class USplineComponent;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class BILLIARDGAME_API ABilliardAimingAidActor : public AActor
{
	GENERATED_BODY()

public:
	ABilliardAimingAidActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Billiards|Aiming")
	void SetCueRef(ABilliardCueActor* InCueRef);

	UFUNCTION(BlueprintCallable, Category = "Billiards|Aiming")
	void SetCueBallRef(ABilliardCueBallActor* InCueBallRef);

	UFUNCTION(BlueprintCallable, Category = "Billiards|Aiming")
	void UpdateFromAimState(const FVector& StartLocation, const FVector& AimDirection);

	UFUNCTION(BlueprintCallable, Category = "Billiards|Aiming")
	void HideAids();

protected:
	virtual void BeginPlay() override;

private:
	void ConfigureGhostMesh(UStaticMeshComponent* MeshComponent) const;
	void UpdateTrajectorySpline(const FVector& StartLocation, const FVector& ContactLocation);
	void ShowBallContactAids(const FHitResult& Hit, ABilliardBallActor* HitBall, const FVector& StartLocation, const FVector& AimDirection);
	void ShowCushionReflectionAid(const FHitResult& Hit, const FVector& StartLocation, const FVector& AimDirection);
	FVector GetCueBallLocation() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|Aiming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|Aiming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> TrajectorySpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|Aiming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> GhostCueBallMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|Aiming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> TargetGhostMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|Aiming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> TargetDirectionArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|Aiming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> ReflectionArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Aiming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ABilliardCueActor> CueRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Aiming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ABilliardCueBallActor> CueBallRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Aiming", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float MaxTraceLength = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Aiming", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float BallDiameter = 5.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Aiming", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float ArrowScale = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Aiming", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> GhostMaterial;
};
