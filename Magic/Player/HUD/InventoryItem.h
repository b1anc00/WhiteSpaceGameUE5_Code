#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.generated.h"

enum class EItemType : uint8
{
	EIT_Healing UMETA(DisplayName = "Healing"),
	EIT_Mana UMETA(DisplayName = "Mana"),
	EIT_None UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	int32 ItemID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	int32 Quantity;
};

FORCEINLINE EItemType GetItemTypeFromName(const FString& InName)
{
	if (InName == TEXT("Estus"))
	{
		return EItemType::EIT_Healing;
	}
	if (InName == TEXT("ManaEstus"))
	{
		return EItemType::EIT_Mana;
	}
	return EItemType::EIT_None;
}