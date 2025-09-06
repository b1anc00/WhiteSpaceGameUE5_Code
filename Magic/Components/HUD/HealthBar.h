#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

UCLASS()
class MAGIC_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:
	/* Привязанные объекты */
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
};
