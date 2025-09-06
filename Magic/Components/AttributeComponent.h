#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAGIC_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/* ����������� */
	UAttributeComponent();

	/* ��������������� ������ */
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* ��������� */
	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	float CurrentHealth;
	
	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	float CurrentMind;
	
	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	float MaxMind;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Vigor;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Mind;
	
	/* ��������� �������� */
	void ReceiveDamage(float Damage);
	
	float GetHealthPercent();
	
	bool isAlive();
	
	void ReceiveMind(float Value);
	
	float GetMindPercent();
	
	bool isMindCan();
	
	/* �������������� �� ������� */
	void ResetStats();
	
	void CalcValueLevels();
	
	void CalcVigor();
	
	void CalcMind();
};