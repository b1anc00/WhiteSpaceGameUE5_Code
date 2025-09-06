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
	/* ����������� */
	AMagic_DamageBuff();

	/* ��������������� */
	virtual void BeginPlay() override;

	/* ������� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	UNiagaraSystem* NiagaraSystem;
	
	/* ��������� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	float AmountBuff = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	float DurationBuff = 10.f;

	/* ������ */
	FTimerHandle TimerHandle;
	
	/* ���������� ����� */
	void ApplyBuff();

	void RemoveBuff();
};
