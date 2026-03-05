#include "Player/Capacity/HookProjectile.h"
#include "Player/MyCharacter.h"
#include "NiagaraComponent.h"
#include "Global/MyBaseLevelGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AHookProjectile::AHookProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("Sphere Collision");

	SphereCollision->SetupAttachment(RootComponent);

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>("HookMesh");

	HookMesh->SetupAttachment(SphereCollision);

	BeamNiagara = CreateDefaultSubobject<UNiagaraComponent>("BeamNiagara");

	BeamNiagara->SetupAttachment(RootComponent);
}

void AHookProjectile::InitHook(const FVector& InLaunchDirection, float InHookSpeed, float InReelingTime)
{
	ReelingTime = InReelingTime;

	LaunchOrigin = GetOwner()->GetActorLocation() + FVector(0, 0, 70);

	LaunchVelocity = InLaunchDirection.GetSafeNormal() * InHookSpeed;

	Cast<AMyCharacter>(GetOwner())->SnarePlayerServerSide();

	SphereCollision->OnComponentHit.AddDynamic(this, &AHookProjectile::OnHit);

	MulticastStartAbility();
}

void AHookProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || CurrentState == EHookState::Finished)
	{
		return;
	}

	SetActorLocation(GetHookLocation(), true);

	MulticastUpdateAbility(GetHookLocation(), CurrentState == EHookState::Traveling ? DeltaTime : 0);

	if (CurrentState == EHookState::Traveling)
	{
		FlightTime += DeltaTime;

		if (FlightTime >= 1)
		{
			ReturnStartLocation = GetHookLocation();

			CurrentState = EHookState::Returning;
		}

		HookMesh->AddLocalRotation(FRotator(0, 0, -720 * DeltaTime));
	}
	else if (CurrentState == EHookState::Returning)
	{
		ReturnAlpha -= DeltaTime / ReelingTime;

		if (ReturnAlpha <= 0)
		{
			CurrentState = EHookState::Finished;

			Cast<AMyCharacter>(GetOwner())->StopAbilitySFX();

			Cast<AMyCharacter>(GetOwner())->ReleasePlayerServerSide();

			Destroy();
		}
	}
}

void AHookProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this || OtherActor == GetOwner() || CurrentState != EHookState::Traveling)
	{
		return;
	}

	AMyCharacter* HitCharacter = Cast<AMyCharacter>(OtherActor);

	if (HitCharacter && HitCharacter->Tags.Contains("PLAYER"))
	{
		if (AMyBaseLevelGameState* GS = Cast<AMyBaseLevelGameState>(UGameplayStatics::GetGameState(GetWorld())))
		{
			GS->KillPlayer(HitCharacter->GetController());
		}
	}

	ReturnStartLocation = GetHookLocation();

	CurrentState = EHookState::Returning;
}

FVector AHookProjectile::GetHookLocation()
{
	if (CurrentState == EHookState::Traveling)
	{
		return LaunchOrigin + LaunchVelocity * FlightTime;
	}

	return FMath::Lerp(LaunchOrigin, ReturnStartLocation, ReturnAlpha);
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

	SetActorLocation(TargetPosition);

	if (DeltaTime > 0)
	{
		HookMesh->AddLocalRotation(FRotator(0, 0, -720 * DeltaTime));
	}
}
