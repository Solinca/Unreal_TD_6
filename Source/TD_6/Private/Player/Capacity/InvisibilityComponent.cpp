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

void UInvisibilityComponent::ActivateAbility()
{
	if (CachedMyCharacter.IsValid() && CachedMyCharacter->GetMesh())
	{
		CachedMyCharacter->GetMesh()->SetVisibility(true, true);
	}

	bIsFadingOut = true;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	TimerManager.SetTimer(
		DissolveTimerHandle, 
		this, 
		&UInvisibilityComponent::UpdateDissolve, 
		Rate, 
		true
	);

	if (GetOwner() && GetOwner()->HasAuthority() && MonsterDataAsset.IsValid())
	{
		TimerManager.SetTimer(
			AbilityTimer,
			this,
			&UBaseAbilityComponent::StopAbility,
			MonsterDataAsset->GhostInvisibilityDuration,
			false
		);
	}
}

void UInvisibilityComponent::UpdateDissolve()
{
	if (bIsFadingOut)
	{
		CurrentDissolveTime += Rate;
	}
	else
	{
		CurrentDissolveTime -= Rate;
	}

	CurrentDissolveTime = FMath::Clamp(CurrentDissolveTime, 0.f, DissolveDuration);

	const float Alpha = CurrentDissolveTime / DissolveDuration;

	const float MaterialValue = FMath::Lerp(DissolveMinValue, DissolveMaxValue, Alpha);

	for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(DissolveParamName, MaterialValue);
		}
	}

	if (bIsFadingOut && CurrentDissolveTime >= DissolveDuration)
	{
		GetWorld()->GetTimerManager().ClearTimer(DissolveTimerHandle);
		
		if (CachedMyCharacter.IsValid())
		{
			CachedMyCharacter->GetMesh()->SetVisibility(false, true);
		}
	}
	else if (!bIsFadingOut && CurrentDissolveTime <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(DissolveTimerHandle);
	}
}

void UInvisibilityComponent::DeactivateAbility()
{
	if (CachedMyCharacter.IsValid() && CachedMyCharacter->GetMesh())
	{
		CachedMyCharacter->GetMesh()->SetVisibility(true, true);
	}

	bIsFadingOut = false;

	GetWorld()->GetTimerManager().SetTimer(
		DissolveTimerHandle, 
		this, 
		&UInvisibilityComponent::UpdateDissolve, 
		Rate, 
		true
	);
}

void UInvisibilityComponent::InitDynamicMaterials()
{
	if (!CachedMyCharacter.IsValid())
	{
		return;
	}
	
	USkeletalMeshComponent* Mesh = CachedMyCharacter->GetMesh();
	if (!Mesh)
	{
		return;
	}
	
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
