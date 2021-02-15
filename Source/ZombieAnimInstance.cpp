// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAnimInstance.h"

UZombieAnimInstance::UZombieAnimInstance()
{
	status = EZombieStatus::E_IDLE;
}

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto actor = TryGetPawnOwner();
	if (::IsValid(actor))
	{
		AZombieHorde* horde = Cast<AZombieHorde>(actor);
		if (horde)
		{
			status = horde->status;
		}
	}
}

void UZombieAnimInstance::AnimNotify_AttackComplete()
{
	APawn* actor = TryGetPawnOwner();
	if (::IsValid(actor))
	{
		AZombieHorde* horde = Cast<AZombieHorde>(actor);

		if (horde)
		{
			if (!horde->is_Attacked)
			{
				horde->is_Attacked = true;
				horde->attackCooldown = (1.0f / horde->zombieAttackSpeed);

				if (::IsValid(horde->zombieAttackSound))
				{
					horde->audioComponent->SetSound(horde->zombieAttackSound);
				}

				horde->audioComponent->Play();

				if (horde->targetDoor != nullptr)
				{
					FPointDamageEvent damageEvent;
					horde->targetDoor->TakeDamage(horde->zombieAttackPoint * horde->zombieQuantity, damageEvent, horde->GetController(), horde);
				}
			}
		}
	}
}