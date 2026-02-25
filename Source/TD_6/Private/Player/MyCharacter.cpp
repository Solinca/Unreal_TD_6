#include "Player/MyCharacter.h"
#include "Player/MyPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Global/MyBaseLevelGameState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Interface/Interactable.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");

	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");

	Camera->SetupAttachment(SpringArm);

	Flashlight = CreateDefaultSubobject<USpotLightComponent>("Flashlight");

	Flashlight->SetupAttachment(RootComponent);

	PostProcess = CreateDefaultSubobject<UPostProcessComponent>("Post Process");

	PostProcess->SetupAttachment(Camera);
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyCharacter::OnHit);

	PostProcess->Settings = DefaultPostProcess;
}

void AMyCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsAttacking && OtherActor->Tags.Contains("PLAYER"))
	{
		if (AMyCharacter* OtherChara = Cast<AMyCharacter>(OtherActor))
		{
			Cast<AMyBaseLevelGameState>(UGameplayStatics::GetGameState(GetWorld()))->KillPlayer(OtherChara->GetController());
		}
	}
}

void AMyCharacter::ChangePostProcess(const bool bIsDefault) const
{
	PostProcess->Settings = bIsDefault ? DefaultPostProcess : NightVisionPostProcess;
}

void AMyCharacter::SetIsAttacking()
{
	IsAttacking = true;

	GetWorld()->GetTimerManager().SetTimer(AttackHandle, this, &AMyCharacter::ResetAttacking, AttackAnimationTime, false);
}

void AMyCharacter::ResetAttacking()
{
	IsAttacking = false;
}

void AMyCharacter::ToggleFlashlight_Implementation()
{
	IsFlashlightOn = !IsFlashlightOn;

	SetFlashlightVisibility();
}

void AMyCharacter::SetFlashlightVisibility()
{
	Flashlight->SetVisibility(IsFlashlightOn, true);
}

void AMyCharacter::InteractWithSurroundingActor_Implementation()
{
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    TArray<AActor*> OutActors;

    TArray<AActor*> ActorsToIgnore;

    ActorsToIgnore.Add(this);

    if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), InteractRange, ObjectTypes, nullptr, ActorsToIgnore, OutActors))
	{
        for (AActor* OverlappedActor : OutActors)
		{
			if (OverlappedActor->Implements<UInteractable>())
			{
				IInteractable* InteractableActor = Cast<IInteractable>(OverlappedActor);

				if (InteractableActor && InteractableActor->InteractWith())
				{
					InteractingActor = OverlappedActor;

					return;
				}
			}
        }
    }
}

void AMyCharacter::StopInteractingWithActor_Implementation()
{
	if (IInteractable* InteractableActor = Cast<IInteractable>(InteractingActor))
	{
		InteractableActor->StopInteractWith();
	}
}

void AMyCharacter::KillPlayer()
{
	IsDead = true;

	OnPlayerDeath();
}

void AMyCharacter::OnPlayerDeath()
{
	if (IsDead)
	{
		GetMesh()->SetSimulatePhysics(true);

		GetMesh()->SetCollisionProfileName("Ragdoll");

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (HasAuthority())
		{
			IsFlashlightOn = false;

			SetFlashlightVisibility();
		}
	}
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyCharacter, IsFlashlightOn);

	DOREPLIFETIME(AMyCharacter, IsDead);
}
