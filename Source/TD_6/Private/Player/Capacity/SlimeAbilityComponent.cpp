#include "Player/Capacity/SlimeAbilityComponent.h"
#include "Data/MonsterDataAsset.h"
#include "Player/MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

USlimeAbilityComponent::USlimeAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USlimeAbilityComponent::ActivateAbility()
{
	MyChara = Cast<AMyCharacter>(GetOwner());

	OriginalScale = MyChara->GetMesh()->GetRelativeScale3D();

	SlimeSprintSpeed = MonsterDataAsset->SlimeSprintSpeed;

	bIsTransforming = true;

	bIsFlattening = true;

	CurrentLerpTime = 0.f;

	MyChara->SetPlayerMovementSpeedServerSide(false, false, TransformationSlowSpeed);

	GetWorld()->GetTimerManager().SetTimer(ScaleLerpTimerHandle, this, &USlimeAbilityComponent::UpdateScaleLerp, ScaleLerpRate, true);

	GetWorld()->GetTimerManager().SetTimer(TransformTimerHandle, this, &USlimeAbilityComponent::OnTransformationComplete, TransformationDuration, false);

	GetWorld()->GetTimerManager().SetTimer(AbilityTimer, this, &UBaseAbilityComponent::StopAbility, MonsterDataAsset->SlimeTransformationDuration + TransformationDuration, false);
}

void USlimeAbilityComponent::DeactivateAbility()
{
	bIsTransforming = true;

	bIsFlattening = false;

	CurrentLerpTime = 0.f;

	MyChara->SetPlayerMovementSpeedServerSide(false, false, TransformationSlowSpeed);

	GetWorld()->GetTimerManager().SetTimer(ScaleLerpTimerHandle, this, &USlimeAbilityComponent::UpdateScaleLerp, ScaleLerpRate, true);

	GetWorld()->GetTimerManager().SetTimer(TransformTimerHandle, this, &USlimeAbilityComponent::OnRevertTransformationComplete, TransformationDuration, false);
}

void USlimeAbilityComponent::OnTransformationComplete()
{
	bIsTransforming = false;

	MyChara->SetPlayerMovementSpeedServerSide(false, false, SlimeSprintSpeed);
}

void USlimeAbilityComponent::OnRevertTransformationComplete()
{
	bIsTransforming = false;

	MyChara->SetPlayerMovementSpeedServerSide(true, false, 0);
}

void USlimeAbilityComponent::UpdateScaleLerp()
{
	CurrentLerpTime += ScaleLerpRate;

	CurrentLerpTime = FMath::Clamp(CurrentLerpTime, 0.f, TransformationDuration);

	const float Alpha = CurrentLerpTime / TransformationDuration;
	
	const FVector NewScale = bIsFlattening ? FMath::Lerp(OriginalScale, FlattenedScale, Alpha) : FMath::Lerp(FlattenedScale, OriginalScale, Alpha);

	MyChara->ChangeCharacterScale(NewScale);

	if (CurrentLerpTime >= TransformationDuration)
	{
		GetWorld()->GetTimerManager().ClearTimer(ScaleLerpTimerHandle);
	}
}
