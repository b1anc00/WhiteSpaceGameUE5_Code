#include "Magic_Rock.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Magic/Interfaces/HitInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Magic/Public/Player/MainCharacter.h"

AMagic_Rock::AMagic_Rock()
{
	PrimaryActorTick.bCanEverTick = true;
	// Создаём компонент статической сетки
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MagicBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MagicBox"));
	MagicBox->SetupAttachment(GetRootComponent());
	MagicBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MagicBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	MagicBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Trace start"));
	TraceStart->SetupAttachment(GetRootComponent());
	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Trace end"));
	TraceEnd->SetupAttachment(GetRootComponent());

	// Создаём компонент движения
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.6f;
	ProjectileMovement->Friction = 0.2f;
}

void AMagic_Rock::BeginPlay()
{
	Super::BeginPlay();

	// Настраиваем физику и коллизию
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComponent->GetBodyInstance()->SetMassOverride(250.f, true);
	
	AMainCharacter* Character = Cast<AMainCharacter>(GetOwner());
	if (Character)
	{
		Damage *= Character->DamageMultiplier;
	}
	
	MagicBox->OnComponentBeginOverlap.AddDynamic(this, &AMagic_Rock::OnOverlaping);

	// Настройка начального движения
	SetupInitialMovement();

	// Таймер для запуска удаления
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AMagic_Rock::DestroyRock, 15.f, false);
}

void AMagic_Rock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
		
}

void AMagic_Rock::SetupInitialMovement()
{
	if (MeshComponent)
	{
		FVector AngularVelocity(300.f, 200.f, 100.f); // Угловая скорость
		MeshComponent->SetPhysicsAngularVelocityInDegrees(AngularVelocity, false); // Вращение
		if (MeshComponent->IsSimulatingPhysics())
		{
			FVector LaunchDirection = GetActorForwardVector();
			float LaunchStrength = 2000.f;
			MeshComponent->AddImpulse(LaunchDirection * LaunchStrength, NAME_None, true);
		}
	}
}

void AMagic_Rock::OnOverlaping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = TraceStart->GetComponentLocation();
	const FVector End = TraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(Owner);
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.Add(Actor);
	}
	FHitResult Hitted;
	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, FVector(10.f, 65.f, 40.f), TraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, Hitted, true);
	if (Hitted.GetActor())
	{
		UGameplayStatics::ApplyDamage(
			Hitted.GetActor(),
			Damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
		
		IHitInterface* HitInterface = Cast<IHitInterface>(Hitted.GetActor());
		if (HitInterface)
		{
			HitInterface->GetHit(Hitted.ImpactPoint, GetOwner());
		}
		IgnoreActors.AddUnique(Hitted.GetActor());
	}
}

void AMagic_Rock::DestroyRock()
{
	Destroy(); // Удаляем объект из мира
}

