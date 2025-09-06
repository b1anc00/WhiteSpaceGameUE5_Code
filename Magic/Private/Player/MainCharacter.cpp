#include "Player/MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MagicSystem/MagicSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Magic/Components/AttributeComponent.h"
#include "Magic/Player/HUD/PlayerHUD.h"
#include "Magic/Player/HUD/PlayerOverlay.h"
#include "Magic/Magician/FSpellData.h"
#include "Magic/Database/DatabaseManager.h"
#include "Magic/Database/MyGameInstance.h"
#include "Magic/Items/Items.h"
#include "Magic/Items/MagicPickup.h"
#include "Magic/ObjectsMap/BossWall.h"
#include "Magic/Player/HUD/InteractionWidget.h"
#include "Player/Enemies/Enemy.h"
#include "Magic/ObjectsMap/Bonfire/BonfireWidget.h"
#include "DieScreen.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bUsePawnControlRotation = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = 150.f;
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	EffectCharacterComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EffectCharacter"));
	EffectCharacterComponent->SetupAttachment(GetRootComponent());
	EffectCharacterComponent->SetAutoActivate(true);
	EffectCharacterComponent->SetAsset(nullptr);
	
}

void AMainCharacter::LoadDatabaseStatsPlayer()
{
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	if (GI && GI->DatabaseManager)
	{
		float LoadedHealth, LoadedMana, LoadedVigor, LoadedMind, LoadedPosX, LoadedPosY, LoadedPosZ;
		if (GI->DatabaseManager->LoadPlayerStats(LoadedHealth, LoadedMana,  LoadedVigor, LoadedMind, LoadedPosX, LoadedPosY, LoadedPosZ))
		{
			Attributes->CurrentHealth = LoadedHealth;
			Attributes->CurrentMind = LoadedMana;
			Attributes->Vigor = LoadedVigor;
			Attributes->Mind = LoadedMind;
			SetActorLocation(FVector(LoadedPosX, LoadedPosY, LoadedPosZ));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load playerstats"));
		}
		TArray<FInventoryItem> Items;
		if (GI->DatabaseManager->LoadInvetoryItems(Items))
		{
			bool bInvOk = GI->DatabaseManager->LoadInvetoryItems(Items);
			UE_LOG(LogTemp, Warning, TEXT("LoadInvetoryItems returned: %s"), bInvOk ? TEXT("true") : TEXT("false"));
				
			UE_LOG(LogTemp, Display, TEXT("Inv Loaded"));
			if (PlayerOverlay)
			{
				PlayerOverlay->InitializeInventory(Items);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Database load failed"));
	}
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Tags.Add(FName("Player"));
	
	InitializeHUD();
	
	PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputContext, 0);
		}
	}

	if (WidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(PlayerController, WidgetClass);
	}
	
	LoadDatabaseStatsPlayer();
	Attributes->CalcValueLevels();
	if (PlayerOverlay)
	{
		PlayerOverlay->SetHealthPercent(Attributes->GetHealthPercent());
		PlayerOverlay->SetMindPercent(Attributes->GetMindPercent());
	}
	SaveDatabaseOnExit();
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CastWidget && CastState == CastingState::CS_Casting)
	{
		CastWidget->UpdateEmitters();
	}
	if (bIsDrawing && CastState == CastingState::CS_Casting)
	{
		Draw();
	}
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* inputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		inputComponent->BindAction(MoveForward, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		inputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		inputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AMainCharacter::Run);
		inputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AMainCharacter::Walk);
		inputComponent->BindAction(CastAction, ETriggerEvent::Started, this, &AMainCharacter::MagicEnable);
		inputComponent->BindAction(CastAction, ETriggerEvent::Completed, this, &AMainCharacter::MagicDisable);
		inputComponent->BindAction(LMBDrawing, ETriggerEvent::Started, this, &AMainCharacter::StartDrawing);
		inputComponent->BindAction(LMBDrawing, ETriggerEvent::Completed, this, &AMainCharacter::StopDrawing);
		inputComponent->BindAction(EAction, ETriggerEvent::Started, this, &AMainCharacter::EPressAction);
		inputComponent->BindAction(RightArrow, ETriggerEvent::Started, this, &AMainCharacter::RightArrowPress);
		inputComponent->BindAction(LeftArrow, ETriggerEvent::Started, this, &AMainCharacter::LeftArrowPress);
		inputComponent->BindAction(RAction, ETriggerEvent::Started, this, &AMainCharacter::RPressAction);
		inputComponent->BindAction(SpaceAction, ETriggerEvent::Started, this, &AMainCharacter::SpaceTeleport);
	}
}

