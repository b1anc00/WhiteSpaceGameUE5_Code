#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossWall.generated.h"

class AMainCharacter;
class UBoxComponent;
class UNiagaraComponent;

UCLASS()
class MAGIC_API ABossWall : public AActor
{
	GENERATED_BODY()
	
public:
	/* Конструктор */
	ABossWall();

	/* Переопределения */
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	/* Компоненты */
	UPROPERTY(EditAnywhere)
	UNiagaraComponent* Effect;

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent;

	/* Перекрытие */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OpenBossLevel(AMainCharacter* Character);
};
