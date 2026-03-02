#pragma once

#include "CoreMinimal.h"
#include "BilliardBallActor.h"
#include "BilliardCueBallActor.generated.h"

UCLASS(Blueprintable)
class BILLIARDGAME_API ABilliardCueBallActor : public ABilliardBallActor
{
	GENERATED_BODY()

public:
	ABilliardCueBallActor();

	UFUNCTION(BlueprintCallable, Category = "Billiards|Cue")
	void FireShot(const FVector& Direction, float Power, const FVector2D& HitOffset);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue")
	FVector2D CueHitOffset = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Cue")
	float MaxImpulse = 500.0f;
};