void AMainCharacter::InitializeHUD()
{
	APlayerController* HUDPlayerController = Cast<APlayerController>(GetController());
	if (HUDPlayerController)
	{
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(HUDPlayerController->GetHUD());
		if (PlayerHUD)
		{
			PlayerOverlay = PlayerHUD->GetPlayerOverlay();
			if (PlayerOverlay && Attributes)
			{
				PlayerOverlay->SetHealthPercent(Attributes->GetHealthPercent());
				PlayerOverlay->SetMindPercent(Attributes->GetMindPercent());
			}
		}
	}
}

void AMainCharacter::SaveDatabaseOnExit()
{
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	if (GI && GI->DatabaseManager)
	{
		GI->DatabaseManager->InsertPlayerStats(Attributes->CurrentHealth, Attributes->CurrentMind, Attributes->Vigor, Attributes->Mind, GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
	}
}

void AMainCharacter::OpenWidgetBonfire()
{
	if (BonfireWidgetClass)
	{
		BonfireWidgetRef = CreateWidget<UBonfireWidget>(GetWorld(), BonfireWidgetClass);
		if (BonfireWidgetRef)
		{
			BonfireWidgetRef->AddToViewport();
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				FInputModeUIOnly Input;
				Input.SetWidgetToFocus(BonfireWidgetRef->TakeWidget());
				PC->SetInputMode(Input);
				PC->bShowMouseCursor = true;
			}
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
	}
}

void AMainCharacter::OpenWidgetBonfireTimer()
{
	GetWorldTimerManager().SetTimer(BonfireTimerHandle, this, &AMainCharacter::OpenWidgetBonfire, 1.f, false);
}

void AMainCharacter::CloseWidgetBonfire()
{
	if (BonfireWidgetRef)
	{
		BonfireWidgetRef->RemoveFromParent();
	}
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameOnly Input;
		PC->SetInputMode(Input);
		PC->bShowMouseCursor = false;
	}
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void AMainCharacter::ShowMagicItemWidget()
{
	if (bCanPickupItem == true)
	{
		MagicPickup->ShowWidget();
		OverlappedItem = nullptr;
	}
}

void AMainCharacter::SpaceTeleport()
{
	if (Attributes->CurrentMind >= 5.f)
	{
		Attributes->ReceiveMind(5.f);
		APlayerController* HUDPlayerController = Cast<APlayerController>(GetController());
		if (HUDPlayerController)
		{
			APlayerHUD* PlayerHUD = Cast<APlayerHUD>(HUDPlayerController->GetHUD());
			if (PlayerHUD)
			{
				PlayerOverlay = PlayerHUD->GetPlayerOverlay();
				if (PlayerOverlay && Attributes)
				{
					PlayerOverlay->SetMindPercent(Attributes->GetMindPercent());
				}
			}	
		}
		if (TeleportSystem)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						TeleportSystem,
						GetActorLocation(),
						GetActorRotation(),
						FVector(1.f),
						true,
						true);
		}
		FVector MovementDirection = GetVelocity().GetSafeNormal();
		if (MovementDirection.IsNearlyZero())
		{
			MovementDirection = GetActorForwardVector();
		}
    
		// Вычисляем целевую точку телепортации
		FVector startLocation = GetActorLocation();
		FVector targetLocation = startLocation + (MovementDirection * 400.f);
    
		// Настроим начальную точку для вертикальной трассировки
		FVector traceStart = targetLocation + FVector(0, 0, 300.f); // немного выше целевой точки
		FVector traceEnd = targetLocation - FVector(0, 0, 300.f);   // ниже целевой точки

		FHitResult Hit;
		// Выполняем трассировку вниз
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			Hit,
			traceStart,
			traceEnd,
			ECC_Visibility
		);

		if (bHit)
		{
			// Проверяем нормаль поверхности
			float AcceptableAngleDegrees = 45.f;
			float SurfaceAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Hit.ImpactNormal, FVector::UpVector)));
        
			if (SurfaceAngle <= AcceptableAngleDegrees)
			{
				// Если угол допустимый, устанавливаем новую позицию, немного сдвинув по вертикали,
				// чтобы избежать провала сквозь пол
				FVector FinalLocation = Hit.Location + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
				SetActorLocation(FinalLocation, false);
            
				// Можно также спавнить систему частиц в новой точке телепортации
				if (TeleportSystem)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						TeleportSystem,
						GetActorLocation(),
						GetActorRotation(),
						FVector(1.f),
						true,
						true);
				}
				UGameplayStatics::PlaySound2D(GetWorld(), SpaceActionSound);
			}
		}
	}
}

