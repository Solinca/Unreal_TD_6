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
	
	if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetOwner()))
	{
		CachedMyCharacter = MyCharacter;

		CachedMyPlayerController = Cast<AMyPlayerController>(MyCharacter->GetController());
	}
}

void UBaseAbilityComponent::StartAbility_Implementation(class UMonsterDataAsset* InData)
{
	MonsterDataAsset = InData;

	ActivateAbility();
}

void UBaseAbilityComponent::StopAbility_Implementation()
{
	DeactivateAbility();
}
