#include "Player/PlayerObjective.h"
#include "Global/MyBaseLevelGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

APlayerObjective::APlayerObjective()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void APlayerObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !IsCompleted && PlayerInteractingWithCount > 0)
	{
		ObjectiveProgression += DeltaTime * PlayerInteractingWithCount;

		if (ObjectiveProgression >= ObjectiveGoal)
		{
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

bool APlayerObjective::InteractWith()
{
	if (!IsCompleted)
	{
		PlayerInteractingWithCount++;

		return true;
	}

	return false;
}

void APlayerObjective::StopInteractWith()
{
	PlayerInteractingWithCount--;
}

void APlayerObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerObjective, ObjectiveProgression);
}
