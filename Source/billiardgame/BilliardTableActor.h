#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BilliardTableActor.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class BILLIARDGAME_API ABilliardTableActor : public AActor
{
	GENERATED_BODY()

public:
	ABilliardTableActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandlePocketOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Table")
	bool bAutoBindPocketTriggers = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billiards|Table")
	FName PocketCollisionProfile = TEXT("OverlapAllDynamic");
};
