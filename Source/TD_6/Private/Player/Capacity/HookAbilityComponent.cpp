#include "Player/Capacity/HookAbilityComponent.h"

#include "Camera/CameraComponent.h"
#include "Data/MonsterDataAsset.h"
#include "Player/MyCharacter.h"
#include "Player/Capacity/HookProjectile.h"

UHookAbilityComponent::UHookAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = false;
}

void UHookAbilityComponent::ActivateAbility()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !MonsterDataAsset.IsValid())
	{
		return;
	}

	if (!HookProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] HookAbilityComponent: HookProjectileClass is not set!"), *GetOwner()->GetName());
		return;
	}

	if (ActiveProjectile.IsValid())
	{
		ActiveProjectile->ForceCleanup();
		ActiveProjectile = nullptr;
	}

	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	const FRotator OwnerRotation = GetOwner()->GetActorRotation();
	const FVector SpawnLocation = OwnerLocation + OwnerRotation.RotateVector(SpawnOffset);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AHookProjectile* Projectile = GetWorld()->SpawnActor<AHookProjectile>(
		HookProjectileClass,
		SpawnLocation,
		OwnerRotation,
		SpawnParams
	);

	if (Projectile)
	{
		ActiveProjectile = Projectile;
		Projectile->InitHook(
			this,
			CachedMyCharacter->GetCameraComponent()->GetForwardVector(),
			MonsterDataAsset->HookMaxDistance,
			MonsterDataAsset->HookSpeed,
			MonsterDataAsset->HookReelingTime
		);
	}
}

void UHookAbilityComponent::DeactivateAbility()
{
	if (ActiveProjectile.IsValid())
	{
		ActiveProjectile->ForceCleanup();
		ActiveProjectile = nullptr;
	}
}

void UHookAbilityComponent::OnHookFinished()
{
	ActiveProjectile = nullptr;

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Deactivate();
		OnRep_IsActive();
	}
}