void AMainCharacter::GoToBossWall()
{
	if (bBossWall && BossWall)
	{
		BossWall->OpenBossLevel(this);
	}
}

void AMainCharacter::ReLive()
{
	LoadDatabaseStatsPlayer();
	ActionState = PlayerStateCast::PSC_Unnoqupied;
	DieScreen->RemoveFromParent();
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(0.2f);
	}
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Tags.Remove(FName("Died"));
	if (!Tags.Contains(FName("Player")))
	{
		Tags.Add(FName("Player"));
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	FName SectionName = "SitUp";
	if (AnimInstance && BonfireSitMontage)
	{
		AnimInstance->Montage_Play(BonfireSitMontage);
		AnimInstance->Montage_JumpToSection(SectionName, BonfireSitMontage);
	}
	InitializeHUD();
}

void AMainCharacter::SitBonfireAnimation()
{
	if (EPressed == true && bCanSitBonfire == true && bSittedInBonfire == false)
	{
		UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
		if (GI && GI->DatabaseManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawning Enemies"));
			GI->DatabaseManager->SpawnEnemies(GetWorld());
		}
		
		bSittedInBonfire = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		FName SectionName = "SitDown";
		if (AnimInstance && BonfireSitMontage)
		{
			AnimInstance->Montage_Play(BonfireSitMontage);
			AnimInstance->Montage_JumpToSection(SectionName, BonfireSitMontage);
			UGameplayStatics::PlaySound2D(GetWorld(), SitBonfireSound);
		}
		OpenWidgetBonfireTimer();
		Attributes->ResetStats();
		APlayerController* HUDPlayerController = Cast<APlayerController>(GetController());
		if (HUDPlayerController)
		{
			APlayerHUD* PlayerHUD = Cast<APlayerHUD>(HUDPlayerController->GetHUD());
			if (PlayerHUD)
			{
				PlayerOverlay = PlayerHUD->GetPlayerOverlay();
				if (PlayerOverlay && Attributes)
				{
					PlayerOverlay->SetHealthPercent(Attributes->GetHealthPercent());
					PlayerOverlay->SetMindPercent(Attributes->GetMindPercent());
				}
			}	
		}
	}
	else if (EPressed == true && bCanSitBonfire == true && bSittedInBonfire == true)
	{
		bSittedInBonfire = false;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		FName SectionName = "SitUp";
		if (AnimInstance && BonfireSitMontage)
		{
			AnimInstance->Montage_Play(BonfireSitMontage);
			AnimInstance->Montage_JumpToSection(SectionName, BonfireSitMontage);
		}
	}
}

void AMainCharacter::ResetE()
{
	EPressed = false;
}

void AMainCharacter::PickupItem(const FString& ItemName)
{
	if (bCanPickupItem == true)
	{
		UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
		if (GI && GI->DatabaseManager)
		{
			bool bResult = GI->DatabaseManager->AddItemToInvetory(ItemName);
			if (!bResult)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to add item '%s' to inventory."), *ItemName);
			}
			else if (bResult)
			{
				TArray<FInventoryItem> Items;
				GI->DatabaseManager->LoadInvetoryItems(Items);
				if (PlayerOverlay)
				{
					PlayerOverlay->InitializeInventory(Items);
				}
				OverlappedItem->Destroy();
				OverlappedItem = nullptr;
				bCanPickupItem = false;
				PickupItemName = nullptr;
			}
		}
	}
}

