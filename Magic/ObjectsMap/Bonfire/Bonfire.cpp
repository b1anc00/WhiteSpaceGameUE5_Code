#include "Magic/ObjectsMap/Bonfire/Bonfire.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Magic/Public/Player/MainCharacter.h"

ABonfire::ABonfire()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	BonFireMesh = CreateDefaultSubobject<UStaticMeshComponent>("BonfireMesh");
	BonFireMesh->SetupAttachment(RootComponent);
	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>("SwordMesh");
	SwordMesh->SetupAttachment(BonFireMesh);
	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetGenerateOverlapEvents(true);
}

void ABonfire::BeginPlay()
{
	Super::BeginPlay();
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ABonfire::OnSphereOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &ABonfire::OnSphereOverlapEnd);
}

void ABonfire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABonfire::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player)
	{
		Player->bCanSitBonfire = true;
		Player->ShowInteractionWidget();
	}
}

void ABonfire::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player)
	{
		Player->bCanSitBonfire = false;
		Player->HideInteractionWidget();
	}
}
