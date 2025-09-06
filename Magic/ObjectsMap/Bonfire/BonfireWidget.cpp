#include "Magic/ObjectsMap/Bonfire/BonfireWidget.h"
#include "Magic/Public/Player/MainCharacter.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Magic/Components/AttributeComponent.h"

void UBonfireWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(PC->GetPawn());
		if (Player && Player->Attributes)
		{
			LevelVigor->SetText(FText::AsNumber(Player->Attributes->Vigor));
			LevelMind->SetText(FText::AsNumber(Player->Attributes->Mind));
		}
	}
	
	if (Escape)
	{
		Escape->OnClicked.AddDynamic(this, &UBonfireWidget::EscapeButton);
		VigorUpLevel->OnClicked.AddDynamic(this, &UBonfireWidget::VigorUpButton);
		VigorDecLevel->OnClicked.AddDynamic(this, &UBonfireWidget::VigorDecButton);
		MindUpLevel->OnClicked.AddDynamic(this, &UBonfireWidget::MindUpButton);
		MindDecLevel->OnClicked.AddDynamic(this, &UBonfireWidget::MindDecButton);
	}
}

void UBonfireWidget::EscapeButton()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(PC->GetPawn());
		if (Player && Player->Attributes)
		{
			Player->Attributes->CalcValueLevels();
			Player->CloseWidgetBonfire();
			Player->EPressed = true;
			Player->bCanSitBonfire = true;
			Player->bSittedInBonfire = true;
			Player->SitBonfireAnimation();
			Player->EPressed = false;
			Player->SaveDatabaseOnExit();
		}
	}
}

void UBonfireWidget::VigorUpButton()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(PC->GetPawn());
		if (Player && Player->Attributes)
		{
			Player->Attributes->Vigor = Player->Attributes->Vigor + 1;
			LevelVigor->SetText(FText::AsNumber(Player->Attributes->Vigor));
			LevelMind->SetText(FText::AsNumber(Player->Attributes->Mind));
		}
	}
}

void UBonfireWidget::VigorDecButton()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(PC->GetPawn());
		if (Player && Player->Attributes)
		{
			Player->Attributes->Vigor = Player->Attributes->Vigor - 1;
			LevelVigor->SetText(FText::AsNumber(Player->Attributes->Vigor));
			LevelMind->SetText(FText::AsNumber(Player->Attributes->Mind));
		}
	}
}

void UBonfireWidget::MindUpButton()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(PC->GetPawn());
		if (Player && Player->Attributes)
		{
			Player->Attributes->Mind = Player->Attributes->Mind + 1;
			LevelVigor->SetText(FText::AsNumber(Player->Attributes->Vigor));
			LevelMind->SetText(FText::AsNumber(Player->Attributes->Mind));
		}
	}
}

void UBonfireWidget::MindDecButton()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(PC->GetPawn());
		if (Player && Player->Attributes)
		{
			Player->Attributes->Mind = Player->Attributes->Mind - 1;
			LevelVigor->SetText(FText::AsNumber(Player->Attributes->Vigor));
			LevelMind->SetText(FText::AsNumber(Player->Attributes->Mind));
		}
	}
}
