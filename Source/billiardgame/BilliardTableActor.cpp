#include "BilliardTableActor.h"

#include "BilliardBallActor.h"
#include "BilliardGameMode8Ball.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"

ABilliardTableActor::ABilliardTableActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABilliardTableActor::BeginPlay()
{
	Super::BeginPlay();

	if (!bAutoBindPocketTriggers)
	{
		return;
	}

	TArray<UBoxComponent*> BoxComponents;
	GetComponents<UBoxComponent>(BoxComponents);

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
		Box->OnComponentBeginOverlap.AddDynamic(this, &ABilliardTableActor::HandlePocketOverlap);
	}
}

void ABilliardTableActor::HandlePocketOverlap(
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
