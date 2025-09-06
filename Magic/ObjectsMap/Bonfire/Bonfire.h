#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bonfire.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class MAGIC_API ABonfire : public AActor
{
	GENERATED_BODY()
	
public:
	/* ����������� */
	ABonfire();
	
	/* ��������������� */
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/* ���������� */
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BonFireMesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SwordMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* OverlapSphere;

	/* ���������� */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
