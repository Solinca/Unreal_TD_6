#include "Player/PlayerObjective.h"
#include "Global/MyBaseLevelGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

APlayerObjective::APlayerObjective()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void APlayerObjective::BeginPlay()
{
	Super::BeginPlay();

	if (!NiagaraComponent)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			Vfx,
			RootComponent,
			NAME_None,
			FVector(30, 0, 40), 
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset, 
			false,
			false
		);
	}
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
}

void APlayerObjective::DisplayObjectiveProgression()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::FromInt(ObjectiveProgression));
}

void APlayerObjective::ToggleEffects_Implementation(const bool bShouldActivate)
{
	if (!Vfx)
	{
		return;
	}

	if (!bShouldActivate)
	{
		NiagaraComponent->Deactivate();
		return;
	}

	NiagaraComponent->Activate(true);
}

bool APlayerObjective::InteractWith()
{
	if (!IsCompleted)
	{
		PlayerInteractingWithCount++;
		
		ToggleEffects(PlayerInteractingWithCount > 0);

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
