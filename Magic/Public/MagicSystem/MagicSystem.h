#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MagicSystem.generated.h"

USTRUCT(BlueprintType)
struct FDrawingTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector2D> Points;

	UPROPERTY(BlueprintReadWrite)
	FString Name;
};

UCLASS()
class MAGIC_API UMagicSystem : public UUserWidget
{
	GENERATED_BODY()

public:
	/** UI Элементы */
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CanvasPanel;

	/** Эффекты (Niagara) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	class UNiagaraSystem* PointEmitterClass;

	TArray<class UNiagaraComponent*> ActiveEmitters;

	/** Данные для рисования */
	TArray<FDrawingTemplate> LoadedTemplates;
	TArray<FVector2D> DrawnPoints;
	float NormalizationFactor = 0.01f;

	FString RecognizedTemplateName;

	/** Управление сеткой и эффектами */
	void CreateGrid(int32 Rows, int32 Columns);
	void RemoveEmitters();
	void UpdateEmitters();

	/** Работа с рисованием */
	class UNiagaraComponent* FindNearestEmitter(const FVector2D& MousePosition);
	void NormalizeDrawnPoints();
	void SetDrawnPoints(const TArray<FVector2D>& Points);

	/** Сохранение и загрузка шаблонов */
	void LoadDrawingTemplates(const FString& FolderPath);
	void SaveTemplate(const FString& TemplateName);

	/** Распознавание рисунков */
	TArray<FString> RecognizeDrawing();

	/** Получение названия распознанного шаблона */
	UFUNCTION(BlueprintCallable, Category = Drawing)
	FString GetRecognizedTemplateName() const { return RecognizedTemplateName; }
};