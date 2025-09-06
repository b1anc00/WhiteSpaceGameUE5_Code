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
	/** UI �������� */
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CanvasPanel;

	/** ������� (Niagara) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	class UNiagaraSystem* PointEmitterClass;

	TArray<class UNiagaraComponent*> ActiveEmitters;

	/** ������ ��� ��������� */
	TArray<FDrawingTemplate> LoadedTemplates;
	TArray<FVector2D> DrawnPoints;
	float NormalizationFactor = 0.01f;

	FString RecognizedTemplateName;

	/** ���������� ������ � ��������� */
	void CreateGrid(int32 Rows, int32 Columns);
	void RemoveEmitters();
	void UpdateEmitters();

	/** ������ � ���������� */
	class UNiagaraComponent* FindNearestEmitter(const FVector2D& MousePosition);
	void NormalizeDrawnPoints();
	void SetDrawnPoints(const TArray<FVector2D>& Points);

	/** ���������� � �������� �������� */
	void LoadDrawingTemplates(const FString& FolderPath);
	void SaveTemplate(const FString& TemplateName);

	/** ������������� �������� */
	TArray<FString> RecognizeDrawing();

	/** ��������� �������� ������������� ������� */
	UFUNCTION(BlueprintCallable, Category = Drawing)
	FString GetRecognizedTemplateName() const { return RecognizedTemplateName; }
};