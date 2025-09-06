#include "Magic/Player/Enemies/FinalPortal.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Magic/Public/Player/Enemies/Boss.h"

AFinalPortal::AFinalPortal()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRoot = nullptr;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;
	PortalBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	PortalBox->SetupAttachment(SceneRoot);
	BossPortal = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	BossPortal->SetupAttachment(PortalBox);
}

void AFinalPortal::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFinalPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> Enemies;
	
	// Получаем всех акторов класса AEnemy в текущем мире
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoss::StaticClass(), Enemies);

	// Перебираем массив и вызываем метод Die для каждого врага
	for (AActor* Actor : Enemies)
	{
		ABoss* Enemy = Cast<ABoss>(Actor);
		if (Enemy)
		{
			Boss = Enemy;
		}
	}
	
	if (Boss)
	{
		if (Boss->DeathState == EDeathState::EDS_Dead)
		{
			SetActorHiddenInGame(false);
			SetActorEnableCollision(true);
		}
		else
		{
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
		}
	}
}

