#include "Magic/Items/MagicPickup.h"
#include "NiagaraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Magic/Public/Player/MainCharacter.h"
#include "Magic/Items/MagicItemWidget.h"

AMagicPickup::AMagicPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffect"));
	RootComponent = ItemEffect;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(ItemEffect);
}

void AMagicPickup::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AMagicPickup::OnSphereOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AMagicPickup::OnSphereOverlapEnd);
}

void AMagicPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMagicPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player)
	{
		Player->bCanPickupItem = true;
		Player->MagicPickup = this;
		Player->ShowInteractionWidget();
	}
}

void AMagicPickup::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	if (Player)
	{
		Player->bCanPickupItem = false;
		Player->MagicPickup = nullptr;
		Player->HideInteractionWidget();
	}
}

void AMagicPickup::ShowWidget()
{
	if (WidgetClass)
	{
		PickupWidget = CreateWidget<UMagicItemWidget>(GetWorld(), WidgetClass);
		if (PickupWidget)
		{
			PickupWidget->AddToViewport();
			PickupWidget->SetTexture(MagicImage);
			FTimerHandle Timer;
			GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
			{
				if (PickupWidget)
				{
					PickupWidget->RemoveFromParent();
					PickupWidget = nullptr;
				}
			}, 5.0f, false);
		}
	}
	SetLifeSpan(.1f);
}

