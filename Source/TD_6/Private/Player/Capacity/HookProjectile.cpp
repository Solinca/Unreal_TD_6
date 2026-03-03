#include "Player/Capacity/HookProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/MyCharacter.h"
#include "Player/Capacity/HookAbilityComponent.h"

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
	HookMesh->SetAbsolute(true, true, false);

	BeamNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BeamNiagara"));
	BeamNiagara->SetupAttachment(CollisionSphere);
	BeamNiagara->SetAutoActivate(false);
	BeamNiagara->SetIsReplicated(false);
	BeamNiagara->SetAbsolute(true, true, true);
}

void AHookProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHookProjectile, CurrentState);
	DOREPLIFETIME(AHookProjectile, LaunchOrigin);
	DOREPLIFETIME(AHookProjectile, LaunchVelocity);
	DOREPLIFETIME(AHookProjectile, FlightTime);
	DOREPLIFETIME(AHookProjectile, ReturnStartLocation);
	DOREPLIFETIME(AHookProjectile, ReturnAlpha);
	DOREPLIFETIME(AHookProjectile, ReelingTime);
}

void AHookProjectile::OnRep_CurrentState()
{
	if (CurrentState == EHookState::Finished && BeamNiagara)
	{
		BeamNiagara->Deactivate();
	}

	if (CurrentState == EHookState::Returning)
	{
		LocalReturnAlpha = ReturnAlpha;
	}

	if (CurrentState == EHookState::Traveling)
	{
		LocalFlightTime = FlightTime;
		EnsureBeamActive();
	}
}

void AHookProjectile::InitHook(UHookAbilityComponent* InAbility, const FVector& InLaunchDirection, float InMaxDistance, float InHookSpeed, float InReelingTime)
{
	OwningAbility = InAbility;
	MaxDistance = InMaxDistance;
	HookSpeed = InHookSpeed;
	ReelingTime = InReelingTime;

	LaunchOrigin = GetActorLocation();
	LaunchVelocity = InLaunchDirection.GetSafeNormal() * HookSpeed;
	CurrentState = EHookState::Traveling;
	FlightTime = 0.f;
	LocalFlightTime = 0.f;

	if (AMyCharacter* OwnerChara = Cast<AMyCharacter>(GetOwner()))
	{
		CachedOwnerCharacter = OwnerChara;
	}

	UpdateHookPosition();

	if (BeamNiagara && BeamNiagaraSystem)
	{
		BeamNiagara->SetAsset(BeamNiagaraSystem);
		BeamNiagara->Activate(true);
		UpdateBeam();
	}
}

void AHookProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
	{
		ClientSimulate(DeltaTime);
		return;
	}

	if (CurrentState == EHookState::Finished)
	{
		return;
	}

	if (!CachedOwnerCharacter.IsValid())
	{
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

	case EHookState::Returning:
		Returning(DeltaTime);
		break;

	default:
		break;
	}

	UpdateHookPosition();
	UpdateBeam();
}

void AHookProjectile::ClientSimulate(float DeltaTime)
{
	if (CachedOwnerCharacter.IsValid())
	{
		SetActorLocation(CachedOwnerCharacter->GetActorLocation());
	}
	else if (const AActor* OwnerActor = GetOwner())
	{
		SetActorLocation(OwnerActor->GetActorLocation());
	}

	if (CurrentState == EHookState::Finished || CurrentState == EHookState::Idle)
	{
		return;
	}

	EnsureBeamActive();

	if (CurrentState == EHookState::Traveling)
	{
		LocalFlightTime += DeltaTime;
	}
	else if (CurrentState == EHookState::Returning)
	{
		const float ReturnSpeed = 1.f / FMath::Max(ReelingTime, 0.01f);
		LocalReturnAlpha -= ReturnSpeed * DeltaTime;
		LocalReturnAlpha = FMath::Max(LocalReturnAlpha, 0.f);
	}

	UpdateHookPosition();
	UpdateBeam();
}

FVector AHookProjectile::ComputeBallisticPosition(float T) const
{
	return LaunchOrigin + LaunchVelocity * T + FVector(0.f, 0.f, -0.5f * HookGravity * T * T);
}

FVector AHookProjectile::ComputeBallisticVelocity(float T) const
{
	return LaunchVelocity + FVector(0.f, 0.f, -HookGravity * T);
}

FVector AHookProjectile::GetHookTipLocation() const
{
	const float UseFlightTime = HasAuthority() ? FlightTime : LocalFlightTime;
	const float UseReturnAlpha = HasAuthority() ? ReturnAlpha : LocalReturnAlpha;

	if (CurrentState == EHookState::Traveling)
	{
		return ComputeBallisticPosition(UseFlightTime);
	}

	if (CurrentState == EHookState::Returning)
	{
		const FVector OwnerLoc = CachedOwnerCharacter.IsValid()
			? CachedOwnerCharacter->GetActorLocation()
			: GetActorLocation();
		return FMath::Lerp(OwnerLoc, ReturnStartLocation, UseReturnAlpha);
	}

	return GetActorLocation();
}

