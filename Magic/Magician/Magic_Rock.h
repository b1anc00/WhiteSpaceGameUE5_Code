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
	/** ���������� ��� ������ ���� */
	virtual void BeginPlay() override;

	/** �������� ������� */
	UFUNCTION()
	void DestroyRock();

	/** ������ ��� �������� ������� */
	FTimerHandle DestroyTimerHandle;

public:
	/** ���������� ������ ���� */
	virtual void Tick(float DeltaTime) override;

	/** ��������� ����� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/** ��������� �������� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

	/** ��������� ���������� �������� */
	void SetupInitialMovement();	

	/* ��������� ������� */
	UPROPERTY(EditAnywhere, Category = "Magic Components")
	class UCapsuleComponent* MagicBox;

	/* ���������� */
	UFUNCTION()
	void OnOverlaping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/* ���������� ����������� */
	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceEnd;

	/* �������� ����� */
	UPROPERTY(VisibleAnywhere, Category = "Magic Components")
	float Damage = 20.f;

	/* ������ ��� ������������� */
	TArray<AActor*> IgnoreActors;
};