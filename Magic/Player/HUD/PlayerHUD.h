#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UPlayerOverlay;

UCLASS()
class MAGIC_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
public:
	/* Класс вижета */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UPlayerOverlay> PlayerOverlayClass;

	UPROPERTY()
	UPlayerOverlay* Overlay;

	/* Переопределение */
	virtual void BeginPlay() override;

	/* Получение оверлея игрока */
	FORCEINLINE UPlayerOverlay* GetPlayerOverlay() { return Overlay; }
};
