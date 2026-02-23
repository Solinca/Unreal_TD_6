#include "Player/PlayerObjective.h"
#include "Net/UnrealNetwork.h"

APlayerObjective::APlayerObjective()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void APlayerObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsCompleted && PlayerInteractingWithCount > 0)
	{
		ObjectiveProgression += DeltaTime * PlayerInteractingWithCount;

		if (ObjectiveProgression >= ObjectiveGoal)
		{
			IsCompleted = true;
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
	PlayerInteractingWithCount++;

	return !IsCompleted;
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
