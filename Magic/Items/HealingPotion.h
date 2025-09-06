#pragma once

#include "CoreMinimal.h"
#include "Magic/Items/Items.h"
#include "HealingPotion.generated.h"


UCLASS()
class MAGIC_API AHealingPotion : public AItems
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Potion)
	float HealAmount = 50.f;
};
