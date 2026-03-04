#include "Player/Capacity/HookAbilityComponent.h"
#include "Camera/CameraComponent.h"
#include "Data/MonsterDataAsset.h"
#include "Player/MyCharacter.h"
#include "Player/Capacity/HookProjectile.h"

UHookAbilityComponent::UHookAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHookAbilityComponent::ActivateAbility()
{
	MyChara = Cast<AMyCharacter>(GetOwner());

	MyChara->PlayAbilitySFX(MonsterDataAsset->AbilityTriggerSound, false, false);

	MyChara->PlayAbilitySFX(MonsterDataAsset->AbilityOnGoingSound, false, true);

	FActorSpawnParameters SpawnParams;

	SpawnParams.Owner = GetOwner();

	SpawnParams.Instigator = Cast<APawn>(GetOwner());

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AHookProjectile* Projectile = GetWorld()->SpawnActor<AHookProjectile>(HookProjectileClass, MyChara->GetActorLocation(), MyChara->GetActorRotation(), SpawnParams))
	{
		Projectile->InitHook(Cast<AMyCharacter>(GetOwner())->GetCameraComponent()->GetForwardVector(), MonsterDataAsset->HookMaxDistance, MonsterDataAsset->HookSpeed, MonsterDataAsset->HookReelingTime);
	}
}
