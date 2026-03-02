#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BilliardsTypes.h"
#include "BilliardBallActor.generated.h"

class UPhysicalMaterial;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class BILLIARDGAME_API ABilliardBallActor : public AActor
{
	GENERATED_BODY()

public:
	ABilliardBallActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Billiards|State")
	UStaticMeshComponent* GetBallMesh() const { return BallMesh; }

	UFUNCTION(BlueprintPure, Category = "Billiards|State")
	int32 GetBallNumber() const { return BallNumber; }

	UFUNCTION(BlueprintPure, Category = "Billiards|State")
	EBilliardsPlayerGroup GetOwnerGroupValue() const { return OwnerGroup; }

	UFUNCTION(BlueprintPure, Category = "Billiards|State")
	bool IsPocketed() const { return bIsPocketed; }

	UFUNCTION(BlueprintCallable, Category = "Billiards|State")
	void SetPocketedState(bool bNewPocketed);

	UFUNCTION(BlueprintPure, Category = "Billiards|State")
	bool IsSettled() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleBallHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	void ConfigurePhysics();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards")
	TObjectPtr<UStaticMeshComponent> BallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Spin")
	float TopspinAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Spin")
	float SidespinAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billiards|Spin")
	bool bIsSliding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|State")
	int32 BallNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|State")
	EBilliardsPlayerGroup OwnerGroup = EBilliardsPlayerGroup::Unassigned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|State")
	bool bIsPocketed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Tuning")
	float StopThreshold = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Tuning")
	float SpinDecayRate = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Tuning")
	float SlidingFriction = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Tuning")
	float BallRadius = 2.625f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Tuning")
	float CushionVelocityLossScale = 0.97f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Tuning")
	float CushionSidespinDeflectionDegrees = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Physics")
	float BallMassKg = 0.17f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Physics")
	float AngularDamping = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Physics")
	float LinearDamping = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Physics")
	TObjectPtr<UPhysicalMaterial> BallPhysicalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Physics")
	TObjectPtr<UPhysicalMaterial> CushionPhysicalMaterial;
};
