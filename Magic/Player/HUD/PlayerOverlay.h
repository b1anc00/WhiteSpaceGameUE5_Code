#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Magic/Player/HUD/InventoryItem.h"
#include "PlayerOverlay.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class MAGIC_API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	/* ������� �������� */
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressbar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* MindProgressbar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ActiveItem;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Quantity;

	/* ����������� ��������� */
	UPROPERTY()
	TArray<FInventoryItem> InventoryItems;

	int32 CurrentIndex = 0;

	/* ��������� �������� */
	void SetHealthPercent(float Percent);

	void SetMindPercent(float Percent);

	UFUNCTION(BlueprintCallable)
	void InitializeInventory(const TArray<FInventoryItem>& InItems);

	void UpdateActiveItem();

	FInventoryItem GetCurrentItem() const;
	
	/* �������������� ������������ ��������� */
	UFUNCTION(BlueprintCallable)
	void ShowNextItemInventory();

	UFUNCTION(BlueprintCallable)
	void ShowPreviousItemInventory();


};