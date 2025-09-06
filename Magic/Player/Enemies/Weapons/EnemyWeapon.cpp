#include "Magic/Player/Enemies/Weapons/EnemyWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Magic/Interfaces/HitInterface.h"

AEnemyWeapon::AEnemyWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	StartTrace = CreateDefaultSubobject<USceneComponent>(TEXT("StartTrace"));
	StartTrace->SetupAttachment(GetRootComponent());
	EndTrace = CreateDefaultSubobject<USceneComponent>(TEXT("EndTrace"));
	EndTrace->SetupAttachment(GetRootComponent());
}

void AEnemyWeapon::BeginPlay()
{
	Super::BeginPlay();
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyWeapon::BoxOverlap);
}

void AEnemyWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);;
	SetInstigator(NewInstigator);
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AEnemyWeapon::BoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = StartTrace->GetComponentLocation();
	const FVector End = EndTrace->GetComponentLocation();

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	IgnoreActors.Add(Owner);
	for (AActor* Actor : IgnoreToActors)
	{
		IgnoreActors.AddUnique(Actor);
	}
	FHitResult WeaponHit;
	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, FVector(5.f, 5.f, 5.f), StartTrace->GetComponentRotation(),  ETraceTypeQuery::TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::None, WeaponHit, true);
	if (WeaponHit.GetActor())
	{
		UGameplayStatics::ApplyDamage(WeaponHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		IHitInterface* HitInterface = Cast<IHitInterface>(WeaponHit.GetActor());
		if (HitInterface)
		{
			HitInterface->GetHit(WeaponHit.ImpactPoint, GetOwner());
		}
		IgnoreToActors.AddUnique(WeaponHit.GetActor());
	}
}
