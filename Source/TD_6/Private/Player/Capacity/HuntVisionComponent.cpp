#include "Player/Capacity/HuntVisionComponent.h"
#include "EngineUtils.h"
#include "Player/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/MonsterDataAsset.h"
#include "Player/MyPlayerController.h"

UHuntVisionComponent::UHuntVisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bAutoActivate = false;
}

void UHuntVisionComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCamera = CachedMyCharacter->GetCameraComponent();
}

void UHuntVisionComponent::ActivateAbility()
{
	if (CachedMyPlayerController.IsValid() && CachedMyPlayerController->IsLocalController())
	{
		CachedMyCharacter->ChangePostProcess(false);

		GetWorld()->GetTimerManager().SetTimer(ScanTimerHandle, this, &UHuntVisionComponent::UpdateHuntVision, ScanInterval, true);

		GetWorld()->GetTimerManager().SetTimer(AbilityTimer, this, &UBaseAbilityComponent::StopAbility, MonsterDataAsset->PredatorHuntDuration, false);
	}
}

void UHuntVisionComponent::DeactivateAbility()
{
	if (CachedMyPlayerController.IsValid() && CachedMyPlayerController->IsLocalController())
	{
		CachedMyCharacter->ChangePostProcess(true);

		GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);

		ClearAllHighlights();
	}
}

void UHuntVisionComponent::UpdateHuntVision()
{
	const FVector CameraLocation = CachedCamera->GetComponentLocation();

	int32 ViewportSizeX, ViewportSizeY;

	CachedMyPlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	TArray<TWeakObjectPtr<ACharacter>> CurrentlyVisible;

	for (TActorIterator<AMyCharacter> It(GetWorld()); It; ++It)
	{
		ACharacter* OtherCharacter = *It;

		if (!OtherCharacter || OtherCharacter == CachedMyCharacter.Get())
		{
			continue;
		}

		const FVector TargetLocation = OtherCharacter->GetActorLocation();

		FVector2D ScreenLocation;

		if (!CachedMyPlayerController->ProjectWorldLocationToScreen(TargetLocation, ScreenLocation))
		{
			continue;
		}

		if (ScreenLocation.X < 0 || ScreenLocation.X > ViewportSizeX || ScreenLocation.Y < 0 || ScreenLocation.Y > ViewportSizeY)
		{
			continue;
		}

		FHitResult HitResult;

		FCollisionQueryParams QueryParams;

		QueryParams.AddIgnoredActor(CachedMyCharacter.Get());

		QueryParams.AddIgnoredActor(OtherCharacter);

		CurrentlyVisible.Add(OtherCharacter);

		if (!HighlightedCharacters.Contains(OtherCharacter))
		{
			SetCharacterHighlight(OtherCharacter, true);
		}
	}

	for (int32 i = HighlightedCharacters.Num() - 1; i >= 0; --i)
	{
		if (!HighlightedCharacters[i].IsValid() || !CurrentlyVisible.Contains(HighlightedCharacters[i]))
		{
			if (HighlightedCharacters[i].IsValid())
			{
				SetCharacterHighlight(HighlightedCharacters[i].Get(), false);
			}

			HighlightedCharacters.RemoveAt(i);
		}
	}

	for (const TWeakObjectPtr<ACharacter>& Char : CurrentlyVisible)
	{
		if (!HighlightedCharacters.Contains(Char))
		{
			HighlightedCharacters.Add(Char);
		}
	}
}

void UHuntVisionComponent::SetCharacterHighlight(ACharacter* Character, bool bHighlight)
{
	if (USkeletalMeshComponent* MeshComp = Character->GetMesh())
	{
		MeshComp->SetRenderCustomDepth(bHighlight);

		if (bHighlight)
		{
			MeshComp->SetCustomDepthStencilValue(1);
		}
	}
}

void UHuntVisionComponent::ClearAllHighlights()
{
	for (const TWeakObjectPtr<ACharacter>& Char : HighlightedCharacters)
	{
		if (Char.IsValid())
		{
			SetCharacterHighlight(Char.Get(), false);
		}
	}

	HighlightedCharacters.Empty();
}
