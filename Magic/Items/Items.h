#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items.generated.h"

class UNiagaraComponent;
class USphereComponent;
class AMainCharacter;
class UItemPickupWidget;

UCLASS(Blueprintable)
class MAGIC_API AItems : public AActor
{
	GENERATED_BODY()
	
public:
	/* Конструктор */
	AItems();

	/* Переопределение */
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	/* Компоненты */
	UPROPERTY(EditAnywhere)
	UNiagaraComponent* ItemEffect;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	/* Виджет */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UItemPickupWidget> PickupWidgetClass;
	
	UPROPERTY()
	UItemPickupWidget* PickupWidget;
	
	/* Перекрытие */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* Взаимодействие */
	void Destroy();
};
