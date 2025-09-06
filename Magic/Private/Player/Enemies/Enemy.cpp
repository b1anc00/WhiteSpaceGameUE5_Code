#include "Player/Enemies/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Magic/Components/AttributeComponent.h"
#include "Magic/Components/HUD/HealthBarComponent.h"
#include "AIController.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Magic/Database/DatabaseManager.h"
#include "Magic/Database/MyGameInstance.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Player/MainCharacter.h"
#include "Magic/Player/Enemies/Weapons/EnemyWeapon.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	PawnSens = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSens->SightRadius = 3000.f;
	PawnSens->SetPeripheralVisionAngle(45.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	Tags.Add(FName("Enemy"));
	
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	if (GI && GI->DatabaseManager)
	{
		if (EnemyID < 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy %s invalid ID, not recorded"), *GetName());
			return;
		}
		FString PathName = GetClass()->GetPathName();
		FVector Loc = GetActorLocation();
		GI->DatabaseManager->InsertOrUpdateEnemyRecord(EnemyID, PathName, Loc.X, Loc.Y, Loc.Z);
	}
	
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
		Attributes->CalcValueLevels();
	}

	AIEnemyController = Cast<AAIController>(GetController());
	if (!PatrolTarget)
	{
		PatrolTarget = ChoosePatrolTarget();
	}
	MoveToTarget(PatrolTarget);
	if (PawnSens)
	{
		PawnSens->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AEnemyWeapon* DefaultWeapon = World->SpawnActor<AEnemyWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::CheckCombatTarget()
{
	if (!inTargetRange(CombatTarget, CombatRadius))
	{
		ClearTimerAttack();
		CombatTarget = nullptr;
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
		if (EnemyState != EEnemyState::EE_InBattle)
		{
			EnemyState = EEnemyState::EE_Patrolling;
			GetCharacterMovement()->MaxWalkSpeed = 150.f;
			MoveToTarget(PatrolTarget);
		}
	}
	else if (!inTargetRange(CombatTarget, AttackRadius) &&
		EnemyState != EEnemyState::EE_Chasing) //patrol outside attack
	{
		ClearTimerAttack();
		if (EnemyState != EEnemyState::EE_InBattle)
		{
			EnemyState = EEnemyState::EE_Chasing;
			GetCharacterMovement()->MaxWalkSpeed = 400.f;
			MoveToTarget(CombatTarget);
		}
	}
	else if (inTargetRange(CombatTarget, AttackRadius) &&
		EnemyState != EEnemyState::EE_Attacking &&
		DeathState != EDeathState::EDS_Dead &&
		bCanAttack == true)
	{
		StartAttackTimer();
	}
}

void AEnemy::CheckPatrolTarget() 
{
	if (inTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, 5.f);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (DeathState == EDeathState::EDS_Dead) return; 
	if (EnemyState > EEnemyState::EE_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{
	bCanAttack = true;
	EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
	HitReact(ImpactPoint);
}

void AEnemy::HitReact(const FVector& ImpactPoint)
{
	if (Attributes && Attributes->isAlive())
	{
		const FVector Forward = GetActorForwardVector();
		const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
		const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
		const double CosTheta = FVector::DotProduct(Forward, ToHit);
		double Theta = FMath::Acos(CosTheta);
		Theta = FMath::RadiansToDegrees(Theta);
		const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
		if (CrossProduct.Z < 0)
		{
			Theta *= -1.f;
		}
		FName Section("Back");
		if (Theta >= -45.f && Theta < 45.f)
		{
			Section = FName("Front");
		}
		else if (Theta >= -135.f && Theta < 45.f)
		{
			Section = FName("Left");
		}
		else if (Theta >= 45.f && Theta < 135.f)
		{
			Section = FName("Right");
		}
		PlayHitReactMontage(Section);
	}
	else
	{
		Die();
	}
	
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	AActor* DamageCauser)
{
	CombatTarget = EventInstigator->GetPawn();
	if (Attributes && HealthBarWidget)
	{
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
	ChaseTarget();
	return DamageAmount;
}

void AEnemy::Die()
{
	EnableBoxCollision(ECollisionEnabled::NoCollision);
	HealthBarWidget->SetVisibility(false);
	DeathState = EDeathState::EDS_Dead;
	ClearTimerAttack();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(5.f);
}

bool AEnemy::inTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget  <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (AIEnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(30.f);
	AIEnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
			
	const int32 NumOfPatrolTargets = ValidTargets.Num();
	if (NumOfPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumOfPatrolTargets - 1);
		return  ValidTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		DeathState != EDeathState::EDS_Dead &&
		EnemyState != EEnemyState::EE_Chasing &&
		EnemyState < EEnemyState::EE_Attacking &&
		SeenPawn->ActorHasTag(FName("Player"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		ChaseTarget();
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::Attack()
{
	PlayAttackMontage();
}

void AEnemy::PlayAttackMontage()
{
	if (AttackMontageSections.Num() <= 0) return;
	const int32 MaxSections = AttackMontageSections.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSections);
	PlayMontageSection(AttackMontage, AttackMontageSections[Selection]);
}

void AEnemy::StartAttackTimer()
{
	bCanAttack = false;
	EnemyState = EEnemyState::EE_Attacking;
	const float AttackTime = FMath::FRandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EE_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	MoveToTarget(CombatTarget);
}

void AEnemy::ClearTimerAttack()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EE_Chasing;
	MoveToTarget(CombatTarget);
	bCanAttack = true;
	CheckCombatTarget();
}

void AEnemy::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void AEnemy::EnableBoxCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreToActors.Empty();
	}
}

FVector AEnemy::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

FVector AEnemy::GetTranslaionWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();
	
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();
	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTarget;
	return CombatTargetLocation + TargetToMe;
}
