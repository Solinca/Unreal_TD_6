#include "Player/Capacity/TrapAbilityComponent.h"
#include "Data/MonsterDataAsset.h"
#include "Player/MyCharacter.h"
#include "Player/Capacity/TrapActor.h"

UTrapAbilityComponent::UTrapAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTrapAbilityComponent::ActivateAbility()
{
	RemoveOldestTrapIfNeeded();

	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	const FRotator OwnerRotation = GetOwner()->GetActorRotation();

	const FVector ForwardPoint = OwnerLocation + OwnerRotation.Vector() * SpawnDistanceFromOwner;

	const FVector TraceStart = ForwardPoint;

	const FVector TraceEnd = ForwardPoint - FVector(0.f, 0.f, GroundTraceHeight);

	FHitResult HitResult;

	FCollisionQueryParams QueryParams;

	QueryParams.AddIgnoredActor(GetOwner());

	FVector SpawnLocation = ForwardPoint;

	FRotator SpawnRotation = FRotator::ZeroRotator;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		SpawnLocation = HitResult.ImpactPoint;

		const FVector GroundNormal = HitResult.ImpactNormal;

		const FVector ForwardDir = OwnerRotation.Vector();

		const FVector RightDir = FVector::CrossProduct(GroundNormal, ForwardDir).GetSafeNormal();

		const FVector AdjustedForward = FVector::CrossProduct(RightDir, GroundNormal).GetSafeNormal();

		SpawnRotation = FRotationMatrix::MakeFromXZ(AdjustedForward, GroundNormal).Rotator();
	}

	FActorSpawnParameters SpawnParams;

	SpawnParams.Owner = GetOwner();

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (ATrapActor* NewTrap = GetWorld()->SpawnActor<ATrapActor>(TrapActorClass, SpawnLocation, SpawnRotation, SpawnParams))
	{
		NewTrap->InitTrap(MonsterDataAsset->TrapSnareTime);

		SpawnedTraps.Add(NewTrap);
	}
}

void UTrapAbilityComponent::RemoveOldestTrapIfNeeded()
{
	SpawnedTraps.RemoveAll([](const TWeakObjectPtr<ATrapActor>& Trap) { return !Trap.IsValid(); });

	if (MonsterDataAsset.IsValid() && SpawnedTraps.Num() >= static_cast<int32>(MonsterDataAsset->MaxNumberOfTraps))
	{
		if (SpawnedTraps[0].IsValid())
		{
			SpawnedTraps[0]->Destroy();
		}

		SpawnedTraps.RemoveAt(0);
	}
}
