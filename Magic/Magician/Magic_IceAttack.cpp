#include "Magic/Magician/Magic_IceAttack.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/MainCharacter.h"

AMagic_IceAttack::AMagic_IceAttack()
{
	PrimaryActorTick.bCanEverTick = true;
	Ice = CreateDefaultSubobject<UNiagaraComponent>(FName("Ice"));
	Ice->SetupAttachment(GetRootComponent());

	IceBox = CreateDefaultSubobject<UBoxComponent>(FName("IceBox"));
	IceBox->SetupAttachment(Ice);
	IceBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	IceBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	IceBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Trace start"));
	TraceStart->SetupAttachment(Ice);
	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Trace end"));
	TraceEnd->SetupAttachment(Ice);

	Ice->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Ice->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AMagic_IceAttack::BeginPlay()
{
	Super::BeginPlay();
	
	AMainCharacter* Character = Cast<AMainCharacter>(GetOwner());
	if (Character)
	{
		Damage *= Character->DamageMultiplier;
	}
	IceBox->OnComponentBeginOverlap.AddDynamic(this, &AMagic_IceAttack::OnOverlaping);
	GetWorld()->GetTimerManager().SetTimer(DestroyTimer, this, &AMagic_IceAttack::DestroyIce, 5.f, false);
}

void AMagic_IceAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMagic_IceAttack::DestroyIce()
{
	Destroy();
}

void AMagic_IceAttack::OnOverlaping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
	FHitResult Hit;
	UKismetSystemLibrary::BoxTraceSingle(this,
		Start,
		End,
		FVector(259.f, 388.f, 230.f),
		TraceStart->GetComponentRotation(),
		TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hit,
		true);
	if (Hit.GetActor())
	{
		UGameplayStatics::ApplyDamage(
			Hit.GetActor(),
			Damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
			);
		IHitInterface* HitInterface = Cast<IHitInterface>(Hit.GetActor());
		if (HitInterface)
		{
			HitInterface->GetHit(Hit.ImpactPoint, GetOwner());
		}
		IgnoreActors.Add(Hit.GetActor());
	}
}

