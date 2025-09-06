#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

UCLASS()
class MAGIC_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UPROPERTY()
	class UHealthBar* HealthBarWidget;
	
	void SetHealthPercent(float Percent);
};