void AMainCharacter::ShowInteractionWidget()
{
	if (InteractionWidget && InteractionWidgetAdded == false)
	{
		InteractionWidget->AddToViewport();
	}
	InteractionWidgetAdded = true;
}

void AMainCharacter::HideInteractionWidget()
{
	if (InteractionWidget && InteractionWidgetAdded == true)
	{
		InteractionWidget->RemoveFromParent();
	}
	InteractionWidgetAdded = false;
}

void AMainCharacter::LeftArrowPress()
{
	if (PlayerOverlay)
	{
		PlayerOverlay->ShowPreviousItemInventory();
	}
}

void AMainCharacter::RightArrowPress()
{
	if (PlayerOverlay)
	{
		PlayerOverlay->ShowNextItemInventory();
	}
}

void AMainCharacter::RPressAction()
{
	FInventoryItem CurrentItem = PlayerOverlay->GetCurrentItem();
	EItemType Type = GetItemTypeFromName(CurrentItem.Name);
	switch (Type)
	{
		case EItemType::EIT_Healing:
			{
				Attributes->CurrentHealth = FMath::Clamp(Attributes->CurrentHealth + 30.f, 0.f, Attributes->MaxHealth);
				PlayerOverlay->SetHealthPercent(Attributes->GetHealthPercent());
				UGameplayStatics::PlaySound2D(GetWorld(), RActionSound);
				break;
			}
		case EItemType::EIT_Mana:
			{
				Attributes->CurrentMind = FMath::Clamp(Attributes->CurrentMind + 50.f, 0.f, Attributes->MaxMind);
				PlayerOverlay->SetMindPercent(Attributes->GetMindPercent());
				UGameplayStatics::PlaySound2D(GetWorld(), RActionSound);
				break;
			}
		default:
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid Item Type"));
				break;
			}
	}
	RemoveOrDecreaseItem(CurrentItem.ItemID);
}

void AMainCharacter::RemoveOrDecreaseItem(int32 InItemID)
{
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	if (GI && GI->DatabaseManager)
	{
		GI->DatabaseManager->RemoveItemFromInvetory(InItemID); // Либо ваш метод
		// После этого перезагрузить инвентарь и обновить UI
		TArray<FInventoryItem> Items;
		GI->DatabaseManager->LoadInvetoryItems(Items);
		if (PlayerOverlay)
		{
			PlayerOverlay->InitializeInventory(Items);
		}
	}
}

void AMainCharacter::MagicEnable()
{
	if (ActionState != PlayerStateCast::PSC_Die)
	{
		// Если Q нажата повторно и ожидалось нажатие ЛКМ, то сбрасываем спавн
		if (bReadyToSpawn)
		{
			PendingSpawnActors.Empty(); // Очищаем подготовленные объекты
			bReadyToSpawn = false; // Отключаем ожидание ЛКМ
			UE_LOG(LogTemp, Warning, TEXT("Spawn process cancelled by pressing Q again."));
		}
		if (PlayerController)
		{
			CastState = CastingState::CS_Casting;
			PlayerController->bShowMouseCursor = true;
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			InputMode.SetHideCursorDuringCapture(false);
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetIgnoreLookInput(true);
			FVector2D ViewportSize;
			if(GEngine)
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			PlayerController->SetMouseLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
			if (CastWidgetClass && !CastWidget)
			{
				CastWidget = CreateWidget<UMagicSystem>(PlayerController, CastWidgetClass);
				if (CastWidget)
				{
					CastWidget->AddToViewport();
					CastWidget->CreateGrid(15, 15);
					FString DrawLoadPath = FPaths::ProjectConfigDir() / TEXT("DrawnShapes/");
					CastWidget->LoadDrawingTemplates(DrawLoadPath);
				}
			}
		}
	}
}

