#include "Player/Enemies/Boss.h"

#include "Kismet/GameplayStatics.h"
#include "Magic/Components/AttributeComponent.h"

ABoss::ABoss()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	Attributes->Vigor = 15.f;
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABoss::Die()
{
	Super::Die();
	UE_LOG(LogTemp, Warning, TEXT("Boss Died!"));
	DeathState = EDeathState::EDS_Dead;
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
