#include "Player/Capacity/HuntVisionComponent.h"
#include "Player/MyCharacter.h"
#include "Data/MonsterDataAsset.h"
#include "EngineUtils.h"

UHuntVisionComponent::UHuntVisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHuntVisionComponent::ActivateAbility()
{
	MyChara = Cast<AMyCharacter>(GetOwner());

	MyChara->ChangePostProcess(false);

	MyChara->PlayAbilitySFX(MonsterDataAsset->AbilityTriggerSound, true);

	GetWorld()->GetTimerManager().SetTimer(ScanTimerHandle, this, &UHuntVisionComponent::UpdateHuntVision, ScanInterval, true);

	GetWorld()->GetTimerManager().SetTimer(AbilityTimer, this, &UBaseAbilityComponent::StopAbility, MonsterDataAsset->PredatorHuntDuration, false);
}

void UHuntVisionComponent::DeactivateAbility()
{
	MyChara->ChangePostProcess(true);

	GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);

	ClearAllHighlights();
}

void UHuntVisionComponent::UpdateHuntVision()
{
	for (TActorIterator<AMyCharacter> It(GetWorld()); It; ++It)
	{
		HighlightedCharacters.Add(*It);

		MyChara->SetCharacterHighlight(*It, true);
	}
}

void UHuntVisionComponent::ClearAllHighlights()
{
	for (ACharacter* Char : HighlightedCharacters)
	{
		MyChara->SetCharacterHighlight(Char, false);
	}

	HighlightedCharacters.Empty();
}
