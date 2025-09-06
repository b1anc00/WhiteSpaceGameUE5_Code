#pragma once

#include "CoreMinimal.h"
#include "SQLiteDatabase.h"
#include "Magic/Player/HUD/InventoryItem.h"
#include "DatabaseManager.generated.h"

UCLASS()
class MAGIC_API UDatabaseManager : public UObject
{
	GENERATED_BODY()
public:
	/* Загрузка БД */
	FString DbPath;
	
	void InitDatabase();
	
	void CheckoutDatabase();
	
	void CreateTables();

	/* Загрузка данных */
	void InsertPlayerStats(float CurrentHealth, float CurrentMana, float LevelVigor, float LevelMind, float posX, float posY, float posZ);

	bool LoadPlayerStats(float& OutCurrentHealth, float& OutCurrentMana, float& OutLevelVigor, float& OutLevelMind, float& OutPosX, float& OutPosY, float& OutPosZ);

	bool AddItemToInvetory(const FString& ItemName);

	bool LoadInvetoryItems(TArray<FInventoryItem>& OutItems);

	bool RemoveItemFromInvetory(const int32 InItemID);

	bool InsertOrUpdateEnemyRecord(int32 ID, const FString& InBlueprintPath, float InposX, float InposY, float InposZ);
	
	/* Взаимодействие с акторами */
	void SpawnEnemies(UWorld* World);

	void RemoveEnemies();

};
