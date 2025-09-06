#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyWeapon.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class MAGIC_API AEnemyWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	/* Конструктор */
	AEnemyWeapon();
	
	/* Переопределение */
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/* Компоненты */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;
	
	UPROPERTY(visibleAnywhere, Category = Weapon)
	UBoxComponent* WeaponBox;

	UPROPERTY(visibleAnywhere)
	USceneComponent* StartTrace;

	UPROPERTY(visibleAnywhere)
	USceneComponent* EndTrace;

	/* Параметры */
	UPROPERTY(EditAnywhere, Category = Weapons)
	float Damage = 20.f;
	
	/* Перекрытие */
	UFUNCTION()
	void BoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/* Действия */
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	
	FORCEINLINE UBoxComponent* GetWeaponBox() const {return WeaponBox;}

	/* Массив для игнора акторов */
	TArray<AActor*> IgnoreToActors;
	
};
