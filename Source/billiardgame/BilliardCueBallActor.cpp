#include "BilliardCueBallActor.h"

#include "Components/StaticMeshComponent.h"

ABilliardCueBallActor::ABilliardCueBallActor()
{
	BallNumber = 0;
}

void ABilliardCueBallActor::FireShot(const FVector& Direction, float Power, const FVector2D& HitOffset)
{
	UStaticMeshComponent* BallMesh = GetBallMesh();
	if (!BallMesh)
	{
		return;
	}

	const FVector ForwardAxis = Direction.GetSafeNormal();
	if (ForwardAxis.IsNearlyZero())
	{
		return;
	}

	CueHitOffset = HitOffset;
	TopspinAmount = HitOffset.Y / BallRadius;
	SidespinAmount = HitOffset.X / BallRadius;

	const float ClampedPower = FMath::Clamp(Power, 0.0f, 100.0f);
	const float ImpulseStrength = (ClampedPower / 100.0f) * MaxImpulse;

	BallMesh->WakeAllRigidBodies();
	BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector, false);
	BallMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector, false);
	BallMesh->AddImpulse(ForwardAxis * ImpulseStrength, NAME_None, false);

	FVector RightAxis = FVector::CrossProduct(ForwardAxis, FVector::UpVector).GetSafeNormal();
	if (RightAxis.IsNearlyZero())
	{
		RightAxis = FVector::RightVector;
	}

	const FVector WorldSpinAxis = (RightAxis * SidespinAmount) + (ForwardAxis * TopspinAmount);
	const FVector InitialAngularImpulse = WorldSpinAxis * ImpulseStrength * 0.1f;
	BallMesh->AddAngularImpulseInRadians(InitialAngularImpulse, NAME_None, false);
}
