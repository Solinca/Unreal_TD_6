#include "Global/MyGameState.h"
#include "Net/UnrealNetwork.h"

void AMyGameState::PlayerJoined_Implementation(const FString& PlayerName)
{
	PlayerNameList.Add(PlayerName);
}

void AMyGameState::PlayerLeft_Implementation(const FString& PlayerName)
{
	PlayerNameList.Remove(PlayerName);
}

void AMyGameState::DisplayEveryPlayer()
{
	for (FString Name : PlayerNameList)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, Name);
	}
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, PlayerNameList);
}
