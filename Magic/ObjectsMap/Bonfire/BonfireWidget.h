#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BonfireWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class MAGIC_API UBonfireWidget : public UUserWidget
{
	GENERATED_BODY()

	/* Переопределение */
	virtual void NativeConstruct() override;

	/* Параметры для настройки */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelVigor;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelMind;

	/* Кнопки */
	UPROPERTY(meta = (BindWidget))
	UButton* VigorUpLevel;

	UPROPERTY(meta = (BindWidget))
	UButton* VigorDecLevel;

	UPROPERTY(meta = (BindWidget))
	UButton* MindUpLevel;

	UPROPERTY(meta = (BindWidget))
	UButton* MindDecLevel;

	UPROPERTY(meta = (BindWidget))
	UButton* Escape;

	/* Работа кнопок */
	UFUNCTION()
	void EscapeButton();

	UFUNCTION()
	void VigorUpButton();

	UFUNCTION()
	void VigorDecButton();

	UFUNCTION()
	void MindUpButton();

	UFUNCTION()
	void MindDecButton();
};
