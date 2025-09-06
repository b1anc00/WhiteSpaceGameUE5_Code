#include "Magic/Database/DatabaseManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "Magic/Player/HUD/InventoryItem.h"
#include "Player/Enemies/Enemy.h"
#include "Engine/GameEngine.h"
#include "Player/Enemies/Boss.h"

void UDatabaseManager::CheckoutDatabase()
{
	FSQLiteDatabase Database;
	if (Database.Open(*DbPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Database opened"));
		GEngine->AddOnScreenDebugMessage(0, 5, FColor::Red, FString::Printf(TEXT("Database opened: %s"), *DbPath));
		UE_LOG(LogTemp, Warning, TEXT(">> Runtime DBPath = %s"), *DbPath);
		Database.Close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Database not opened"));
		Database.Close();
	}
}

void UDatabaseManager::CreateTables()
{
	FSQLiteDatabase Database;
	if (Database.Open(*DbPath))
	{
		// Массив запросов для создания таблиц
		TArray<FString> Queries;
		Queries.Add(TEXT("CREATE TABLE IF NOT EXISTS Player (Id INTEGER PRIMARY KEY AUTOINCREMENT, CurrentHealth REAL NOT NULL, CurrentMana REAL NOT NULL, LevelVigor REAL NOT NULL, LevelMind REAL NOT NULL, posX REAL, posY REAL, posZ REAL);"));
		Queries.Add(TEXT("CREATE TABLE IF NOT EXISTS Items (id INTEGER PRIMARY KEY AUTOINCREMENT, iName TEXT, iCategory TEXT);"));
		Queries.Add(TEXT("CREATE TABLE IF NOT EXISTS Inventory (item_id INTEGER, quantity INTEGER, FOREIGN KEY (item_id) REFERENCES Items(id));"));
		Queries.Add(TEXT("CREATE TABLE IF NOT EXISTS Enemies (id INTEGER PRIMARY KEY AUTOINCREMENT, blueprintPath TEXT, posX REAL, posY REAL, posZ REAL);"));

		// Выполнение каждого запроса по отдельности
		for (const FString& Query : Queries)
		{
			if (!Database.Execute(*Query))
			{
				UE_LOG(LogTemp, Warning, TEXT("Ошибка выполнения запроса: %s"), *Query);
			}
		}

		// Добавляем стартовые предметы, только если их ещё нет
		const FString InsertEstus = 
			TEXT("INSERT INTO Items (iName, iCategory) ")
			TEXT("SELECT 'Estus','Item' ")
			TEXT("WHERE NOT EXISTS (SELECT 1 FROM Items WHERE iName='Estus');");

		const FString InsertManaEstus = 
			TEXT("INSERT INTO Items (iName, iCategory) ")
			TEXT("SELECT 'ManaEstus','Item' ")
			TEXT("WHERE NOT EXISTS (SELECT 1 FROM Items WHERE iName='ManaEstus');");

		if (!Database.Execute(*InsertEstus))
		{
			UE_LOG(LogTemp, Warning, TEXT("Не удалось вставить Estus"));
		}
		if (!Database.Execute(*InsertManaEstus))
		{
			UE_LOG(LogTemp, Warning, TEXT("Не удалось вставить ManaEstus"));
		}

		
		Database.Close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ошибка открытия базы данных"));
	}
}

void UDatabaseManager::InsertPlayerStats(float CurrentHealth, float CurrentMana, float LevelVigor, float LevelMind, float posX, float posY, float posZ)
{
	FSQLiteDatabase Database;
	if (Database.Open(*DbPath))
	{
		FString Query = FString::Printf(TEXT("INSERT INTO Player (CurrentHealth, CurrentMana, LevelVigor, LevelMind, posX, posY, posZ) VALUES (%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f);"), 
										CurrentHealth, CurrentMana, LevelVigor, LevelMind, posX, posY, posZ);
		if (!Database.Execute(*Query))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Stats not executed"));
		}
		Database.Close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Database error"));
	}
}

