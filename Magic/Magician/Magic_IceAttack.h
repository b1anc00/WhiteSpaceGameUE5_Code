#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Magic_IceAttack.generated.h"

UCLASS()
class MAGIC_API AMagic_IceAttack : public AActor
{
	GENERATED_BODY()
	
public:
	AMagic_IceAttack();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void DestroyIce();

	FTimerHandle DestroyTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* Ice;
	
	UPROPERTY(EditAnywhere, Category = "Magic Components")
	class UBoxComponent* IceBox;

	UFUNCTION()
	void OnOverlaping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceEnd;

	UPROPERTY(VisibleAnywhere, Category = "Magic Components")
	float Damage = 60.f;

	TArray<AActor*> IgnoreActors;
};
