#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

class UDatabaseManager;

UCLASS()
class MAGIC_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	/* Объекты */
	UPROPERTY()
	UDatabaseManager* DatabaseManager;
	
	/* Переопределение функций */
	virtual void Init() override;
};
