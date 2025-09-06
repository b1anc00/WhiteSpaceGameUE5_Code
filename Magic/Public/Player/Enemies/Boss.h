#pragma once

#include "CoreMinimal.h"
#include "Player/Enemies/Enemy.h"
#include "Boss.generated.h"

UCLASS()
class MAGIC_API ABoss : public AEnemy
{
	GENERATED_BODY()

	ABoss();

	virtual void BeginPlay() override;

	virtual void Die() override;

	virtual void Tick(float DeltaTime) override;
	
};
