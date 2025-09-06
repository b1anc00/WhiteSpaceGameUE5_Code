#include "Magic/Player/HUD/PlayerOverlay.h"
#include "Components/ProgressBar.h"
#include "Magic/Player/HUD/InventoryItem.h"
#include "Components/TextBlock.h"

void UPlayerOverlay::SetHealthPercent(float Percent)
{
	if (HealthProgressbar)
	{
		HealthProgressbar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetMindPercent(float Percent)
{
	if (MindProgressbar)
	{
		MindProgressbar->SetPercent(Percent);
	}
}

void UPlayerOverlay::InitializeInventory(const TArray<FInventoryItem>& InItems)
{
	InventoryItems = InItems;
	if (InventoryItems.Num() > 0)
	{
		CurrentIndex = 0;
		UpdateActiveItem();
	}
	else
	{
		CurrentIndex = -1;
		if (ActiveItem)
		{
			ActiveItem->SetText(FText::FromString(""));
		}
		if (Quantity)
		{
			Quantity->SetText(FText::FromString(""));
		}
	}
}

void UPlayerOverlay::ShowNextItemInventory()
{
	if (InventoryItems.Num() == 0) return;
	// —мещаем индекс по модулю
	CurrentIndex = (CurrentIndex + 1) % InventoryItems.Num();
	UpdateActiveItem();
}

void UPlayerOverlay::ShowPreviousItemInventory()
{
	if (InventoryItems.Num() == 0) return;
	// ”ходим на -1, но добавл€ем размер массива, чтобы не выйти в отрицательный индекс
	CurrentIndex = (CurrentIndex - 1 + InventoryItems.Num()) % InventoryItems.Num();
	UpdateActiveItem();
}

void UPlayerOverlay::UpdateActiveItem()
{
	if (InventoryItems.IsValidIndex(CurrentIndex))
	{
		const FInventoryItem& Item = InventoryItems[CurrentIndex];
		if (ActiveItem)
		{
			ActiveItem->SetText(FText::FromString(Item.Name));
		}
		if (Quantity)
		{
			Quantity->SetText(FText::AsNumber(Item.Quantity));
		}
	}
	else
	{
		// ≈сли массив пуст или индекс неверный
		if (ActiveItem)
		{
			ActiveItem->SetText(FText::FromString(TEXT("No Items")));
		}
		if (Quantity)
		{
			Quantity->SetText(FText::FromString(TEXT("0")));
		}
	}
}

FInventoryItem UPlayerOverlay::GetCurrentItem() const
{
	if (InventoryItems.IsValidIndex(CurrentIndex))
	{
		return InventoryItems[CurrentIndex];
	}
	FInventoryItem EmptyItem;
	EmptyItem.ItemID = -1;
	EmptyItem.Name = TEXT("");
	EmptyItem.Quantity = 0;
	return EmptyItem;
}
