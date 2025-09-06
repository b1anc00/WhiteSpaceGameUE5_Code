#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FSpellData.generated.h"

USTRUCT(BlueprintType)
struct FSpellData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	FString SpellName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float ManaCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	TSubclassOf<AActor> SpellActorClass;
};
