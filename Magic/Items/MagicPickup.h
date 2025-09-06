#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicPickup.generated.h"

class UNiagaraComponent;
class USphereComponent;
class UTexture2D;
class UMagicItemWidget;

UCLASS()
class MAGIC_API AMagicPickup : public AActor
{
	GENERATED_BODY()
	
public:
	/* ����������� */
	AMagicPickup();

	/* ��������������� */
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	/* ���������� */
	UPROPERTY(EditAnywhere)
	UNiagaraComponent* ItemEffect;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	UTexture2D* MagicImage;

	/* ������ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	TSubclassOf<UMagicItemWidget> WidgetClass;

	UPROPERTY()
	UMagicItemWidget* PickupWidget;
	
	/* ���������� */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* �������������� */
	void ShowWidget();
};
