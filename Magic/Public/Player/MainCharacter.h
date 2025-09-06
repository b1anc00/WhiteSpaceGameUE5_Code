#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Magic/Interfaces/HitInterface.h"
#include "Magic/Magician/FSpellData.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class CastingState : uint8
{
	CS_NotCasting UMETA(DisplayName = "NotCasting"),
	CS_Casting UMETA(DisplayName = "Casting")
};

UENUM(BlueprintType)
enum class PlayerStateCast : uint8
{
	PSC_Unnoqupied UMETA(DisplayName = "Unnoqiupied"),
	PSC_HitReact UMETA(DisplayName = "HitReact"),
	PSC_Attacking UMETA(DisplayName = "Attacking"),
	PSC_Die UMETA(DisplayName = "Die")
};

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCharacterMovementComponent;
class UMagicSystem;
class APlayerController;
class UNiagaraComponent;
class UPlayerOverlay;
class UDataTable;
class UDatabaseManager;
class AItems;
class AMagicPickup;
class UNiagaraSystem;
class ABossWall;
class UDieScreen;

UCLASS()
class MAGIC_API AMainCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AMainCharacter();
	void LoadDatabaseStatsPlayer();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Компоненты персонажа */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY()
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY()
	APlayerController* PlayerController;

	/** Входные данные (Inputs) */
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveForward;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* RunAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CastAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LMBDrawing;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* RightArrow;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LeftArrow;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* RAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SpaceAction;
	
	/** Состояния */
	UPROPERTY(VisibleInstanceOnly)
	CastingState CastState = CastingState::CS_NotCasting;

	bool bIsDrawing;
	bool bReadyToSpawn = false;

	/** Магическая система */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> CastWidgetClass;

	UMagicSystem* CastWidget;
	UNiagaraComponent* PreviousEmitter = nullptr;

	TArray<TArray<FVector2D>> AllDrawnPoints;
	TArray<FString> PendingSpawnActors;

	/** Методы управления магией */
	void MagicEnable();
	void MagicDisable();

	/** Методы управления движением */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Run();
	void Walk();
	void EPressAction();

	/** Методы рисования */
	void StartDrawing();
	void StopDrawing();
	void Draw();
	void SpawnActor(const FString& DrawingName, FSpellData* SpellData);

public:
	/** Спавн объектов */
	UFUNCTION(BlueprintCallable, Category = Magic)
	void SpawningActorForDrawing(const FString& DrawingName);

	/** Переписать */
	void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* BonfireSitMontage;

	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* Attributes;

	void DirectionalHitReact(const FVector& ImpactPoint);

	void PlayHitReactMontage(const FName& SectionName);

	void Die();

	UPROPERTY(BluePrintReadOnly)
	PlayerStateCast ActionState = PlayerStateCast::PSC_Unnoqupied;

	UFUNCTION(blueprintCallable)
	void HitReactEnd();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY()
	UPlayerOverlay* PlayerOverlay;

	void InitializeHUD();
	void SaveDatabaseOnExit();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Magic)
	UDataTable* SpellDataTable;

	void SitBonfireAnimation();

	bool EPressed = false;
	FTimerHandle TimerHandle_ResetE;
	void ResetE();

	bool bCanSitBonfire = false;

	bool bSittedInBonfire = false;

	bool bCanPickupItem = false;

	void PickupItem(const FString& ItemName);

	FString PickupItemName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AItems* OverlappedItem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UInteractionWidget> WidgetClass;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;

	void ShowInteractionWidget();

	void HideInteractionWidget();

	bool InteractionWidgetAdded = false;

	void LeftArrowPress();

	void RightArrowPress();

	void RPressAction();

	void RemoveOrDecreaseItem(int32 InItemID);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UBonfireWidget> BonfireWidgetClass;

	UPROPERTY()
	UBonfireWidget* BonfireWidgetRef;

	void OpenWidgetBonfire();

	UFUNCTION()
	void OpenWidgetBonfireTimer();

	FTimerHandle BonfireTimerHandle;

	void CloseWidgetBonfire();

	float DamageMultiplier = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Magic)
	UNiagaraComponent* EffectCharacterComponent;

	void ShowMagicItemWidget();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AMagicPickup* MagicPickup = nullptr;

	void SpaceTeleport();

	UPROPERTY(EditAnywhere, Category = Combat)
	UNiagaraSystem* TeleportSystem;

	bool bBossWall = false;

	UFUNCTION(BlueprintCallable)
	void GoToBossWall();

	ABossWall* BossWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UDieScreen> DieScreenClass;

	UDieScreen* DieScreen;
	
	void ReLive();

	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* RActionSound;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* SpaceActionSound;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* SitBonfireSound;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* CastActionSound;
};
