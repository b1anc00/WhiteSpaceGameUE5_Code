#include "Magic/Items/Items.h"
#include "NiagaraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Magic/Player/HUD/ItemPickupWidget.h"
#include "Magic/Public/Player/MainCharacter.h"

AItems::AItems()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffect"));
	RootComponent = ItemEffect;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(ItemEffect);
}

void AItems::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItems::OnSphereOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItems::OnSphereOverlapEnd);
}

void AItems::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItems::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player)
	{
		Player->bCanPickupItem = true;
		Player->PickupItemName = ItemName;
		Player->OverlappedItem = this;
		Player->ShowInteractionWidget();
	}
}

void AItems::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player)
	{
		Player->bCanPickupItem = false;
		Player->PickupItemName = nullptr;
		Player->OverlappedItem = nullptr;
		Player->HideInteractionWidget();
	}
}

void AItems::Destroy()
{
	if (PickupWidgetClass)
	{
		// Создаем виджет
		PickupWidget = CreateWidget<UItemPickupWidget>(GetWorld()->GetFirstPlayerController(), PickupWidgetClass);
		if (PickupWidget)
		{
			// Устанавливаем имя предмета
			PickupWidget->SetItemName(ItemName);
			// Добавляем виджет в viewport
			PickupWidget->AddToViewport();
            
			// Устанавливаем таймер на удаление виджета через 2 секунды
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				if (PickupWidget)
				{
					PickupWidget->RemoveFromParent();
					PickupWidget = nullptr;
				}
			}, 2.0f, false);
		}
	}
	SetLifeSpan(.1f);
}
