#include "Magic/Database/MyGameInstance.h"

#include "GameFramework/GameUserSettings.h"
#include "Magic/Database/DatabaseManager.h"

void UMyGameInstance::Init()
{
	Super::Init();

	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (Settings)
	{
		Settings->SetScreenResolution(FIntPoint(1920, 1080));
		Settings->SetFullscreenMode(EWindowMode::Windowed);
		Settings->ApplySettings(false);
	}
	
	if (!DatabaseManager)
	{
		DatabaseManager = NewObject<UDatabaseManager>(this, UDatabaseManager::StaticClass());
	}
	DatabaseManager->InitDatabase();
	DatabaseManager->CheckoutDatabase();
	DatabaseManager->CreateTables();
}
