#include "Player/Capacity/HookProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/MyCharacter.h"
#include "Player/Capacity/HookAbilityComponent.h"
#include "Global/MyGameInstance.h"

AHookProjectile::AHookProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SetReplicatingMovement(false);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(CollisionRadius);
	CollisionSphere->SetCollisionProfileName(TEXT("NoCollision"));
	CollisionSphere->SetGenerateOverlapEvents(false);
	SetRootComponent(CollisionSphere);

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	HookMesh->SetupAttachment(CollisionSphere);
	HookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HookMesh->SetIsReplicated(false);
}

void AHookProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHookProjectile, CurrentState);
	DOREPLIFETIME(AHookProjectile, CurrentReach);
	DOREPLIFETIME(AHookProjectile, LaunchOrigin);
	DOREPLIFETIME(AHookProjectile, TravelDirection);
}

void AHookProjectile::OnRep_CurrentReach()
{
	UpdateHookScale();
}

void AHookProjectile::OnRep_CurrentState()
{
	// Les clients peuvent réagir aux changements d'état ici (ex: SFX, animations)
}

void AHookProjectile::InitHook(UHookAbilityComponent* InAbility, float InMaxDistance, float InHookSpeed, float InReelingTime)
{
	OwningAbility = InAbility;
	MaxDistance = InMaxDistance;
	HookSpeed = InHookSpeed;
	ReelingTime = InReelingTime;

	LaunchOrigin = GetActorLocation();
	TravelDirection = GetActorForwardVector();
	CurrentState = EHookState::Traveling;
	CurrentReach = 0.f;

	UpdateHookScale();

	if (AMyCharacter* OwnerChara = Cast<AMyCharacter>(GetOwner()))
	{
		CachedOwnerCharacter = OwnerChara;
	}

	if (HasAuthority())
	{
		FreezeCharacter(CachedOwnerCharacter.Get());
	}
}

void AHookProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
	{
		if (CachedOwnerCharacter.IsValid())
		{
			SetActorLocation(CachedOwnerCharacter->GetActorLocation());
		}
		else if (const AActor* OwnerActor = GetOwner())
		{
			SetActorLocation(OwnerActor->GetActorLocation());
		}
		return;
	}

	if (CurrentState == EHookState::Finished)
	{
		return;
	}

	if (!CachedOwnerCharacter.IsValid())
	{
		ReleasePulledPlayer();
		Destroy();
		return;
	}

	SetActorLocation(CachedOwnerCharacter->GetActorLocation());
	LaunchOrigin = CachedOwnerCharacter->GetActorLocation();

	switch (CurrentState)
	{
	case EHookState::Traveling:
		Traveling(DeltaTime);
		break;

	case EHookState::Pulling:
		Pulling(DeltaTime);
		break;

	case EHookState::Returning:
		Returning(DeltaTime);
		break;

	default:
		break;
	}
}

FVector AHookProjectile::GetHookTipLocation() const
{
	return LaunchOrigin + TravelDirection * CurrentReach;
}

void AHookProjectile::UpdateHookScale()
{
	const float ScaleZ = FMath::Max(CurrentReach / BaseHookLength, 0.01f);
	HookMesh->SetWorldScale3D(FVector(1.f, 1.f, ScaleZ));
}

