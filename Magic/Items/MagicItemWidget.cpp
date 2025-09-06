#include "Magic/Items/MagicItemWidget.h"
#include "Components/Image.h"

void UMagicItemWidget::SetTexture(UTexture2D* Texture)
{
	if (TextureImage && Texture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		TextureImage->SetBrush(Brush);
	}
}
