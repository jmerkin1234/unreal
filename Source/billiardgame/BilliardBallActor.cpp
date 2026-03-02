#include "BilliardBallActor.h"

#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UObject/ConstructorHelpers.h"

ABilliardBallActor::ABilliardBallActor()
{
	PrimaryActorTick.bCanEverTick = true;

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	SetRootComponent(BallMesh);

	BallMesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	BallMesh->SetNotifyRigidBodyCollision(true);
	BallMesh->SetGenerateOverlapEvents(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BallMeshAsset(TEXT("/Game/Meshes/Balls/SM_Ball.SM_Ball"));
	if (BallMeshAsset.Succeeded())
	{
		BallMesh->SetStaticMesh(BallMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> BallPmAsset(TEXT("/Game/Physics/PM_Ball.PM_Ball"));
	if (BallPmAsset.Succeeded())
	{
		BallPhysicalMaterial = BallPmAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> CushionPmAsset(TEXT("/Game/Physics/PM_Cushion.PM_Cushion"));
	if (CushionPmAsset.Succeeded())
	{
		CushionPhysicalMaterial = CushionPmAsset.Object;
	}
}

void ABilliardBallActor::BeginPlay()
{
	Super::BeginPlay();

	ConfigurePhysics();

	if (ensure(BallMesh))
	{
		BallMesh->OnComponentHit.AddDynamic(this, &ABilliardBallActor::HandleBallHit);
	}
}

void ABilliardBallActor::SetPocketedState(bool bNewPocketed)
{
	bIsPocketed = bNewPocketed;
}

bool ABilliardBallActor::IsSettled() const
{
	if (!BallMesh || !BallMesh->IsSimulatingPhysics())
	{
		return true;
	}

	if (BallMesh->RigidBodyIsAwake() == false)
	{
		return true;
	}

	const FVector LinearVelocity = BallMesh->GetPhysicsLinearVelocity();
	const FVector AngularVelocity = BallMesh->GetPhysicsAngularVelocityInRadians();
	return LinearVelocity.Size() < StopThreshold && AngularVelocity.Size() < 1.0f;
}

void ABilliardBallActor::ConfigurePhysics()
{
	if (!BallMesh)
	{
		return;
	}

	BallMesh->SetSimulatePhysics(true);
	BallMesh->SetEnableGravity(true);
	BallMesh->SetUseCCD(true);
	BallMesh->SetAngularDamping(AngularDamping);
	BallMesh->SetLinearDamping(LinearDamping);
	BallMesh->SetMassOverrideInKg(NAME_None, BallMassKg, true);

	if (BallPhysicalMaterial)
	{
		BallMesh->SetPhysMaterialOverride(BallPhysicalMaterial);
	}
}

void ABilliardBallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!BallMesh || !BallMesh->IsSimulatingPhysics() || bIsPocketed)
	{
		return;
	}

	const FVector LinearVelocity = BallMesh->GetPhysicsLinearVelocity();
	const FVector AngularVelocity = BallMesh->GetPhysicsAngularVelocityInRadians();
	const FVector SurfaceNormal = FVector::UpVector;
	const FVector ContactOffset = -SurfaceNormal * BallRadius;
	const FVector ContactPointVelocity = LinearVelocity + FVector::CrossProduct(AngularVelocity, ContactOffset);
	const FVector TangentialSlip(ContactPointVelocity.X, ContactPointVelocity.Y, 0.0f);
	const float SlipSpeed = TangentialSlip.Size();

	if (SlipSpeed > 2.0f)
	{
		bIsSliding = true;

		const FVector SlipDirection = TangentialSlip.GetSafeNormal();
		const FVector FrictionForce = -SlipDirection * SlidingFriction * BallMassKg * 980.0f;
		const FVector FrictionTorque = FVector::CrossProduct(ContactOffset, FrictionForce);

		BallMesh->AddForce(FrictionForce);
		BallMesh->AddTorqueInRadians(FrictionTorque);
	}
	else
	{
		bIsSliding = false;

		const FVector RollingTargetAngularVelocity = FVector::CrossProduct(SurfaceNormal, LinearVelocity) / BallRadius;
		const FVector VerticalSpin(0.0f, 0.0f, AngularVelocity.Z);
		const FVector CorrectedAngularVelocity = FMath::VInterpTo(
			AngularVelocity,
			RollingTargetAngularVelocity + VerticalSpin,
			DeltaTime,
			12.0f);

		BallMesh->SetPhysicsAngularVelocityInRadians(CorrectedAngularVelocity, false);
	}

	TopspinAmount = FMath::FInterpTo(TopspinAmount, 0.0f, DeltaTime, SpinDecayRate);
	SidespinAmount = FMath::FInterpTo(SidespinAmount, 0.0f, DeltaTime, SpinDecayRate);

	if (LinearVelocity.Size() < StopThreshold && AngularVelocity.Size() < 1.0f)
	{
		BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector, false);
		BallMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector, false);
		BallMesh->PutRigidBodyToSleep();
	}
}

void ABilliardBallActor::HandleBallHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (!BallMesh || !OtherComp)
	{
		return;
	}

	UPhysicalMaterial* HitPhysicalMaterial = Hit.PhysMaterial.Get();
	const bool bHitCushion =
		(CushionPhysicalMaterial && HitPhysicalMaterial == CushionPhysicalMaterial) ||
		(HitPhysicalMaterial && HitPhysicalMaterial->GetName().Contains(TEXT("PM_Cushion")));

	if (!bHitCushion)
	{
		return;
	}

	FVector ExitVelocity = BallMesh->GetPhysicsLinearVelocity() * CushionVelocityLossScale;

	if (!FMath::IsNearlyZero(SidespinAmount))
	{
		ExitVelocity = ExitVelocity.RotateAngleAxis(
			SidespinAmount * CushionSidespinDeflectionDegrees,
			FVector::UpVector);
	}

	BallMesh->SetPhysicsLinearVelocity(ExitVelocity, false);
}
