#include "BilliardCueActor.h"

#include "BilliardCueBallActor.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ABilliardCueActor::ABilliardCueActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	CueMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CueMesh"));
	CueMesh->SetupAttachment(SceneRoot);
	CueMesh->SetSimulatePhysics(false);
	CueMesh->SetEnableGravity(false);
	CueMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CueMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	AimArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("AimArrow"));
	AimArrow->SetupAttachment(SceneRoot);
	AimArrow->ArrowSize = 1.5f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CueMeshFinder(TEXT("/Game/Meshes/Cue/SM_Cue.SM_Cue"));
	if (CueMeshFinder.Succeeded())
	{
		CueMesh->SetStaticMesh(CueMeshFinder.Object);
	}
}

void ABilliardCueActor::BeginPlay()
{
	Super::BeginPlay();
	RefreshCueTransform();
}

void ABilliardCueActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bPendingShot)
	{
		PendingShotElapsed += DeltaSeconds;

		if (!bShotFired && PendingShotElapsed >= ShootAnimationDuration)
		{
			FirePendingShot();
		}

		if (PendingShotElapsed >= HideCueDelay)
		{
			SetCueVisible(false);
			bPendingShot = false;
		}
	}

	if (bIsAiming)
	{
		RefreshCueTransform();
	}
}

void ABilliardCueActor::SetCueBallRef(ABilliardCueBallActor* InCueBallRef)
{
	CueBallRef = InCueBallRef;
	RefreshCueTransform();
}

void ABilliardCueActor::SetAimDirection(const FVector& InAimDirection)
{
	AimDirection = GetClampedAimDirection(InAimDirection);
	RefreshCueTransform();
}

void ABilliardCueActor::UpdateAimFromWorldHit(const FVector& WorldHitPoint)
{
	if (!CueBallRef)
	{
		return;
	}

	const FVector CueBallLocation = CueBallRef->GetActorLocation();
	FVector NewDirection = WorldHitPoint - CueBallLocation;
	NewDirection.Z = 0.0f;
	SetAimDirection(NewDirection);
}

void ABilliardCueActor::AdjustPower(float DeltaPower)
{
	SetCurrentPower(CurrentPower + DeltaPower);
}

void ABilliardCueActor::SetCurrentPower(float NewPower)
{
	CurrentPower = FMath::Clamp(NewPower, 0.0f, MaxPower);
	RefreshCueTransform();
}

void ABilliardCueActor::AdjustHitOffset(const FVector2D& DeltaOffset)
{
	SetHitOffset(HitOffset + DeltaOffset);
}

void ABilliardCueActor::SetHitOffset(const FVector2D& NewHitOffset)
{
	HitOffset = ClampHitOffset(NewHitOffset);
}

void ABilliardCueActor::ReleaseShot()
{
	if (!CueBallRef || !bIsAiming || bPendingShot)
	{
		return;
	}

	bIsAiming = false;
	bPendingShot = true;
	bShotFired = false;
	PendingShotElapsed = 0.0f;
}

void ABilliardCueActor::SetCueVisible(bool bVisible)
{
	SetActorHiddenInGame(!bVisible);
	SetActorEnableCollision(bVisible);
	CueMesh->SetVisibility(bVisible, true);

	if (bVisible)
	{
		bShotFired = false;
		bPendingShot = false;
		PendingShotElapsed = 0.0f;
	}
}

void ABilliardCueActor::RefreshCueTransform()
{
	if (!CueBallRef)
	{
		return;
	}

	const FVector ClampedDirection = GetClampedAimDirection(AimDirection);
	AimDirection = ClampedDirection;

	const FVector CueBallLocation = CueBallRef->GetActorLocation();
	const float PullbackDistance = (CueLength * 0.5f) + (CurrentPower * 0.1f);
	const FVector CueLocation = CueBallLocation - (AimDirection * PullbackDistance);

	SetActorLocation(CueLocation);
	SetActorRotation((-AimDirection).Rotation());
	AimArrow->SetWorldRotation(AimDirection.Rotation());
}

void ABilliardCueActor::FirePendingShot()
{
	if (!CueBallRef || bShotFired)
	{
		return;
	}

	CueBallRef->FireShot(AimDirection, CurrentPower, HitOffset);
	bShotFired = true;
}

FVector ABilliardCueActor::GetClampedAimDirection(const FVector& Candidate) const
{
	FVector FlatDirection = Candidate;
	FlatDirection.Z = 0.0f;

	if (FlatDirection.IsNearlyZero())
	{
		return FVector(1.0f, 0.0f, 0.0f);
	}

	return FlatDirection.GetSafeNormal();
}

FVector2D ABilliardCueActor::ClampHitOffset(const FVector2D& Candidate) const
{
	const float CandidateSize = Candidate.Size();
	if (CandidateSize <= HitOffsetLimit || CandidateSize <= KINDA_SMALL_NUMBER)
	{
		return Candidate;
	}

	return Candidate.GetSafeNormal() * HitOffsetLimit;
}
