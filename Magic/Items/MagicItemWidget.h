#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MagicItemWidget.generated.h"

class UTexture2D;
class UImage;

UCLASS()
class MAGIC_API UMagicItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetTexture(UTexture2D* Texture);

	UPROPERTY(meta = (BindWidget))
	UImage* TextureImage;
};
