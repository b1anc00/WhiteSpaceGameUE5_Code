#include "Magic/Magician/Magic_DamageBuff.h"
#include "NiagaraComponent.h"
#include "Player/MainCharacter.h"

AMagic_DamageBuff::AMagic_DamageBuff()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMagic_DamageBuff::BeginPlay()
{
	Super::BeginPlay();

	if (AMainCharacter* Character = Cast<AMainCharacter>(GetOwner()))
	{
		ApplyBuff();
		UE_LOG(LogTemp, Display, TEXT("Buff accepted"));
	}
}

void AMagic_DamageBuff::ApplyBuff()
{
	if (AMainCharacter* Character = Cast<AMainCharacter>(GetOwner()))
	{
		if (Character->EffectCharacterComponent && NiagaraSystem)
		{
			Character->EffectCharacterComponent->SetAsset(NiagaraSystem);
		}
		Character->DamageMultiplier += AmountBuff;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMagic_DamageBuff::RemoveBuff, DurationBuff, false);
	}
}

void AMagic_DamageBuff::RemoveBuff()
{
	if (AMainCharacter* Character = Cast<AMainCharacter>(GetOwner()))
	{
		if (Character->EffectCharacterComponent && NiagaraSystem)
		{
			Character->EffectCharacterComponent->SetAsset(nullptr);
		}
		Character->DamageMultiplier -= AmountBuff;
		UE_LOG(LogTemp, Display, TEXT("Buff removed"));
	}
	Destroy();
}