void AHookProjectile::UpdateHookPosition()
{
	const FVector TipLocation = GetHookTipLocation();

	if (!HookMesh)
	{
		return;
	}

	HookMesh->SetWorldLocation(TipLocation);

	if (CurrentState == EHookState::Traveling)
	{
		CurrentSpinAngle = FMath::Fmod(CurrentSpinAngle + SpinSpeed * GetWorld()->GetDeltaSeconds(), 360.f);

		const float UseFlightTime = HasAuthority() ? FlightTime : LocalFlightTime;
		const FVector CurrentVel = ComputeBallisticVelocity(UseFlightTime);
		const FQuat BaseQuat = CurrentVel.Rotation().Quaternion();
		const FQuat SpinQuat = FQuat(CurrentVel.GetSafeNormal(), FMath::DegreesToRadians(CurrentSpinAngle));
		HookMesh->SetWorldRotation((SpinQuat * BaseQuat * MeshRotationOffset.Quaternion()).Rotator());
	}
	else if (CurrentState == EHookState::Returning)
	{
		const FVector OwnerLoc = CachedOwnerCharacter.IsValid()
			? CachedOwnerCharacter->GetActorLocation()
			: GetActorLocation();
		const FVector ReturnDir = (OwnerLoc - TipLocation).GetSafeNormal();

		if (!ReturnDir.IsNearlyZero())
		{
			const FQuat BaseQuat = ReturnDir.Rotation().Quaternion();
			const FQuat SpinQuat = FQuat(ReturnDir, FMath::DegreesToRadians(CurrentSpinAngle));
			HookMesh->SetWorldRotation((SpinQuat * BaseQuat * MeshRotationOffset.Quaternion()).Rotator());
		}
	}
}

void AHookProjectile::UpdateBeam()
{
	if (!BeamNiagara || !BeamNiagara->IsActive())
	{
		return;
	}

	const FVector OwnerLoc = CachedOwnerCharacter.IsValid()
		? CachedOwnerCharacter->GetActorLocation()
		: GetActorLocation();

	BeamNiagara->SetVariableVec3(BeamStartParamName, OwnerLoc);
	BeamNiagara->SetVariableVec3(BeamEndParamName, GetHookTipLocation());
}

void AHookProjectile::EnsureBeamActive()
{
	if (BeamNiagara && BeamNiagaraSystem && !BeamNiagara->IsActive())
	{
		BeamNiagara->SetAsset(BeamNiagaraSystem);
		BeamNiagara->Activate(true);
	}
}

void AHookProjectile::Traveling(float DeltaTime)
{
	const float PreviousTime = FlightTime;
	FlightTime += DeltaTime;

	const FVector PrevPos = ComputeBallisticPosition(PreviousTime);
	const FVector NewPos = ComputeBallisticPosition(FlightTime);
	const float DistFromOrigin = FVector::Dist(LaunchOrigin, NewPos);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult WallHit;
	if (GetWorld()->LineTraceSingleByChannel(WallHit, PrevPos, NewPos, ECC_Visibility, QueryParams))
	{
		FlightTime = PreviousTime + DeltaTime * WallHit.Time;
		MulticastOnHookMiss();
		StartReturning();
		return;
	}

	FHitResult CharacterHit;
	if (GetWorld()->SweepSingleByChannel(
		CharacterHit, PrevPos, NewPos, FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(CollisionRadius), QueryParams))
	{
		AMyCharacter* HitCharacter = Cast<AMyCharacter>(CharacterHit.GetActor());

		if (HitCharacter && HitCharacter != GetOwner() && HitCharacter->Tags.Contains("PLAYER"))
		{
			FCollisionQueryParams WallCheckParams;
			WallCheckParams.AddIgnoredActor(this);
			WallCheckParams.AddIgnoredActor(GetOwner());
			WallCheckParams.AddIgnoredActor(HitCharacter);

			const FVector OwnerLoc = CachedOwnerCharacter.IsValid()
				? CachedOwnerCharacter->GetActorLocation()
				: LaunchOrigin;

			FHitResult WallCheck;
			if (!GetWorld()->LineTraceSingleByChannel(
				WallCheck, OwnerLoc, HitCharacter->GetActorLocation(),
				ECC_Visibility, WallCheckParams))
			{
				FlightTime = PreviousTime + DeltaTime * CharacterHit.Time;
				MulticastOnHookHit();
				StartReturning();
				return;
			}
		}
	}

	if (DistFromOrigin >= MaxDistance)
	{
		MulticastOnHookMiss();
		StartReturning();
	}
}

void AHookProjectile::Returning(float DeltaTime)
{
	const float ReturnSpeed = 1.f / FMath::Max(ReelingTime, 0.01f);
	ReturnAlpha -= ReturnSpeed * DeltaTime;

	if (ReturnAlpha <= 0.f)
	{
		ReturnAlpha = 0.f;
		FinishHook();
	}
}

void AHookProjectile::StartReturning()
{
	ReturnStartLocation = GetHookTipLocation();
	ReturnAlpha = 1.f;
	LocalReturnAlpha = 1.f;
	CurrentState = EHookState::Returning;
}

void AHookProjectile::FinishHook()
{
	CurrentState = EHookState::Finished;

	if (BeamNiagara)
	{
		BeamNiagara->Deactivate();
	}

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

	if (BeamNiagara)
	{
		BeamNiagara->Deactivate();
	}

	Destroy();
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
}