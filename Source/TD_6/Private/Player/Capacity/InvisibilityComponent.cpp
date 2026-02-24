#include "Player/Capacity/InvisibilityComponent.h"
#include "Data/MonsterDataAsset.h"
#include "Player/MyCharacter.h"

UInvisibilityComponent::UInvisibilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bAutoActivate = false;
}

void UInvisibilityComponent::BeginPlay()
{
	Super::BeginPlay();

	InitDynamicMaterials();
}

void UInvisibilityComponent::InitDynamicMaterials()
{
	USkeletalMeshComponent* Mesh = CachedMyCharacter->GetMesh();

	if (DynamicMaterials.IsEmpty())
	{
		const int32 NumMaterials = Mesh->GetNumMaterials();

		for (int32 i = 0; i < NumMaterials; ++i)
		{
			if (UMaterialInstanceDynamic* MaterialInstanceDynamic = Mesh->CreateDynamicMaterialInstance(i))
			{
				DynamicMaterials.Add(MaterialInstanceDynamic);
			}
		}
	}
}

void UInvisibilityComponent::ActivateAbility()
{
	bIsFadingOut = true;

	GetWorld()->GetTimerManager().SetTimer(DissolveTimerHandle, this, &UInvisibilityComponent::UpdateDissolve, DissolveRate, true);

	GetWorld()->GetTimerManager().SetTimer(AbilityTimer, this, &UBaseAbilityComponent::StopAbility, MonsterDataAsset->GhostInvisibilityDuration, false);
}

void UInvisibilityComponent::DeactivateAbility()
{
	bIsFadingOut = false;

	GetWorld()->GetTimerManager().SetTimer(DissolveTimerHandle, this, &UInvisibilityComponent::UpdateDissolve, DissolveRate, true);
}

void UInvisibilityComponent::UpdateDissolve()
{
	CurrentDissolveTime += DissolveRate * (bIsFadingOut ? 1 : -1);

	CurrentDissolveTime = FMath::Clamp(CurrentDissolveTime, 0, DissolveDuration);

	for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
	{
		DynamicMaterial->SetScalarParameterValue(DissolveParamName, FMath::Lerp(DissolveMinValue, DissolveMaxValue, CurrentDissolveTime / DissolveDuration));
	}

	if ((bIsFadingOut && CurrentDissolveTime == DissolveDuration) || (!bIsFadingOut && CurrentDissolveTime == 0))
	{
		GetWorld()->GetTimerManager().ClearTimer(DissolveTimerHandle);
	}
}
