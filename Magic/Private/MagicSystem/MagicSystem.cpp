#include "MagicSystem/MagicSystem.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "NiagaraComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraFunctionLibrary.h"
#include "JsonUtilities.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "DesktopPlatformModule.h"
#include "JsonObjectConverter.h"

void UMagicSystem::CreateGrid(int32 Rows, int32 Columns)
{
	if (!CanvasPanel || !PointEmitterClass) return;
	FVector2D ViewportSize;
	if(GEngine)
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const float CellWidth = ViewportSize.X / Columns;
	const float CellHeight = ViewportSize.Y / Rows;
	AActor* OwnerActor = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!OwnerActor) return;
	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Col = 0; Col < Columns; ++Col)
		{
			const float X = Col * CellWidth + CellWidth / 2;
			const float Y = Row * CellHeight + CellHeight / 2;
			if (PointEmitterClass)
			{
				FVector WorldLocation;
				FVector WorldDirection;
				FVector2D ScreenPosition(X, Y);
				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
				if (PlayerController && PlayerController->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, WorldLocation, WorldDirection))
				{
					UNiagaraComponent* Emitter = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					PointEmitterClass,
					WorldLocation);
					if (Emitter)
					{
						Emitter->SetRelativeScale3D(FVector(1.f));
						ActiveEmitters.Add(Emitter);
					}
				}
			}
		}
	}
}

void UMagicSystem::RemoveEmitters()
{
	for (UNiagaraComponent* Emitter : ActiveEmitters)
	{
		if (Emitter)
		{
			Emitter->DestroyComponent();
		}
	}
	ActiveEmitters.Empty();
}

void UMagicSystem::UpdateEmitters()
{
	if (!PointEmitterClass || ActiveEmitters.Num() == 0) return;
	FVector2D ViewportSize;
	if(GEngine)
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const float CellWidth = ViewportSize.X / 15;
	const float CellHeight = ViewportSize.Y / 15;
	for (int32 Index = 0; Index < ActiveEmitters.Num(); ++Index)
	{
		if (ActiveEmitters[Index])
		{
			int32 Row = Index / 15;
			int32 Col = Index % 15;
			const float X = Col * CellWidth + CellWidth / 2;
			const float Y = Row * CellHeight + CellHeight / 2;
			FVector WorldLocation;
			FVector WorldDirection;
			FVector2D ScreenPosition(X, Y);
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController && PlayerController->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, WorldLocation, WorldDirection))
			{
				ActiveEmitters[Index]->SetWorldLocation(WorldLocation);
			}
		}
	}
}

UNiagaraComponent* UMagicSystem::FindNearestEmitter(const FVector2D& MousePosition)
{
	float MinDistance = FLT_MAX;
	UNiagaraComponent* NearestEmitter = nullptr;
	for (UNiagaraComponent* Emitter : ActiveEmitters)
	{
		if (!Emitter) continue;
		FVector2D EmitterScreenPosition;
		FVector WorldLocation = Emitter->GetComponentLocation();
		if (UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), WorldLocation, EmitterScreenPosition))
		{
			float Distance = FVector2D::Distance(MousePosition, EmitterScreenPosition);
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				NearestEmitter = Emitter;
			}
		}
	}
	return NearestEmitter;
}

void UMagicSystem::NormalizeDrawnPoints()
{
	FVector2D ViewportSize;
	if(GEngine)
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	for (FVector2D Point : DrawnPoints)
	{
		Point.X /= ViewportSize.X;
		Point.Y /= ViewportSize.Y;
	}
}

void UMagicSystem::LoadDrawingTemplates(const FString& FolderPath)
{
	TArray<FString> FileNames;
	IFileManager::Get().FindFiles(FileNames, *FolderPath, TEXT("*.json"));
	FVector2D ViewportSize;
	if(GEngine)
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	for (const FString& FileName : FileNames)
	{
		FString FilePath = FolderPath / FileName;
		FString JsonString;
		if (FFileHelper::LoadFileToString(JsonString, *FilePath))
		{
			FDrawingTemplate Template;
			if (FJsonObjectConverter::JsonObjectStringToUStruct<FDrawingTemplate>(JsonString, &Template, 0, 0))
			{
				for (FVector2D& Point : Template.Points)
				{
					Point.X /= ViewportSize.X;
					Point.Y /= ViewportSize.Y;
				}
				LoadedTemplates.Add(Template);
				UE_LOG(LogTemp, Warning, TEXT("Template %s loaded successfully with %d points"), *Template.Name, Template.Points.Num());
			}
		}
	}
}