void AMainCharacter::MagicDisable()
{
	if (ActionState != PlayerStateCast::PSC_Die)
	{
		if (PlayerController)
		{
			CastState = CastingState::CS_NotCasting;
			PlayerController->bShowMouseCursor = false;
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetIgnoreLookInput(false);

			if (CastWidget)
			{
				UE_LOG(LogTemp, Log, TEXT("Processing all drawings. Total: %d"), AllDrawnPoints.Num());

				// Обрабатываем каждый рисунок
				for (const TArray<FVector2D>& Drawing : AllDrawnPoints)
				{
					CastWidget->SetDrawnPoints(Drawing); // Передаём текущий рисунок
					TArray<FString> RecognizedDrawings = CastWidget->RecognizeDrawing();

					UE_LOG(LogTemp, Log, TEXT("Recognized drawings count for current set: %d"), RecognizedDrawings.Num());

					for (const FString& RecognizedDrawing : RecognizedDrawings)
					{
						if (!RecognizedDrawing.IsEmpty())
						{
							PendingSpawnActors.Add(RecognizedDrawing); // Сохраняем для дальнейшего спауна
						}
					}
				}

				if (AllDrawnPoints.Num() > 0)
				{
					bReadyToSpawn = true;
				}

				// Очищаем общий массив после обработки
				AllDrawnPoints.Empty();

				// Очищаем виджет
				CastWidget->RemoveEmitters();
				CastWidget->RemoveFromParent();
				CastWidget = nullptr;

				UE_LOG(LogTemp, Log, TEXT("Ready to spawn objects: %d"), PendingSpawnActors.Num());
			}
		}
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != PlayerStateCast::PSC_HitReact && ActionState != PlayerStateCast::PSC_Die && bSittedInBonfire == false)
	{
		const FVector2D MovementVector = Value.Get<FVector2D>();
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Forward, MovementVector.Y);
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Right, MovementVector.X);
	}
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();
	AddControllerPitchInput(LookAxis.Y);
	AddControllerYawInput(LookAxis.X);
}

void AMainCharacter::Run()
{
	MovementComponent->MaxWalkSpeed = 450.f;
}

void AMainCharacter::Walk()
{
	MovementComponent->MaxWalkSpeed = 150.f;
}

void AMainCharacter::EPressAction()
{
	EPressed = true;
	GetWorldTimerManager().SetTimer(TimerHandle_ResetE, this, &AMainCharacter::ResetE, 0.5f, false);
	SitBonfireAnimation();
	PickupItem(PickupItemName);
	ShowMagicItemWidget();
	GoToBossWall();
}

void AMainCharacter::StartDrawing()
{
	if (ActionState != PlayerStateCast::PSC_Die)
	{
		if (bReadyToSpawn)
		{
			for (const FString& DrawingName : PendingSpawnActors)
			{
				SpawningActorForDrawing(DrawingName);
				UGameplayStatics::PlaySound2D(GetWorld(), CastActionSound);
			}

			PendingSpawnActors.Empty(); // Очищаем список объектов
			bReadyToSpawn = false;      // Сбрасываем флаг
			UE_LOG(LogTemp, Log, TEXT("Actors spawned."));
		}
		else if (CastState == CastingState::CS_Casting)
		{
			bIsDrawing = true;
			if (CastWidget)
			{
				CastWidget->DrawnPoints.Empty();
			}
		}
	}
}

void AMainCharacter::StopDrawing()
{
	if (ActionState != PlayerStateCast::PSC_Die)
	{
		bIsDrawing = false;
		if (CastWidget)
		{
			if (CastWidget->DrawnPoints.Num() > 0)
			{
				// Сохраняем текущий рисунок в общий массив
				AllDrawnPoints.Add(CastWidget->DrawnPoints);
				UE_LOG(LogTemp, Log, TEXT("Drawing saved. Total drawings: %d"), AllDrawnPoints.Num());
				//CastWidget->SaveTemplate(TEXT("Ice3"));
			}

			// Очищаем текущие точки для нового рисунка
			CastWidget->DrawnPoints.Empty();

			UE_LOG(LogTemp, Log, TEXT("Drawing stopped, waiting for further processing."));
		}
	}
}

