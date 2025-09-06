#include "Magic/Player/HUD/ItemPickupWidget.h"
#include "Components/TextBlock.h"

void UItemPickupWidget::SetItemName(const FString& ItemName)
{
	if (P_ItemName)
	{
		P_ItemName->SetText(FText::FromString(ItemName));
	}
}
