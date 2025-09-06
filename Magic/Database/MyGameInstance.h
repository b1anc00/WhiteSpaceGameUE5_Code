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
	/* ������� */
	UPROPERTY()
	UDatabaseManager* DatabaseManager;
	
	/* ��������������� ������� */
	virtual void Init() override;
};