void AMainCharacter::Draw()
{
	if (!CastWidget) return;
	FVector2D MousePosition;
	if (PlayerController && PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		UNiagaraComponent* NearestEmitter = CastWidget->FindNearestEmitter(MousePosition);
		if (NearestEmitter)
		{
			NearestEmitter->SetVariableVec4(FName(TEXT("UserColor")), FVector4(0.f, 0.f, 100.f, 1.0f));
			PreviousEmitter = NearestEmitter;
			FVector WorldLocation = NearestEmitter->GetComponentLocation();
			FVector2D ScreenPosition;
			if (UGameplayStatics::ProjectWorldToScreen(PlayerController, WorldLocation, ScreenPosition))
			{
				if (!CastWidget->DrawnPoints.Contains(ScreenPosition))
				{
					CastWidget->DrawnPoints.Add(ScreenPosition);
				}
			}
		}
	}
}

void AMainCharacter::SpawnActor(const FString& DrawingName, FSpellData* SpellData)
{
	// Получаем направление камеры и устанавливаем поворот персонажа
	FRotator CameraRotation = Camera->GetComponentRotation();
	FRotator CharacterRotation = GetActorRotation();
	CharacterRotation.Yaw = CameraRotation.Yaw;
	SetActorRotation(CharacterRotation);

	// Рассчитываем позицию спавна перед персонажем
	FVector CharacterForward = GetActorForwardVector();
	CharacterForward.Normalize();
	FVector SpawnLocation = GetActorLocation() + CharacterForward * 200.f;
	FRotator SpawnRotation = CameraRotation;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = this;
	SpawnParameters.Owner = this;

	// Спавним актор, используя класс из данных DataTable
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SpellData->SpellActorClass, SpawnLocation, SpawnRotation, SpawnParameters);
	if (SpawnedActor)
	{
		UE_LOG(LogTemp, Log, TEXT("Успешно заспавнен актор для заклинания: %s"), *DrawingName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Не удалось заспавнить актор для заклинания: %s"), *DrawingName);
	}
}

void AMainCharacter::SpawningActorForDrawing(const FString& DrawingName)
{
	// Проверяем, назначена ли таблица
	if (!SpellDataTable) return;

	// Ищем данные заклинания по имени рисунка
	FName RowName(*DrawingName);
	FSpellData* SpellData = SpellDataTable->FindRow<FSpellData>(RowName, TEXT("Lookup Spell Data"), true);
	if (!SpellData) return;

	// Проверяем, что класс актора указан
	if (!SpellData->SpellActorClass) return;

	if (Attributes->CurrentMind < SpellData->ManaCost)
	{
		return;
	}

	Attributes->ReceiveMind(SpellData->ManaCost);
	PlayerOverlay->SetMindPercent(Attributes->GetMindPercent());
	SpawnActor(DrawingName, SpellData);
}

void AMainCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{
	if (Attributes && Attributes->isAlive())
	{
		DirectionalHitReact(Hitter->GetActorLocation());
		ActionState = PlayerStateCast::PSC_HitReact;
	}
	else Die();
}

void AMainCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z > 0)
	{
		Theta *= -1.f;
	}
	
	FName Section("Back");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("Front");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("Left");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("Right");
	}
	PlayHitReactMontage(Section);
}

void AMainCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AMainCharacter::Die()
{
	Tags.Add(FName("Died"));
	ActionState = PlayerStateCast::PSC_Die;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection("Death1", DeathMontage);
	}
	if (DieScreenClass)
	{
		DieScreen = CreateWidget<UDieScreen>(PlayerController, DieScreenClass);
		if (DieScreen)
		{
			DieScreen->AddToViewport();
		}
	}
	FTimerHandle DieTimer;
	GetWorldTimerManager().SetTimer(DieTimer, this, &AMainCharacter::ReLive, 4.f, false);
}

void AMainCharacter::HitReactEnd()
{
	ActionState = PlayerStateCast::PSC_Unnoqupied;
}

float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
	if (PlayerOverlay && Attributes)
	{
		PlayerOverlay->SetHealthPercent(Attributes->GetHealthPercent());
	}
	return DamageAmount;
}
