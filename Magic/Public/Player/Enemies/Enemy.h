#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Magic/Interfaces/HitInterface.h"
#include "Enemy.generated.h"

/* Состояния врага */
UENUM(BlueprintType)
enum class EDeathState : uint8
{
	EDS_Alive UMETA(DisplayName = "Alive"),
	EDS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EE_Patrolling UMETA(DisplayName = "Patrolling"),
	EE_Chasing UMETA(DisplayName = "Chasing"),
	EE_Attacking UMETA(DisplayName = "Attacking"),
	EE_InBattle UMETA(DisplayName = "InBattle")
};

class UAnimMontage;
class AAIController;
class UPawnSensingComponent;

UCLASS()
class MAGIC_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	/** Конструктор */
	AEnemy();

	/** Переопределённые функции актора */
	virtual void Tick(float DeltaTime) override;
	
	virtual void BeginPlay() override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void Destroyed() override;

	/** Реализация интерфейса IHitInterface */
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;
	
	/** Монтажи анимаций */
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSections;

	/* Воспроизведение анимаций */
	void PlayHitReactMontage(const FName& SectionName);
	
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);

	/** Компоненты и виджеты */
	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	class UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSens;

	UPROPERTY(EditAnywhere, Category = Combat)
	int32 EnemyID;

	/** Параметры боя */
	UPROPERTY(VisibleAnywhere ,BlueprintReadOnly, Category = Combat)
	EDeathState DeathState = EDeathState::EDS_Alive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	bool bCanAttack = true;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTarget = 75.f;

	FTimerHandle AttackTimer;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.1f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 0.3f;

	/* Действия боя */
	void HitReact(const FVector& ImpactPoint);
	
	virtual void Die();
	
	bool inTargetRange(AActor* Target, double Radius);
	
	void MoveToTarget(AActor* Target);
	
	void CheckCombatTarget();

	UFUNCTION(BlueprintCallable)
	void EnableBoxCollision(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	FVector GetTranslaionWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	void Attack();
	
	void PlayAttackMontage();

	void StartAttackTimer();

	void ChaseTarget();

	void ClearTimerAttack();

	void AttackEnd();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	
	/** Параметры патрулирования */
	UPROPERTY(EditInstanceOnly, Category = NavigationAI)
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = NavigationAI)
	TArray<AActor*> PatrolTargets; 

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;

	UPROPERTY(VisibleAnywhere, Category = Combat)
	EEnemyState EnemyState = EEnemyState::EE_Patrolling;

	/* действия патрулирования */
	AActor* ChoosePatrolTarget();
	
	void PatrolTimerFinished();
	
	void CheckPatrolTarget();
	
	/** AI контроллер */
	AAIController* AIEnemyController;

	/* Оружие */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemyWeapon> WeaponClass;
	
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AEnemyWeapon* EquippedWeapon;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Combat)
	TSubclassOf<AActor> ItemToSpawn;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Combat)
	int32 CountItems = 1;
};