bool UDatabaseManager::LoadPlayerStats(float& OutCurrentHealth, float& OutCurrentMana, float& OutLevelVigor, float& OutLevelMind, float& OutPosX, float& OutPosY,
	float& OutPosZ)
{
	FSQLiteDatabase Database;
	if (!Database.Open(*DbPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Database not opened: %s"), *DbPath);
		return false;
	}

	bool bLoaded = false;
	// SQL-запрос: получаем первые попавшиеся записи по здоровью, мане и координатам
	FString Query = TEXT("SELECT CurrentHealth, CurrentMana, LevelVigor, LevelMind, posX, posY, posZ FROM Player ORDER BY Id DESC LIMIT 1;");

	// Выполняем запрос через Execute с лямбда-функцией (callback).
	Database.Execute(*Query, 
		[&](const FSQLitePreparedStatement& Statement) -> ESQLitePreparedStatementExecuteRowResult
		{
			// Попробуем получить все нужные поля из результата
			bool bSuccess = true;
			bSuccess &= Statement.GetColumnValueByIndex(0, OutCurrentHealth);
			bSuccess &= Statement.GetColumnValueByIndex(1, OutCurrentMana);
			bSuccess &= Statement.GetColumnValueByIndex(2, OutLevelVigor);
			bSuccess &= Statement.GetColumnValueByIndex(3, OutLevelMind);
			bSuccess &= Statement.GetColumnValueByIndex(4, OutPosX);
			bSuccess &= Statement.GetColumnValueByIndex(5, OutPosY);
			bSuccess &= Statement.GetColumnValueByIndex(6, OutPosZ);

			if (!bSuccess)
			{
				UE_LOG(LogTemp, Error, TEXT("no one of string (CurrentHealth, CurrentMana, posX, posY, posZ)."));
			}
			else
			{
				bLoaded = true;
			}

			// Возвращаем Break, чтобы прекратить чтение после первой строки
			return ESQLitePreparedStatementExecuteRowResult::Stop;
		}
	);

	Database.Close();
	return bLoaded;
}

bool UDatabaseManager::AddItemToInvetory(const FString& ItemName)
{
	// 1) Открываем базу данных
	FSQLiteDatabase Database;
	if (!Database.Open(*DbPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Database not opened: %s"), *DbPath);
		return false;
	}

	// 2) Ищем ID предмета по его имени в таблице Items
	int32 FoundItemId = -1;
	FString SelectQuery = FString::Printf(TEXT("SELECT id FROM Items WHERE iName = '%s';"), *ItemName);

	// Выполняем запрос: если хотя бы одна запись найдена, калбэк сработает один раз
	Database.Execute(*SelectQuery, 
		[&](const FSQLitePreparedStatement& Statement) -> ESQLitePreparedStatementExecuteRowResult
		{
			// Извлекаем id из первого (и единственного) столбца
			Statement.GetColumnValueByIndex(0, FoundItemId);
			// Прекращаем чтение, так как нам нужен только первый найденный id
			return ESQLitePreparedStatementExecuteRowResult::Stop;
		}
	);

	// Если FoundItemId так и остался -1, значит предмет не найден
	if (FoundItemId == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item '%s' not found in Items table."), *ItemName);
		Database.Close();
		return false;
	}

	int32 ExistingCount = 0;
	FString CheckQuery = FString::Printf(TEXT("SELECT quantity FROM Inventory WHERE Item_id = %d;"), FoundItemId);
	Database.Execute(*CheckQuery,
		[&](const FSQLitePreparedStatement& Statement) -> ESQLitePreparedStatementExecuteRowResult
		{
			Statement.GetColumnValueByIndex(0, ExistingCount);
			return ESQLitePreparedStatementExecuteRowResult::Stop;
		}
	);

	if (ExistingCount > 0)
	{
		// (3a) Если предмет уже есть, увеличиваем количество
		FString UpdateQuery = FString::Printf(TEXT("UPDATE Inventory SET quantity = quantity + 1 WHERE Item_id = %d;"), FoundItemId);
		Database.Execute(*UpdateQuery,
			[&](const FSQLitePreparedStatement& Statement) -> ESQLitePreparedStatementExecuteRowResult
			{
				return ESQLitePreparedStatementExecuteRowResult::Stop;
			}
		);
	}
	else
	{
		// (3b) Если предмета ещё нет, вставляем новую запись
		FString InsertQuery = FString::Printf(TEXT("INSERT INTO Inventory (Item_id, quantity) VALUES (%d, 1);"), FoundItemId);
		Database.Execute(*InsertQuery,
			[&](const FSQLitePreparedStatement& Statement) -> ESQLitePreparedStatementExecuteRowResult
			{
				return ESQLitePreparedStatementExecuteRowResult::Stop;
			}
		);
	}

	Database.Close();
	return true;
}

bool UDatabaseManager::LoadInvetoryItems(TArray<FInventoryItem>& OutItems)
{
	OutItems.Empty();
	FSQLiteDatabase Database;
	if (!Database.Open(*DbPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Database not opened: %s"), *DbPath);
		return false;
	}
	FString Query = TEXT("SELECT Items.id, Items.iName, Inventory.quantity FROM Inventory JOIN Items ON Items.id = Inventory.item_id;");
	bool bAnyRow = false;
	Database.Execute(*Query,
		[&](const FSQLitePreparedStatement& Statement)->ESQLitePreparedStatementExecuteRowResult
		{
			bAnyRow = true;
			FInventoryItem NewItem;
			Statement.GetColumnValueByIndex(0, NewItem.ItemID);
			Statement.GetColumnValueByIndex(1, NewItem.Name);
			Statement.GetColumnValueByIndex(2, NewItem.Quantity);
			OutItems.Add(NewItem);
			return ESQLitePreparedStatementExecuteRowResult::Continue;
		});
	Database.Close();
	if (!bAnyRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("No inventory items found."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory %d found."), OutItems.Num());
	}
	return true;
}

bool UDatabaseManager::RemoveItemFromInvetory(const int32 InItemID)
{
	FSQLiteDatabase Database;
	if (!Database.Open(*DbPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Database not opened: %s"), *DbPath);
		return false;
	}
	int32 ExistingCount = 0;
	{
		FString CheckQuery = FString::Printf(TEXT("SELECT quantity FROM Inventory WHERE item_id = %d;"), InItemID);
		Database.Execute(*CheckQuery,
			[&](const FSQLitePreparedStatement& Statement)->ESQLitePreparedStatementExecuteRowResult
			{
				Statement.GetColumnValueByIndex(0, ExistingCount);
				return ESQLitePreparedStatementExecuteRowResult::Stop;
			});
	}
	if (ExistingCount > 1)
	{
		FString UpdateQuery = FString::Printf(TEXT("UPDATE Inventory SET quantity = quantity - 1 WHERE item_id = %d;"), InItemID);
		Database.Execute(*UpdateQuery,
			[&](const FSQLitePreparedStatement& Statement) -> ESQLitePreparedStatementExecuteRowResult
			{
				return ESQLitePreparedStatementExecuteRowResult::Stop;
			}
		);
	}
	else
	{
		FString DeleteQuery = FString::Printf(TEXT("DELETE FROM Inventory WHERE item_id = %d;"), InItemID);
		Database.Execute(*DeleteQuery,
			[&](const FSQLitePreparedStatement& Statement) -> ESQLitePreparedStatementExecuteRowResult
			{
				return ESQLitePreparedStatementExecuteRowResult::Stop;
			}
		);
	}
	Database.Close();
	return true;
}

bool UDatabaseManager::InsertOrUpdateEnemyRecord(int32 ID, const FString& InBlueprintPath, float InposX, float InposY, float InposZ)
{
	FSQLiteDatabase Database;
	if (!Database.Open(*DbPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Database not opened: %s"), *DbPath);
		return false;
	}
	bool bRecordExists = false;
	{
		FString CheckQuery = FString::Printf(TEXT("SELECT id FROM Enemies WHERE id = %d"), ID);
		Database.Execute(*CheckQuery,
			[&](const FSQLitePreparedStatement& Statement)->ESQLitePreparedStatementExecuteRowResult
			{
				bRecordExists = true;
				return ESQLitePreparedStatementExecuteRowResult::Stop;
			});
	}
	bool bSuccess = false;
	if (bRecordExists)
	{
		FString UpdateQuery = FString::Printf(TEXT("UPDATE Enemies SET blueprintPath = '%s', posX = %.2f, posY = %.2f, posZ = %.2f WHERE id = %d"), *InBlueprintPath, InposX, InposY, InposZ, ID);
		Database.Execute(*UpdateQuery);
	}
	else
	{
		FString InsertQuery = FString::Printf(
			TEXT("INSERT INTO Enemies (id, blueprintPath, posX, posY, posZ) VALUES (%d, '%s', %.2f, %.2f, %.2f);"),
			ID, *InBlueprintPath, InposX, InposY, InposZ
		);
		bSuccess = Database.Execute(*InsertQuery);
	}
	if (!bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to upsert enemy record with ID=%d"), ID);
	}
	Database.Close();
	return bSuccess;
}

void UDatabaseManager::SpawnEnemies(UWorld* World)
{
	RemoveEnemies();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is null"));
		return;
	}
	FSQLiteDatabase Database;
	if (!Database.Open(*DbPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Database not opened"));
		return;
	}
	FString Query = FString::Printf(TEXT("SELECT id, blueprintPath, posX, posY, posZ FROM Enemies;"));
	Database.Execute(*Query,
		[&](const FSQLitePreparedStatement& Statement)->ESQLitePreparedStatementExecuteRowResult
		{
			int32 EnemyID;
			FString BlueprintPath;
			float x, y, z;
			Statement.GetColumnValueByIndex(0, EnemyID);
			Statement.GetColumnValueByIndex(1, BlueprintPath);
			Statement.GetColumnValueByIndex(2, x);
			Statement.GetColumnValueByIndex(3, y);
			Statement.GetColumnValueByIndex(4, z);
			UE_LOG(LogTemp, Warning, TEXT("Spawning Enemy"));
			UClass* EnemyClass = LoadObject<UClass>(nullptr, *BlueprintPath);
			if (!EnemyClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load enemy BP: %s"), *BlueprintPath);
				return ESQLitePreparedStatementExecuteRowResult::Continue;
			}
			FVector SpawnLocation(x, y, z);
			FRotator SpawnRotation(0.f, 0.f, 0.f);
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AActor* SpawnEnemy = World->SpawnActor<AActor>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (!SpawnEnemy)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to spawn enemy form %s"), *BlueprintPath);
			}
			return ESQLitePreparedStatementExecuteRowResult::Continue;
		});
	TArray<AActor*> Enemies;

	// Получаем всех акторов класса AEnemy в текущем мире
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoss::StaticClass(), Enemies);

	// Перебираем массив и вызываем метод Die для каждого врага
	for (AActor* Actor : Enemies)
	{
		ABoss* Enemy = Cast<ABoss>(Actor);
		if (Enemy)
		{
			Enemy->DeathState = EDeathState::EDS_Alive;
		}
	}
	Database.Close();
}

void UDatabaseManager::RemoveEnemies()
{
	TArray<AActor*> Enemies;

	// Получаем всех акторов класса AEnemy в текущем мире
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);

	// Перебираем массив и вызываем метод Die для каждого врага
	for (AActor* Actor : Enemies)
	{
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		if (Enemy)
		{
			Enemy->Die();
		}
	}
}

void UDatabaseManager::InitDatabase()
{
	// Собираем полный путь: <Project>/Saved/Database.db
	const FString SaveDir = FPaths::ProjectSavedDir();
	const FString FileName = TEXT("Database.db");
	const FString FullPath = FPaths::Combine(*SaveDir, *FileName);

	// Если файла ещё нет — создаём его «заготовку»
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FullPath))
	{
		// Создаём пустой файл
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		// Создаём папку, если вдруг её нет
		PlatformFile.CreateDirectoryTree(*SaveDir);
		// Открываем для записи: при закрытии будет пустой файл нужного размера
		TUniquePtr<IFileHandle> FileHandle(PlatformFile.OpenWrite(*FullPath));
		if (FileHandle)
		{
			// Ничего не пишем — просто «трогаем» файл
			FileHandle.Reset();
			UE_LOG(LogTemp, Log, TEXT("Created new database file: %s"), *FullPath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create database file: %s"), *FullPath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Database file already exists: %s"), *FullPath);
	}

	// Сохраняем путь в свойство
	DbPath = FullPath;
}

