#include "Magic/ObjectsMap/BossWall.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Player/MainCharacter.h"

ABossWall::ABossWall()
{
	PrimaryActorTick.bCanEverTick = true;

	Effect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));
	RootComponent = Effect;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(Effect);
}

void ABossWall::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABossWall::OnSphereOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ABossWall::OnSphereOverlapEnd);
}

void ABossWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossWall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player)
	{
		Player->bBossWall = true;
		Player->BossWall = this;
		Player->ShowInteractionWidget();
	}
}

void ABossWall::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player)
	{
		Player->bBossWall = false;
		Player->BossWall = nullptr;
		Player->HideInteractionWidget();
	}
}

void ABossWall::OpenBossLevel(AMainCharacter* Character)
{
	FHitResult Hit;
	Character->SetActorLocation(FVector(-55310.0f, 2090.0f, -3155.0f), false, &Hit, ETeleportType::TeleportPhysics);
}
