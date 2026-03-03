#include "Player/Capacity/BaseAbilityComponent.h"
#include "Data/MonsterDataAsset.h"
#include "Kismet/GameplayStatics.h"

UBaseAbilityComponent::UBaseAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

void UBaseAbilityComponent::MulticastPlayScreamWithSound_Implementation(USoundBase* Sound)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetOwner()->GetActorLocation());
}