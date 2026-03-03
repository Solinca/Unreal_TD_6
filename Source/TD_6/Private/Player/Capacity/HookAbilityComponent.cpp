#include "Player/Capacity/HookAbilityComponent.h"
#include "Data/MonsterDataAsset.h"
#include "Player/MyCharacter.h"
#include "Player/Capacity/HookProjectile.h"

UHookAbilityComponent::UHookAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHookAbilityComponent::ActivateAbility()
{
	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	const FRotator OwnerRotation = GetOwner()->GetActorRotation();

	const FVector SpawnLocation = OwnerLocation + OwnerRotation.RotateVector(SpawnOffset);

	FActorSpawnParameters SpawnParams;

	SpawnParams.Owner = GetOwner();

	SpawnParams.Instigator = Cast<APawn>(GetOwner());

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (ActiveProjectile = GetWorld()->SpawnActor<AHookProjectile>(HookProjectileClass, SpawnLocation, OwnerRotation, SpawnParams))
	{
		ActiveProjectile->InitHook(this, MonsterDataAsset->HookMaxDistance, MonsterDataAsset->HookSpeed, MonsterDataAsset->HookReelingTime);
	}
}

void UHookAbilityComponent::OnHookFinished()
{
	ActiveProjectile->ForceCleanup();

	ActiveProjectile = nullptr;
}
