#include "Player/PlayerObjective.h"
#include "Global/MyBaseLevelGameState.h"
#include "Global/MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "UI/ProgressBarWidget.h"

APlayerObjective::APlayerObjective()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");

	Mesh->SetupAttachment(RootComponent);

	ProgressBar = CreateDefaultSubobject<UWidgetComponent>("Progress Bar");

	ProgressBar->SetupAttachment(Mesh);
}

void APlayerObjective::BeginPlay()
{
	Super::BeginPlay();

	if (!NiagaraComponent)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(Vfx, RootComponent, NAME_None, FVector(30, 0, 40), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, false);
	}

	Cast<UProgressBarWidget>(ProgressBar->GetUserWidgetObject())->SetProgressBarTextVisibility(Cast<UMyGameInstance>(GetGameInstance())->GetCustomPlayerData().CurrentTeam == ETeam::PLAYER);
}

void APlayerObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !IsCompleted && PlayerInteractingWithCount > 0)
	{
		ObjectiveProgression += DeltaTime * PlayerInteractingWithCount;

		if (ObjectiveProgression >= ObjectiveGoal)
		{
			ToggleEffects(false);
			
			IsCompleted = true;

			ObjectiveProgression = ObjectiveGoal;

			Cast<AMyBaseLevelGameState>(UGameplayStatics::GetGameState(GetWorld()))->RegisterPlayerObjectiveCompleted();
		}

		DisplayObjectiveProgression();
	}

	if (APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		ProgressBar->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(ProgressBar->GetComponentLocation(), CamManager->GetCameraLocation()));
	}
}

void APlayerObjective::DisplayObjectiveProgression()
{
	Cast<UProgressBarWidget>(ProgressBar->GetUserWidgetObject())->SetProgress(ObjectiveProgression / ObjectiveGoal);

	if (ObjectiveProgression >= ObjectiveGoal)
	{
		Cast<UProgressBarWidget>(ProgressBar->GetUserWidgetObject())->SetProgressBarTextVisibility(false);
	}
}

void APlayerObjective::ToggleEffects_Implementation(const bool bShouldActivate)
{
	if (bShouldActivate)
	{
		OnGoingAudioComponent = UGameplayStatics::SpawnSoundAttached(SFX, GetRootComponent());

		NiagaraComponent->Activate(true);
	}
	else
	{
		OnGoingAudioComponent->Stop();

		NiagaraComponent->Deactivate();
	}
}

bool APlayerObjective::InteractWith()
{
	if (!IsCompleted)
	{
		if (PlayerInteractingWithCount == 0)
		{
			ToggleEffects(true);
		}

		PlayerInteractingWithCount++;

		return true;
	}

	return false;
}

void APlayerObjective::StopInteractWith()
{
	PlayerInteractingWithCount--;

	if (PlayerInteractingWithCount <= 0)
	{
		ToggleEffects(false);
	}
}

void APlayerObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerObjective, ObjectiveProgression);
}
