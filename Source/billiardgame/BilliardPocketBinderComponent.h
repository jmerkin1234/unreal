#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BilliardPocketBinderComponent.generated.h"

class UBoxComponent;

UCLASS(ClassGroup = (Billiards), Blueprintable, meta = (BlueprintSpawnableComponent))
class BILLIARDGAME_API UBilliardPocketBinderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBilliardPocketBinderComponent();

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
