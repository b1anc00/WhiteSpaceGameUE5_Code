#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FinalPortal.generated.h"

class ABoss;

UCLASS()
class MAGIC_API AFinalPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	AFinalPortal();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, Category = Portal)
	ABoss* Boss;

	UPROPERTY(EditAnywhere, Category = Portal)
	class UBoxComponent* PortalBox;

	UPROPERTY(EditAnywhere, Category = Portal)
	class UNiagaraComponent* BossPortal;
};
