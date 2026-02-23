#include "Player/Capacity/BaseAbilityComponent.h"

#include "Data/MonsterDataAsset.h"
#include "Player/MyCharacter.h"
#include "Player/MyPlayerController.h"
#include "Net/UnrealNetwork.h"

UBaseAbilityComponent::UBaseAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UBaseAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto* MyCharacter = Cast<AMyCharacter>(GetOwner()))
	{
		CachedMyCharacter = MyCharacter;
		CachedMyPlayerController = Cast<AMyPlayerController>(MyCharacter->GetController());
	}
}

void UBaseAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UBaseAbilityComponent::StartAbility(UMonsterDataAsset* InData)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (!MonsterDataAsset.Get())
		{
			MonsterDataAsset = InData;
		}
		
		Activate(true);
		OnRep_IsActive();
	}
}

void UBaseAbilityComponent::StopAbility()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] StopAbility CALLED - HasAuthority: %d, IsActive: %d"),
		*GetOwner()->GetName(),
		GetOwner()->HasAuthority(),
		IsActive());
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Deactivate();
		OnRep_IsActive();
	}
}

void UBaseAbilityComponent::OnRep_IsActive()
{
	IsActive() ? ActivateAbility() : DeactivateAbility();
}