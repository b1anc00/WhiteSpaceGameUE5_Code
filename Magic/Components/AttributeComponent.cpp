#include "Magic/Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
}

float UAttributeComponent::GetHealthPercent()
{
	return CurrentHealth / MaxHealth;
}

bool UAttributeComponent::isAlive()
{
	return CurrentHealth > 0.f;
}

void UAttributeComponent::ReceiveMind(float Value)
{
	CurrentMind = FMath::Clamp(CurrentMind - Value, 0.0f, MaxMind);
}

float UAttributeComponent::GetMindPercent()
{
	return CurrentMind / MaxMind;
}

bool UAttributeComponent::isMindCan()
{
	return CurrentMind > 0.f;
}

void UAttributeComponent::ResetStats()
{
	CurrentHealth = MaxHealth;
	CurrentMind = MaxMind;
}

void UAttributeComponent::CalcValueLevels() //вызов для расчёта уровня
{
	CalcVigor();
	CalcMind();
}

void UAttributeComponent::CalcVigor()
{
	if (Vigor <= 35)
	{
		float a = FMath::Pow(50.f, 1.f / 34.f);
		MaxHealth = 50.f * FMath::Pow(a, Vigor - 1);
	}
	else if (Vigor <= 99)
	{
		float b = FMath::Pow(1.8f, 1.f / 64.f);
		MaxHealth = 2500.f * FMath::Pow(b, Vigor - 35);
	}
	else
	{
		MaxHealth = 4500.f;
	}
	CurrentHealth = MaxHealth;
}

void UAttributeComponent::CalcMind()
{
	if (Mind <= 35)
	{
		float a = FMath::Pow(50.f, 1.f / 34.f);
		MaxMind = 50.f * FMath::Pow(a, Mind - 1);
	}
	else if (Mind <= 99)
	{
		float b = FMath::Pow(1.8f, 1.f / 64.f);
		MaxMind = 2500.f * FMath::Pow(b, Mind - 35);
	}
	else
	{
		MaxMind = 4500.f;
	}
	CurrentMind = MaxMind;
}




