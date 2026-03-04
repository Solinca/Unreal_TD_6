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

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>("HookMesh");

	HookMesh->SetupAttachment(RootComponent);

	BeamNiagara = CreateDefaultSubobject<UNiagaraComponent>("BeamNiagara");

	BeamNiagara->SetupAttachment(RootComponent);
}

void AHookProjectile::InitHook(const FVector& InLaunchDirection, float InMaxDistance, float InHookSpeed, float InReelingTime)
{
	MaxDistance = InMaxDistance;

	ReelingTime = InReelingTime;

	LaunchOrigin = GetOwner()->GetActorLocation() + FVector(0, 0, 70);

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
	MulticastUpdateAbility(GetHookTipLocation(), CurrentState == EHookState::Traveling ? DeltaTime : 0.f);
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

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	
	TArray<AActor*> OutActors;
	
	TArray<AActor*> ActorsToIgnore;
	
	ActorsToIgnore.Add(this);
	
	ActorsToIgnore.Add(GetOwner());

	if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), HookMesh->GetComponentLocation(), 10.f, ObjectTypes, nullptr, ActorsToIgnore, OutActors))
	{
		AMyCharacter* HitCharacter = Cast<AMyCharacter>(OutActors[0]);
		
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

void AHookProjectile::MulticastUpdateAbility_Implementation(const FVector& TargetPosition, const float DeltaTime)
{
	BeamNiagara->SetVariableVec3(BeamEndParamName, TargetPosition);

	HookMesh->SetWorldLocation(TargetPosition);

	if (DeltaTime > 0.f)
	{
		HookMesh->AddLocalRotation(FRotator(0.f, 0.f, -720.f * DeltaTime));
	}
}

void AHookProjectile::MulticastOnHookHit_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, TEXT("Hook HIT!"));
}

void AHookProjectile::MulticastOnHookMiss_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Hook MISS"));
}