TArray<FString> UMagicSystem::RecognizeDrawing()
{
	UE_LOG(LogTemp, Log, TEXT("Drawn points count: %d"), DrawnPoints.Num());

	TArray<FString> RecognizedTemplates;

	if (DrawnPoints.Num() == 0 || LoadedTemplates.Num() == 0)
	{
		return RecognizedTemplates;
	}

	FVector2D ViewportSize;
	if(GEngine)
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	// Нормализация нарисованных точек
	TArray<FVector2D> NormalizedDrawnPoints;
	for (const FVector2D& DrawnPoint : DrawnPoints)
	{
		NormalizedDrawnPoints.Add(FVector2D(
			DrawnPoint.X / ViewportSize.X,
			DrawnPoint.Y / ViewportSize.Y
		));
	}

	// Центрирование нарисованных точек
	FVector2D DrawnCenter = FVector2D::ZeroVector;
	for (const FVector2D& Point : NormalizedDrawnPoints)
	{
		DrawnCenter += Point;
	}
	DrawnCenter /= NormalizedDrawnPoints.Num();
	for (FVector2D& Point : NormalizedDrawnPoints)
	{
		Point -= DrawnCenter;
	}

	// Сравнение с шаблонами
	for (const FDrawingTemplate& Template : LoadedTemplates)
	{
		TArray<FVector2D> NormalizedTemplatePoints = Template.Points;

		FVector2D TemplateCenter = FVector2D::ZeroVector;
		for (const FVector2D& Point : NormalizedTemplatePoints)
		{
			TemplateCenter += Point;
		}
		TemplateCenter /= NormalizedTemplatePoints.Num();
		for (FVector2D& Point : NormalizedTemplatePoints)
		{
			Point -= TemplateCenter;
		}

		float TotalDistance = 0.f;
		for (const FVector2D& DrawnPoint : NormalizedDrawnPoints)
		{
			float MinDistance = FLT_MAX;
			for (const FVector2D& TemplatePoint : NormalizedTemplatePoints)
			{
				float Distance = FVector2D::Distance(DrawnPoint, TemplatePoint);
				MinDistance = FMath::Min(MinDistance, Distance);
			}
			TotalDistance += MinDistance;
		}

		float MatchScore = TotalDistance / NormalizedDrawnPoints.Num();
		if (MatchScore < NormalizationFactor)
		{
			RecognizedTemplates.Add(Template.Name);
			UE_LOG(LogTemp, Log, TEXT("Recognized Template: %s"), *Template.Name);
		}
	}

	return RecognizedTemplates;
}

void UMagicSystem::SaveTemplate(const FString& TemplateName)
{
	if (DrawnPoints.Num() == 0)
	{
		return;
	}
	FDrawingTemplate Template;
	Template.Points = DrawnPoints;
	Template.Name = TEXT("Ice3");
	FString JsonString;
	if (FJsonObjectConverter::UStructToJsonObjectString(Template, JsonString))
	{
		FString SavePath = FPaths::ProjectDir() + TEXT("Config/DrawnShapes/Ice3.json");
		if (FFileHelper::SaveStringToFile(JsonString, *SavePath))
		{
			UE_LOG(LogTemp, Warning, TEXT("Template saved successfully: %s"), *SavePath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save template to file: %s"), *SavePath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to convert template to JSON string!"));
	}
}

void UMagicSystem::SetDrawnPoints(const TArray<FVector2D>& Points)
{
	DrawnPoints = Points;
	UE_LOG(LogTemp, Log, TEXT("Points set in widget. Count: %d"), DrawnPoints.Num());
}
