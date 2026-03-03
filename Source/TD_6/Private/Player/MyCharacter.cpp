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
#include "GameFramework/CharacterMovementComponent.h"
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

	Flashlight->SetupAttachment(GetMesh(), "clavicle_r");

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

	DefaultMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;

	InitDynamicMaterials();

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

void AMyCharacter::InitDynamicMaterials()
{
	if (DynamicMaterials.IsEmpty())
	{
		const int32 NumMaterials = GetMesh()->GetNumMaterials();

		for (int32 i = 0; i < NumMaterials; ++i)
		{
			if (UMaterialInstanceDynamic* MaterialInstanceDynamic = GetMesh()->CreateDynamicMaterialInstance(i))
			{
				DynamicMaterials.Add(MaterialInstanceDynamic);
			}
		}
	}
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
	ForceDisableProgressBarOnAllClients();

	DisableFlashlight();
}

void AMyCharacter::ForceDisableProgressBarOnAllClients_Implementation()
{
	HasToForceDisableProgressBar = true;

	ProgressBar->SetVisibility(false, true);
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

void AMyCharacter::ChangePostProcess_Implementation(const bool bIsDefault) const
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

void AMyCharacter::DisableFlashlight()
{
	IsFlashlightOn = false;

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

	DisableFlashlight();

	if (MyBLGS->CanBeResurrected(GetController()))
	{
		GetWorld()->GetTimerManager().SetTimer(ScreamHandle, this, &AMyCharacter::PlayerScreamRandomizer, (FMath::Rand() % 5) + 3, false);
	}
}

void AMyCharacter::PlayerScreamRandomizer()
{
	TriggerPlayerScreamOnAllClient();

	GetWorld()->GetTimerManager().SetTimer(ScreamHandle, this, &AMyCharacter::PlayerScreamRandomizer, (FMath::Rand() % 5) + 3, false);
}

void AMyCharacter::TriggerPlayerScreamOnAllClient_Implementation()
{
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PlayerHelpScream, GetActorLocation());
}

void AMyCharacter::ResurrectPlayer()
{
	IsDead = false;

	GetWorld()->GetTimerManager().ClearTimer(ScreamHandle);

	OnPlayerDeathStatusChanged();
}

void AMyCharacter::OnPlayerDeathStatusChanged()
{
	GetMesh()->SetSimulatePhysics(IsDead);

	GetMesh()->SetCollisionProfileName(IsDead ? "Ragdoll" : "CharacterMesh");

	GetCapsuleComponent()->SetCollisionProfileName(IsDead ? "Ragdoll" : "Pawn");

	StopCollidingWithCamera();

	ProgressBar->SetVisibility(!HasToForceDisableProgressBar && IsDead && ResurrectProgression < ResurrectDuration && Cast<UMyGameInstance>(GetGameInstance())->GetCustomPlayerData().CurrentTeam == ETeam::PLAYER, true);

	if (IsDead)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ResurrectProgression < ResurrectDuration ? PlayerHurtSound : PlayerDeathSound, GetActorLocation());
	}

	if (!IsDead)
	{
		GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -90.f));

		GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0));
	}
}

void AMyCharacter::SnarePlayerServerSide_Implementation()
{
	GetCharacterMovement()->DisableMovement();

	SnarePlayerClientSide();
}

void AMyCharacter::SnarePlayerClientSide_Implementation()
{
	GetCharacterMovement()->DisableMovement();
}

void AMyCharacter::ReleasePlayerServerSide_Implementation()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	ReleasePlayerClientSide();
}

void AMyCharacter::ReleasePlayerClientSide_Implementation()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AMyCharacter::SetPlayerMovementSpeedServerSide_Implementation(bool IsDefaultSpeed, bool IsSprinting, float NewSpeed)
{
	float CalculatedSpeed = IsDefaultSpeed ? DefaultMaxSpeed : (IsSprinting ? DefaultMaxSpeed * PlayerSprintFactor : NewSpeed);

	GetCharacterMovement()->MaxWalkSpeed = CalculatedSpeed;

	SetPlayerMovementSpeedClientSide(CalculatedSpeed);
}

void AMyCharacter::SetPlayerMovementSpeedClientSide_Implementation(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AMyCharacter::ChangeCharacterScale_Implementation(FVector NewScale)
{
	GetMesh()->SetRelativeScale3D(NewScale);
}

void AMyCharacter::ChangePlayerMaterial_Implementation(FName DissolveParamName, float Value)
{
	for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
	{
		DynamicMaterial->SetScalarParameterValue(DissolveParamName, Value);
	}
}

void AMyCharacter::SetCharacterHighlight_Implementation(ACharacter* Character, bool bHighlight)
{
	if (USkeletalMeshComponent* MeshComp = Character->GetMesh())
	{
		MeshComp->SetRenderCustomDepth(bHighlight);

		MeshComp->SetCustomDepthStencilValue(1);
	}
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyCharacter, IsFlashlightOn);

	DOREPLIFETIME(AMyCharacter, IsDead);

	DOREPLIFETIME(AMyCharacter, ResurrectProgression);
}
