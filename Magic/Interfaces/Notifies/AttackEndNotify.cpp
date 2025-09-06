#include "Magic/Interfaces/Notifies/AttackEndNotify.h"
#include "Magic/Public/Player/Enemies/Enemy.h"

void UAttackEndNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		AEnemy* Enemy = Cast<AEnemy>(MeshComp->GetOwner());
		if (Enemy)
		{
			Enemy->AttackEnd();
		}
	}
}
