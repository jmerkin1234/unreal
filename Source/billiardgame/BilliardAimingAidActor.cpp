#include "BilliardAimingAidActor.h"

#include "BilliardBallActor.h"
#include "BilliardCueActor.h"
#include "BilliardCueBallActor.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ABilliardAimingAidActor::ABilliardAimingAidActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TrajectorySpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrajectorySpline"));
	TrajectorySpline->SetupAttachment(SceneRoot);

	GhostCueBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostCueBallMesh"));
	GhostCueBallMesh->SetupAttachment(SceneRoot);

	TargetGhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetGhostMesh"));
	TargetGhostMesh->SetupAttachment(SceneRoot);

	TargetDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("TargetDirectionArrow"));
	TargetDirectionArrow->SetupAttachment(SceneRoot);
	TargetDirectionArrow->ArrowSize = ArrowScale;

	ReflectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ReflectionArrow"));
	ReflectionArrow->SetupAttachment(SceneRoot);
	ReflectionArrow->ArrowSize = ArrowScale;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BallMeshAsset(TEXT("/Game/Meshes/Balls/SM_Ball.SM_Ball"));
	if (BallMeshAsset.Succeeded())
	{
		GhostCueBallMesh->SetStaticMesh(BallMeshAsset.Object);
		TargetGhostMesh->SetStaticMesh(BallMeshAsset.Object);
	}

	ConfigureGhostMesh(GhostCueBallMesh);
	ConfigureGhostMesh(TargetGhostMesh);

	HideAids();
}

void ABilliardAimingAidActor::BeginPlay()
{
	Super::BeginPlay();
	HideAids();
}

void ABilliardAimingAidActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!CueRef)
	{
		HideAids();
		return;
	}

	if (!CueBallRef)
	{
		CueBallRef = CueRef->GetCueBallRef();
	}

	if (!CueBallRef || !CueRef->IsAiming())
	{
		HideAids();
		return;
	}

	UpdateFromAimState(GetCueBallLocation(), CueRef->GetAimDirection());
}

void ABilliardAimingAidActor::SetCueRef(ABilliardCueActor* InCueRef)
{
	CueRef = InCueRef;
	if (!CueBallRef && CueRef)
	{
		CueBallRef = CueRef->GetCueBallRef();
	}
}

void ABilliardAimingAidActor::SetCueBallRef(ABilliardCueBallActor* InCueBallRef)
{
	CueBallRef = InCueBallRef;
}

void ABilliardAimingAidActor::UpdateFromAimState(const FVector& StartLocation, const FVector& AimDirection)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector SafeAimDirection = AimDirection.GetSafeNormal();
	if (SafeAimDirection.IsNearlyZero())
	{
		HideAids();
		return;
	}

	const FVector TraceEnd = StartLocation + (SafeAimDirection * MaxTraceLength);
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(BilliardsAimTrace), false, this);
	if (CueBallRef)
	{
		Params.AddIgnoredActor(CueBallRef);
	}
	if (CueRef)
	{
		Params.AddIgnoredActor(CueRef);
	}

	const bool bHit = World->LineTraceSingleByChannel(Hit, StartLocation, TraceEnd, ECC_Visibility, Params);
	const FVector ContactLocation = bHit ? Hit.ImpactPoint : TraceEnd;
	UpdateTrajectorySpline(StartLocation, ContactLocation);

	if (!bHit)
	{
		HideAids();
		UpdateTrajectorySpline(StartLocation, TraceEnd);
		return;
	}

	if (ABilliardBallActor* HitBall = Cast<ABilliardBallActor>(Hit.GetActor()))
	{
		if (HitBall->GetBallNumber() != 0 && !HitBall->IsPocketed())
		{
			ShowBallContactAids(Hit, HitBall, StartLocation, SafeAimDirection);
			return;
		}
	}

	ShowCushionReflectionAid(Hit, StartLocation, SafeAimDirection);
}

void ABilliardAimingAidActor::HideAids()
{
	GhostCueBallMesh->SetVisibility(false, true);
	TargetGhostMesh->SetVisibility(false, true);
	TargetDirectionArrow->SetVisibility(false, true);
	ReflectionArrow->SetVisibility(false, true);
	TrajectorySpline->SetVisibility(false, true);
}

void ABilliardAimingAidActor::ConfigureGhostMesh(UStaticMeshComponent* MeshComponent) const
{
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetCastShadow(false);
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetEnableGravity(false);

	if (GhostMaterial)
	{
		MeshComponent->SetMaterial(0, GhostMaterial);
	}
}

void ABilliardAimingAidActor::UpdateTrajectorySpline(const FVector& StartLocation, const FVector& ContactLocation)
{
	TrajectorySpline->ClearSplinePoints(false);
	TrajectorySpline->AddSplinePoint(StartLocation, ESplineCoordinateSpace::World, false);
	TrajectorySpline->AddSplinePoint(ContactLocation, ESplineCoordinateSpace::World, false);
	TrajectorySpline->UpdateSpline();
	TrajectorySpline->SetVisibility(true, true);
}

void ABilliardAimingAidActor::ShowBallContactAids(const FHitResult& Hit, ABilliardBallActor* HitBall, const FVector& StartLocation, const FVector& AimDirection)
{
	const FVector GhostCueBallLocation = Hit.ImpactPoint - (AimDirection * BallDiameter);
	const FVector TargetBallLocation = HitBall->GetActorLocation();
	const FVector TargetTravelDirection = (TargetBallLocation - GhostCueBallLocation).GetSafeNormal();

	GhostCueBallMesh->SetWorldLocation(GhostCueBallLocation);
	GhostCueBallMesh->SetVisibility(true, true);

	TargetGhostMesh->SetWorldLocation(TargetBallLocation);
	TargetGhostMesh->SetVisibility(true, true);

	TargetDirectionArrow->SetWorldLocation(TargetBallLocation);
	TargetDirectionArrow->SetWorldRotation(TargetTravelDirection.Rotation());
	TargetDirectionArrow->SetVisibility(true, true);

	ReflectionArrow->SetVisibility(false, true);
	UpdateTrajectorySpline(StartLocation, Hit.ImpactPoint);
}

void ABilliardAimingAidActor::ShowCushionReflectionAid(const FHitResult& Hit, const FVector& StartLocation, const FVector& AimDirection)
{
	const FVector ReflectedDirection = FMath::GetReflectionVector(AimDirection, Hit.ImpactNormal).GetSafeNormal();

	GhostCueBallMesh->SetVisibility(false, true);
	TargetGhostMesh->SetVisibility(false, true);
	TargetDirectionArrow->SetVisibility(false, true);

	ReflectionArrow->SetWorldLocation(Hit.ImpactPoint);
	ReflectionArrow->SetWorldRotation(ReflectedDirection.Rotation());
	ReflectionArrow->SetVisibility(true, true);

	UpdateTrajectorySpline(StartLocation, Hit.ImpactPoint);
}

FVector ABilliardAimingAidActor::GetCueBallLocation() const
{
	return CueBallRef ? CueBallRef->GetActorLocation() : FVector::ZeroVector;
}
