#include "Player/MyCharacter.h"
#include "Player/MyPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Global/MyBaseLevelGameState.h"
#include "Global/MyGameInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interface/Interactable.h"
#include "UI/ProgressBarWidget.h"

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

	ProgressBar = CreateDefaultSubobject<UWidgetComponent>("Progress Bar");

	ProgressBar->SetupAttachment(GetCapsuleComponent());
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyCharacter::OnHit);

	if (IsLocallyControlled())
	{
		PostProcess->Settings = DefaultPostProcess;
	}

	MyBLGS = Cast<AMyBaseLevelGameState>(UGameplayStatics::GetGameState(GetWorld()));

	ProgressBar->SetVisibility(false, true);

	if (UProgressBarWidget* ProgressBarWidget = Cast<UProgressBarWidget>(ProgressBar->GetUserWidgetObject()))
	{
		ProgressBarWidget->SetProgressBarTextVisibility(!IsLocallyControlled());
	}

	StopCollidingWithCamera();
}

void AMyCharacter::StopCollidingWithCamera()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !IsResurrectionComplete && MyBLGS && MyBLGS->CanBeResurrected(GetController()) && PlayerInteractingWithCount > 0)
	{
		ResurrectProgression += DeltaTime * PlayerInteractingWithCount;

		if (ResurrectProgression >= ResurrectDuration)
		{
			MyBLGS->ResurrectPlayer(GetController());

			IsResurrectionComplete = true;
		}

		DisplayResurrectProgression();
	}

	if (APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		ProgressBar->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(ProgressBar->GetComponentLocation(), CamManager->GetCameraLocation()));
	}
}

void AMyCharacter::DisplayResurrectProgression()
{
	Cast<UProgressBarWidget>(ProgressBar->GetUserWidgetObject())->SetProgress(ResurrectProgression / ResurrectDuration);
}

bool AMyCharacter::InteractWith()
{
	if (MyBLGS && MyBLGS->CanBeResurrected(GetController()))
	{
		PlayerInteractingWithCount++;

		return true;
	}

	return false;
}

void AMyCharacter::StopInteractWith()
{
	PlayerInteractingWithCount--;
}

void AMyCharacter::PreparePlayerForEndScreen()
{
	ProgressBar->SetVisibility(false, true);

	if (IsFlashlightOn)
	{
		ToggleFlashlight();
	}

	IsLoadingEndScreen = true;
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

    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

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

		InteractableActor = nullptr;
	}
}

void AMyCharacter::KillPlayer()
{
	IsDead = true;

	OnPlayerDeathStatusChanged();

	StopInteractingWithActor();

	IsFlashlightOn = false;

	SetFlashlightVisibility();
}

void AMyCharacter::ResurrectPlayer()
{
	IsDead = false;

	OnPlayerDeathStatusChanged();
}

void AMyCharacter::OnPlayerDeathStatusChanged()
{
	GetMesh()->SetSimulatePhysics(IsDead);

	GetMesh()->SetCollisionProfileName(IsDead ? "Ragdoll" : "CharacterMesh");

	GetCapsuleComponent()->SetCollisionProfileName(IsDead ? "Ragdoll" : "Pawn");

	StopCollidingWithCamera();

	ProgressBar->SetVisibility(!IsLoadingEndScreen && IsDead && ResurrectProgression < ResurrectDuration && Cast<UMyGameInstance>(GetGameInstance())->GetCustomPlayerData().CurrentTeam == ETeam::PLAYER, true);

	if (!IsDead)
	{
		GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -90.f));

		GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0));
	}
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyCharacter, IsFlashlightOn);

	DOREPLIFETIME(AMyCharacter, IsDead);

	DOREPLIFETIME(AMyCharacter, ResurrectProgression);
}
