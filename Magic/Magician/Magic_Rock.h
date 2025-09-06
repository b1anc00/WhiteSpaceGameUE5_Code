#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Magic_Rock.generated.h"

UCLASS()
class MAGIC_API AMagic_Rock : public AActor
{
	GENERATED_BODY()

public:
	AMagic_Rock();

protected:
	/** Вызывается при начале игры */
	virtual void BeginPlay() override;

	/** Удаление объекта */
	UFUNCTION()
	void DestroyRock();

	/** Таймер для удаления объекта */
	FTimerHandle DestroyTimerHandle;

public:
	/** Вызывается каждый кадр */
	virtual void Tick(float DeltaTime) override;

	/** Компонент сетки */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/** Компонент движения */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

	/** Настройка начального движения */
	void SetupInitialMovement();	

	/* Компонент капсулы */
	UPROPERTY(EditAnywhere, Category = "Magic Components")
	class UCapsuleComponent* MagicBox;

	/* Перекрытие */
	UFUNCTION()
	void OnOverlaping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/* Компоненты трассировки */
	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceEnd;

	/* Значение урона */
	UPROPERTY(VisibleAnywhere, Category = "Magic Components")
	float Damage = 20.f;

	/* Акторы для игнорирования */
	TArray<AActor*> IgnoreActors;
};