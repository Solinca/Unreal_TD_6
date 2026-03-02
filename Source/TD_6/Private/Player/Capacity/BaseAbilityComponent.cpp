#include "Player/Capacity/BaseAbilityComponent.h"
#include "Data/MonsterDataAsset.h"
#include "Player/MyCharacter.h"
#include "Player/MyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

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

void UBaseAbilityComponent::MulticastPlayScreamWithSound_Implementation(USoundBase* Sound)
{
	if (!Sound)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	USoundAttenuation* Attenuation = nullptr;
	if (MonsterDataAsset.IsValid())
	{
		Attenuation = MonsterDataAsset->ScreamAttenuation;
	}

	UGameplayStatics::PlaySoundAtLocation(
		this,
		Sound,
		Owner->GetActorLocation(),
		Owner->GetActorRotation(),
		1.0f,
		1.0f,
		0.0f,
		Attenuation
	);
}