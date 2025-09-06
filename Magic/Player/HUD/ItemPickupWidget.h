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
	/* ������ �������� */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* P_ItemName;

	/* �������� */
	void SetItemName(const FString& ItemName);
};
