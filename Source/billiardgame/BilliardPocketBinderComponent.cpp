#include "BilliardPocketBinderComponent.h"

#include "BilliardBallActor.h"
#include "BilliardGameMode8Ball.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"

UBilliardPocketBinderComponent::UBilliardPocketBinderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBilliardPocketBinderComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!bAutoBindPocketTriggers)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	TArray<UBoxComponent*> BoxComponents;
	Owner->GetComponents<UBoxComponent>(BoxComponents);

	for (UBoxComponent* Box : BoxComponents)
	{
		if (!Box)
		{
			continue;
		}

		if (PocketCollisionProfile != NAME_None)
		{
			Box->SetCollisionProfileName(PocketCollisionProfile);
		}

		Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Box->SetGenerateOverlapEvents(true);
		Box->OnComponentBeginOverlap.AddDynamic(this, &UBilliardPocketBinderComponent::HandlePocketOverlap);
	}
}

void UBilliardPocketBinderComponent::HandlePocketOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	ABilliardBallActor* Ball = Cast<ABilliardBallActor>(OtherActor);
	if (!Ball)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ABilliardGameMode8Ball* GameMode = Cast<ABilliardGameMode8Ball>(World->GetAuthGameMode());
	if (!GameMode)
	{
		return;
	}

	GameMode->BallPocketed(Ball);
}