void AHookProjectile::Traveling(float DeltaTime)
{
	const float PreviousReach = CurrentReach;
	CurrentReach = FMath::Min(CurrentReach + HookSpeed * DeltaTime, MaxDistance);

	const FVector TraceStart = LaunchOrigin + TravelDirection * PreviousReach;
	const FVector TraceEnd = LaunchOrigin + TravelDirection * CurrentReach;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult WallHit;
	
	if (GetWorld()->LineTraceSingleByChannel(WallHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		CurrentReach = FVector::Dist(LaunchOrigin, WallHit.ImpactPoint);
		UpdateHookScale();
		MulticastOnHookMiss();
		StartReturning();
		return;
	}

	FHitResult CharacterHit;
	
	if (GetWorld()->SweepSingleByChannel(
		CharacterHit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(CollisionRadius),
		QueryParams))
	{
		
		AMyCharacter* HitCharacter = Cast<AMyCharacter>(CharacterHit.GetActor());
		
		if (HitCharacter && HitCharacter != GetOwner() && IsPlayerTeam(HitCharacter))
		{
			FCollisionQueryParams WallCheckParams;
			WallCheckParams.AddIgnoredActor(this);
			WallCheckParams.AddIgnoredActor(GetOwner());
			WallCheckParams.AddIgnoredActor(HitCharacter);

			FHitResult WallCheck;
			if (!GetWorld()->LineTraceSingleByChannel(
				WallCheck,
				LaunchOrigin,
				HitCharacter->GetActorLocation(),
				ECC_Visibility,
				WallCheckParams))
			{
				CurrentReach = FVector::Dist(LaunchOrigin, CharacterHit.ImpactPoint);
				UpdateHookScale();
				StartPulling(HitCharacter);
				return;
			}
		}
	}

	UpdateHookScale();

	if (CurrentReach >= MaxDistance)
	{
		MulticastOnHookMiss();
		StartReturning();
	}
}

void AHookProjectile::Pulling(float DeltaTime)
{
	PullElapsedTime += DeltaTime;

	if (PullElapsedTime >= PullTimeout)
	{
		ReleasePulledPlayer();
		StartReturning();
		return;
	}

	if (!HookedCharacter.IsValid())
	{
		StartReturning();
		return;
	}

	const FVector OwnerLocation = CachedOwnerCharacter->GetActorLocation();
	const FVector TargetLocation = HookedCharacter->GetActorLocation();
	const float Dist = FVector::Dist(TargetLocation, OwnerLocation);

	if (Dist <= PullArrivalDistance)
	{
		ReleasePulledPlayer();
		StartReturning();
		return;
	}

	const FVector Dir = (OwnerLocation - TargetLocation).GetSafeNormal();
	HookedCharacter->SetActorLocation(TargetLocation + Dir * PullSpeed * DeltaTime, false);

	CurrentReach = FVector::Dist(OwnerLocation, HookedCharacter->GetActorLocation());
	UpdateHookScale();
}

void AHookProjectile::Returning(float DeltaTime)
{
	CurrentReach -= ReturnSpeed * DeltaTime;

	if (CurrentReach <= 0.f)
	{
		CurrentReach = 0.f;
		UpdateHookScale();
		FinishHook();
	}
	else
	{
		UpdateHookScale();
	}
}

void AHookProjectile::StartPulling(AMyCharacter* TargetCharacter)
{
	HookedCharacter = TargetCharacter;
	PullElapsedTime = 0.f;

	if (CachedOwnerCharacter.IsValid())
	{
		const float InitialDist = FVector::Dist(
			TargetCharacter->GetActorLocation(),
			CachedOwnerCharacter->GetActorLocation()
		);
		PullSpeed = InitialDist / FMath::Max(PullDuration, 0.01f);
	}

	FreezeCharacter(TargetCharacter);

	CurrentState = EHookState::Pulling;
	MulticastOnHookHit();
}

void AHookProjectile::StartReturning()
{
	if (CachedOwnerCharacter.IsValid())
	{
		ReturnSpeed = CurrentReach / FMath::Max(ReelingTime, 0.01f);
	}
	else
	{
		ReturnSpeed = HookSpeed;
	}

	CurrentState = EHookState::Returning;
}

void AHookProjectile::ReleasePulledPlayer()
{
	if (HookedCharacter.IsValid())
	{
		UnfreezeCharacter(HookedCharacter.Get());
		HookedCharacter = nullptr;
	}
}

void AHookProjectile::FinishHook()
{
	CurrentState = EHookState::Finished;

	ReleasePulledPlayer();
	UnfreezeCharacter(CachedOwnerCharacter.Get());

	MulticastOnHookFinished();

	if (OwningAbility.IsValid())
	{
		OwningAbility->OnHookFinished();
	}

	Destroy();
}

void AHookProjectile::ForceCleanup()
{
	CurrentState = EHookState::Finished;

	ReleasePulledPlayer();
	UnfreezeCharacter(CachedOwnerCharacter.Get());

	Destroy();
}

void AHookProjectile::FreezeCharacter(AMyCharacter* Character)
{
	if (Character)
	{
		if (UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
		{
			CMC->DisableMovement();
		}
	}
}

void AHookProjectile::UnfreezeCharacter(AMyCharacter* Character)
{
	if (Character)
	{
		if (UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
		{
			CMC->SetMovementMode(MOVE_Walking);
		}
	}
}

void AHookProjectile::MulticastOnHookHit_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Hook HIT!"));
}

void AHookProjectile::MulticastOnHookMiss_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Hook MISS"));
}

void AHookProjectile::MulticastOnHookFinished_Implementation()
{
	// Placeholder VFX/SFX
}

bool AHookProjectile::IsPlayerTeam(AMyCharacter* Character) const
{
	if (!Character)
	{
		return false;
	}

	if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
	{
		if (const APlayerController* PC = Cast<APlayerController>(Character->GetController()))
		{
			if (const APlayerState* PS = PC->GetPlayerState<APlayerState>())
			{
				const FCustomPlayerData PlayerData = GI->RetrieveServerPlayerData(PS->GetUniqueId());
				return PlayerData.CurrentTeam == ETeam::PLAYER;
			}
		}
	}

	return false;
}