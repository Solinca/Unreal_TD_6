#include "Player/Capacity/HookProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Global/MyBaseLevelGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MyCharacter.h"

AHookProjectile::AHookProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>("HookMesh");

	HookMesh->SetupAttachment(RootComponent);

	BeamNiagara = CreateDefaultSubobject<UNiagaraComponent>("BeamNiagara");

	BeamNiagara->SetupAttachment(RootComponent);
}

void AHookProjectile::InitHook(const FVector& InLaunchDirection, float InMaxDistance, float InHookSpeed, float InReelingTime)
{
	MaxDistance = InMaxDistance;

	ReelingTime = InReelingTime;

	LaunchOrigin = GetOwner()->GetActorLocation();

	LaunchVelocity = InLaunchDirection.GetSafeNormal() * InHookSpeed;

	Cast<AMyCharacter>(GetOwner())->SnarePlayerServerSide();

	MulticastStartAbility();
}

void AHookProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || CurrentState == EHookState::Finished)
	{
		return;
	}

	if (CurrentState == EHookState::Traveling)
	{
		Traveling(DeltaTime);
	}
	else if (CurrentState == EHookState::Returning)
	{
		Returning(DeltaTime);
	}

	UpdateHookPosition(DeltaTime);
}

void AHookProjectile::UpdateHookPosition(float DeltaTime)
{
	MulticastUpdateAbility(GetHookTipLocation());
}

FVector AHookProjectile::ComputeBallisticPosition(float DeltaTime)
{
	return LaunchOrigin + LaunchVelocity * DeltaTime + FVector(0, 0, -0.5f * HookGravity * DeltaTime * DeltaTime);
}

FVector AHookProjectile::ComputeBallisticVelocity(float DeltaTime)
{
	return LaunchVelocity + FVector(0, 0, -HookGravity * DeltaTime);
}

FVector AHookProjectile::GetHookTipLocation()
{
	if (CurrentState == EHookState::Traveling)
	{
		return ComputeBallisticPosition(FlightTime);
	}

	if (CurrentState == EHookState::Returning)
	{
		return FMath::Lerp(LaunchOrigin, ReturnStartLocation, ReturnAlpha);
	}

	return GetActorLocation();
}

void AHookProjectile::Traveling(float DeltaTime)
{
	const FVector PrevPos = ComputeBallisticPosition(FlightTime);

	FlightTime += DeltaTime;

	const FVector NewPos = ComputeBallisticPosition(FlightTime);

	FCollisionQueryParams QueryParams;

	QueryParams.AddIgnoredActor(this);

	QueryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->SweepSingleByChannel(Hit, PrevPos, NewPos, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(CollisionRadius), QueryParams))
	{
		AMyCharacter* HitCharacter = Cast<AMyCharacter>(Hit.GetActor());

		if (HitCharacter && HitCharacter->Tags.Contains("PLAYER"))
		{
			MulticastOnHookHit();

			if (AMyBaseLevelGameState* GS = Cast<AMyBaseLevelGameState>(UGameplayStatics::GetGameState(GetWorld())))
			{
				GS->KillPlayer(HitCharacter->GetController());
			}
		}
		else
		{
			MulticastOnHookMiss();
		}

		StartReturning();

		return;
	}

	if (FVector::Dist(LaunchOrigin, NewPos) >= MaxDistance)
	{
		MulticastOnHookMiss();

		StartReturning();
	}
}

void AHookProjectile::StartReturning()
{
	ReturnStartLocation = GetHookTipLocation();

	CurrentState = EHookState::Returning;

	ReturnAlpha = 1;
}

void AHookProjectile::Returning(float DeltaTime)
{
	ReturnAlpha -= (1 / FMath::Max(ReelingTime, 0.01f)) * DeltaTime;

	if (ReturnAlpha <= 0)
	{
		ReturnAlpha = 0;

		FinishHook();
	}
}

void AHookProjectile::FinishHook()
{
	CurrentState = EHookState::Finished;

	Cast<AMyCharacter>(GetOwner())->StopAbilitySFX();

	Cast<AMyCharacter>(GetOwner())->ReleasePlayerServerSide();

	Destroy();
}

void AHookProjectile::MulticastStartAbility_Implementation()
{
	BeamNiagara->SetAsset(BeamNiagaraSystem);

	BeamNiagara->Activate(true);

	BeamNiagara->SetVariableVec3(BeamStartParamName, GetOwner()->GetActorLocation());
}

void AHookProjectile::MulticastUpdateAbility_Implementation(FVector TargetPosition)
{
	BeamNiagara->SetVariableVec3(BeamEndParamName, TargetPosition);

	HookMesh->SetWorldLocation(TargetPosition);
}

void AHookProjectile::MulticastOnHookHit_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, TEXT("Hook HIT!"));
}

void AHookProjectile::MulticastOnHookMiss_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Hook MISS"));
}
