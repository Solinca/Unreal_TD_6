#include "Player/Capacity/SlimeAbilityComponent.h"

#include "Data/MonsterDataAsset.h"
#include "Player/MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

USlimeAbilityComponent::USlimeAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = false;
}

void USlimeAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (CachedMyCharacter.IsValid() && CachedMyCharacter->GetCharacterMovement())
	{
		DefaultMaxSpeed = CachedMyCharacter->GetCharacterMovement()->MaxWalkSpeed;
		OriginalScale = CachedMyCharacter->GetMesh()->GetRelativeScale3D();
	}
}

void USlimeAbilityComponent::ActivateAbility()
{
	if (!CachedMyCharacter.IsValid() || !CachedMyCharacter->GetCharacterMovement())
	{
		return;
	}

	if (MonsterDataAsset.IsValid())
	{
		SlimeSprintSpeed = MonsterDataAsset->SlimeSprintSpeed;
	}

	bIsTransforming = true;
	bIsFlattening = true;
	CurrentLerpTime = 0.f;

	CachedMyCharacter->GetCharacterMovement()->MaxWalkSpeed = TransformationSlowSpeed;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.SetTimer(
		ScaleLerpTimerHandle,
		this,
		&USlimeAbilityComponent::UpdateScaleLerp,
		ScaleLerpRate,
		true
	);

	TimerManager.SetTimer(
		TransformTimerHandle,
		this,
		&USlimeAbilityComponent::OnTransformationComplete,
		TransformationDuration,
		false
	);

	if (GetOwner() && GetOwner()->HasAuthority() && MonsterDataAsset.IsValid())
	{
		TimerManager.SetTimer(
			AbilityTimer,
			this,
			&UBaseAbilityComponent::StopAbility,
			MonsterDataAsset->SlimeTransformationDuration + TransformationDuration,
			false
		);
	}
}

void USlimeAbilityComponent::OnTransformationComplete()
{
	if (!CachedMyCharacter.IsValid() || !CachedMyCharacter->GetCharacterMovement())
	{
		return;
	}

	bIsTransforming = false;

	CachedMyCharacter->GetCharacterMovement()->MaxWalkSpeed = SlimeSprintSpeed;
}

void USlimeAbilityComponent::DeactivateAbility()
{
	if (!CachedMyCharacter.IsValid() || !CachedMyCharacter->GetCharacterMovement())
	{
		return;
	}

	bIsTransforming = true;
	bIsFlattening = false;
	CurrentLerpTime = 0.f;

	CachedMyCharacter->GetCharacterMovement()->MaxWalkSpeed = TransformationSlowSpeed;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.SetTimer(
		ScaleLerpTimerHandle,
		this,
		&USlimeAbilityComponent::UpdateScaleLerp,
		ScaleLerpRate,
		true
	);

	TimerManager.SetTimer(
		TransformTimerHandle,
		this,
		&USlimeAbilityComponent::OnRevertTransformationComplete,
		TransformationDuration,
		false
	);
}

void USlimeAbilityComponent::OnRevertTransformationComplete()
{
	if (!CachedMyCharacter.IsValid() || !CachedMyCharacter->GetCharacterMovement())
	{
		return;
	}

	bIsTransforming = false;

	CachedMyCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed;
}

void USlimeAbilityComponent::UpdateScaleLerp()
{
	if (!CachedMyCharacter.IsValid() || !CachedMyCharacter->GetMesh())
	{
		return;
	}

	CurrentLerpTime += ScaleLerpRate;
	CurrentLerpTime = FMath::Clamp(CurrentLerpTime, 0.f, TransformationDuration);

	const float Alpha = CurrentLerpTime / TransformationDuration;

	
	const FVector NewScale = bIsFlattening ? FMath::Lerp(OriginalScale, FlattenedScale, Alpha) : FMath::Lerp(FlattenedScale, OriginalScale, Alpha);

	CachedMyCharacter->GetMesh()->SetRelativeScale3D(NewScale);

	if (CurrentLerpTime >= TransformationDuration)
	{
		GetWorld()->GetTimerManager().ClearTimer(ScaleLerpTimerHandle);
	}
}