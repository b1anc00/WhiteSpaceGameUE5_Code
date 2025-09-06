#include "Magic/Player/HUD/PlayerHUD.h"
#include "Magic/Player/HUD/PlayerOverlay.h"

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && PlayerOverlayClass)
		{
			Overlay = CreateWidget<UPlayerOverlay>(Controller, PlayerOverlayClass);
			Overlay->AddToViewport();
		}
	}
}
