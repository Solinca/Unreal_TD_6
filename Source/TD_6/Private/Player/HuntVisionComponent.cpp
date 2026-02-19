#include "Player/HuntVisionComponent.h"
#include "Player/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

UHuntVisionComponent::UHuntVisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = false;
}

void UHuntVisionComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwnerCharacter = Cast<AMyCharacter>(GetOwner());
	if (CachedOwnerCharacter)
	{
		CachedPC = Cast<APlayerController>(CachedOwnerCharacter->GetController());
		CachedCamera = CachedOwnerCharacter->GetCameraComponent();
	}
}

void UHuntVisionComponent::Activate()
{
	if (!CachedOwnerCharacter || !CachedPC || !CachedCamera)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		ScanTimerHandle,
		this,
		&UHuntVisionComponent::UpdateHuntVision,
		ScanInterval,
		true);
}

void UHuntVisionComponent::Deactivate()
{
	GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);
	ClearAllHighlights();
}

void UHuntVisionComponent::UpdateHuntVision()
{
	const FVector CameraLocation = CachedCamera->GetComponentLocation();

	int32 ViewportSizeX, ViewportSizeY;
	CachedPC->GetViewportSize(ViewportSizeX, ViewportSizeY);

	TArray<TWeakObjectPtr<ACharacter>> CurrentlyVisible;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const APlayerController* OtherPC = It->Get();
		if (!OtherPC || OtherPC == CachedPC)
		{
			continue;
		}

		ACharacter* OtherCharacter = OtherPC->GetCharacter();
		if (!OtherCharacter)
		{
			continue;
		}

		const FVector TargetLocation = OtherCharacter->GetActorLocation();

		FVector2D ScreenLocation;
		if (!CachedPC->ProjectWorldLocationToScreen(TargetLocation, ScreenLocation))
		{
			continue;
		}

		if (ScreenLocation.X < 0 || ScreenLocation.X > ViewportSizeX ||
			ScreenLocation.Y < 0 || ScreenLocation.Y > ViewportSizeY)
		{
			continue;
		}

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(CachedOwnerCharacter);
		QueryParams.AddIgnoredActor(OtherCharacter);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, TargetLocation, ECC_Visibility, QueryParams))
		{
			continue;
		}

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
