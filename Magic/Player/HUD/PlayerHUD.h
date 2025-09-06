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
	/* ����� ������ */
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UPlayerOverlay> PlayerOverlayClass;

	UPROPERTY()
	UPlayerOverlay* Overlay;

	/* ��������������� */
	virtual void BeginPlay() override;

	/* ��������� ������� ������ */
	FORCEINLINE UPlayerOverlay* GetPlayerOverlay() { return Overlay; }
};
