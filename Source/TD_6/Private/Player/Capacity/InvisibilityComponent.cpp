#include "Player/Capacity/InvisibilityComponent.h"
#include "Data/MonsterDataAsset.h"
#include "Player/MyCharacter.h"

UInvisibilityComponent::UInvisibilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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

void UInvisibilityComponent::UpdateDissolve_Implementation()
{
	CurrentDissolveTime += DissolveRate * (bIsFadingOut ? 1 : -1);

	CurrentDissolveTime = FMath::Clamp(CurrentDissolveTime, 0, DissolveDuration);

	Cast<AMyCharacter>(GetOwner())->ChangePlayerMaterial(DissolveParamName, FMath::Lerp(DissolveMinValue, DissolveMaxValue, CurrentDissolveTime / DissolveDuration));

	if ((bIsFadingOut && CurrentDissolveTime == DissolveDuration) || (!bIsFadingOut && CurrentDissolveTime == 0))
	{
		GetWorld()->GetTimerManager().ClearTimer(DissolveTimerHandle);
	}
}
