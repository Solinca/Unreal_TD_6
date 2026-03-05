#include "Player/Capacity/HookProjectile.h"
#include "Player/MyCharacter.h"
#include "NiagaraComponent.h"
#include "Global/MyBaseLevelGameState.h"
#include "Kismet/GameplayStatics.h"

AHookProjectile::AHookProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>("HookMesh");

	HookMesh->SetupAttachment(RootComponent);

	BeamNiagara = CreateDefaultSubobject<UNiagaraComponent>("BeamNiagara");

	BeamNiagara->SetupAttachment(RootComponent);
}

void AHookProjectile::InitHook(const FVector& InLaunchDirection, float InHookSpeed, float InReelingTime)
{
	ReelingTime = InReelingTime;

	LaunchOrigin = GetOwner()->GetActorLocation() + FVector(0, 0, 70);

	LaunchVelocity = InLaunchDirection.GetSafeNormal() * InHookSpeed;

	Cast<AMyCharacter>(GetOwner())->SnarePlayerServerSide();

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	ActorsToIgnore.Add(this);

	ActorsToIgnore.Add(GetOwner());

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

	MulticastUpdateAbility(GetHookTipLocation(), CurrentState == EHookState::Traveling ? DeltaTime : 0);
}

FVector AHookProjectile::GetHookTipLocation()
{
	if (CurrentState == EHookState::Traveling)
	{
		return LaunchOrigin + LaunchVelocity * FlightTime;
	}

	return FMath::Lerp(LaunchOrigin, ReturnStartLocation, ReturnAlpha);
}

void AHookProjectile::Traveling(float DeltaTime)
{
	FlightTime += DeltaTime;

	TArray<AActor*> OutActors;

	if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetHookTipLocation(), CollisionRadius, ObjectTypes, nullptr, ActorsToIgnore, OutActors))
	{
		AMyCharacter* HitCharacter = Cast<AMyCharacter>(OutActors[0]);
		
		if (HitCharacter && HitCharacter->Tags.Contains("PLAYER"))
		{
			if (AMyBaseLevelGameState* GS = Cast<AMyBaseLevelGameState>(UGameplayStatics::GetGameState(GetWorld())))
			{
				GS->KillPlayer(HitCharacter->GetController());
			}
		}
	
		StartReturning();
	
		return;
	}

	if (FlightTime >= 1)
	{
		StartReturning();
	}
}

void AHookProjectile::StartReturning()
{
	ReturnStartLocation = GetHookTipLocation();

	CurrentState = EHookState::Returning;
}

void AHookProjectile::Returning(float DeltaTime)
{
	ReturnAlpha -= DeltaTime / ReelingTime;

	if (ReturnAlpha <= 0)
	{
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

	if (DeltaTime > 0)
	{
		HookMesh->AddLocalRotation(FRotator(0, 0, -720 * DeltaTime));
	}
}
