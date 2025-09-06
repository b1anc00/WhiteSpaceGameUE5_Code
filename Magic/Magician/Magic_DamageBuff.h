#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Magic_DamageBuff.generated.h"

class UNiagaraSystem;

UCLASS()
class MAGIC_API AMagic_DamageBuff : public AActor
{
	GENERATED_BODY()
	
public:
	/* Конструктор */
	AMagic_DamageBuff();

	/* Переопределение */
	virtual void BeginPlay() override;

	/* Эффекты */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	UNiagaraSystem* NiagaraSystem;
	
	/* Параметры */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	float AmountBuff = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	float DurationBuff = 10.f;

	/* Таймер */
	FTimerHandle TimerHandle;
	
	/* Применение баффа */
	void ApplyBuff();

	void RemoveBuff();
};
