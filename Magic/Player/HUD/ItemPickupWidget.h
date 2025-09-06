#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemPickupWidget.generated.h"

class UTextBlock;

UCLASS()
class MAGIC_API UItemPickupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/* Объект привязки */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* P_ItemName;

	/* Действие */
	void SetItemName(const FString& ItemName);
};
