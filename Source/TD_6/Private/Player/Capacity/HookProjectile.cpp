#include "Player/Capacity/HookProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Player/MyCharacter.h"
#include "Player/Capacity/HookAbilityComponent.h"
#include "Global/MyGameInstance.h"

AHookProjectile::AHookProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicatingMovement(true);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(CollisionRadius);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionSphere->SetGenerateOverlapEvents(true);
	SetRootComponent(CollisionSphere);

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	HookMesh->SetupAttachment(CollisionSphere);
	HookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	if (AMyCharacter* OwnerChara = Cast<AMyCharacter>(GetOwner()))
	{
		CachedOwnerCharacter = OwnerChara;
	}

	if (HasAuthority())
	{
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AHookProjectile::OnHitSomething);

		FreezeCharacter(CachedOwnerCharacter.Get());
	}
}

void AHookProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || CurrentState == EHookState::Finished)
	{
		return;
	}

	if (!CachedOwnerCharacter.IsValid())
	{
		ReleasePulledPlayer();
		Destroy();
		return;
	}

	switch (CurrentState)
	{
	case EHookState::Traveling:
	{
		Traveling(DeltaTime);
		break;
	}
		
	case EHookState::Pulling:
	{
		Pulling(DeltaTime);
	}

	case EHookState::Returning:
	{
		Returning(DeltaTime);
	}

	default:
		break;
	}
}

void AHookProjectile::Traveling(float DeltaTime)
{
	const FVector PreviousLocation = GetActorLocation();
	const FVector Delta = TravelDirection * HookSpeed * DeltaTime;
	const FVector NewLocation = PreviousLocation + Delta;

	FHitResult WallHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(WallHit, PreviousLocation, NewLocation, ECC_Visibility, QueryParams))
	{
		SetActorLocation(WallHit.ImpactPoint);
		MulticastOnHookMiss();
		StartReturning();
		return;
	}

	SetActorLocation(NewLocation);

	if (FVector::Dist(LaunchOrigin, NewLocation) >= MaxDistance)
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
	const FVector NextPosition = TargetLocation + Dir * PullSpeed * DeltaTime;
	HookedCharacter->SetActorLocation(NextPosition, false);

	SetActorLocation(HookedCharacter->GetActorLocation());
}

void AHookProjectile::Returning(float DeltaTime)
{
	const FVector OwnerLocation = CachedOwnerCharacter->GetActorLocation();
	const FVector CurrentLocation = GetActorLocation();
	const float Dist = FVector::Dist(CurrentLocation, OwnerLocation);

	if (Dist <= ReturnArrivalDistance)
	{
		FinishHook();
	}
	else
	{
		const FVector Dir = (OwnerLocation - CurrentLocation).GetSafeNormal();
		SetActorLocation(CurrentLocation + Dir * ReturnSpeed * DeltaTime);
	}
}

void AHookProjectile::OnHitSomething(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority() || CurrentState != EHookState::Traveling)
	{
		return;
	}

	if (OtherActor == GetOwner() || OtherActor == this)
	{
		return;
	}

	AMyCharacter* HitCharacter = Cast<AMyCharacter>(OtherActor);
	if (!HitCharacter)
	{
		return;
	}

	if (!IsPlayerTeam(HitCharacter))
	{
		return;
	}

	FHitResult WallCheck;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(HitCharacter);

	if (GetWorld()->LineTraceSingleByChannel(
		WallCheck,
		GetActorLocation(),
		HitCharacter->GetActorLocation(),
		ECC_Visibility,
		QueryParams))
	{
		return;
	}

	StartPulling(HitCharacter);
}

void AHookProjectile::StartPulling(AMyCharacter* TargetCharacter)
{
	CurrentState = EHookState::Pulling;
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

	MulticastOnHookHit();
}

void AHookProjectile::ReleasePulledPlayer()
{
	if (HookedCharacter.IsValid())
	{
		UnfreezeCharacter(HookedCharacter.Get());
		HookedCharacter = nullptr;
	}
}

void AHookProjectile::StartReturning()
{
	CurrentState = EHookState::Returning;

	if (CachedOwnerCharacter.IsValid())
	{
		const float Dist = FVector::Dist(GetActorLocation(), CachedOwnerCharacter->GetActorLocation());
		ReturnSpeed = Dist / FMath::Max(ReelingTime, 0.01f);
	}
	else
	{
		ReturnSpeed = HookSpeed;
	}

	CollisionSphere->SetGenerateOverlapEvents(false);
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
	// Placeholder VFX/SFX hook finished
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