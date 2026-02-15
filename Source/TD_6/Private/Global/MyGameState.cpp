#include "Global/MyGameState.h"
#include "Player/MyLobbyPlayerController.h"
#include "Net/UnrealNetwork.h"

void AMyGameState::DisplayEveryPlayerInLobby()
{
	if (AMyLobbyPlayerController* MLPC = Cast<AMyLobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		MLPC->DisplayLobbyInfoOnClient(PlayerDataList, CurrentSessionName, CurrentMaxPlayerConnectionAmount, CurrentMaxMonsterAmount);
	}
}

void AMyGameState::PlayerJoined_Implementation(AController* Controller)
{
	PlayerList.Add(Controller->GetPlayerState<AMyPlayerState>());

	PlayerDataList.Empty();

	for (AMyPlayerState* MPS : PlayerList)
	{
		PlayerDataList.Add(MPS->GetCustomPlayerData());
	}

	DisplayEveryPlayerInLobby();
}

void AMyGameState::PlayerLeft_Implementation(AController* Controller)
{
	PlayerList.Remove(Controller->GetPlayerState<AMyPlayerState>());

	PlayerDataList.Empty();

	for (AMyPlayerState* MPS : PlayerList)
	{
		PlayerDataList.Add(MPS->GetCustomPlayerData());
	}

	DisplayEveryPlayerInLobby();
}

void AMyGameState::SetupCurrentSession_Implementation(const FString& SessionName, int MaxConnectionAmount, int MaxMonsterAmount)
{
	CurrentSessionName = SessionName;

	CurrentMaxPlayerConnectionAmount = MaxConnectionAmount;

	CurrentMaxMonsterAmount = MaxMonsterAmount;

	DisplayEveryPlayerInLobby();
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, CurrentSessionName);

	DOREPLIFETIME(AMyGameState, CurrentMaxPlayerConnectionAmount);

	DOREPLIFETIME(AMyGameState, CurrentMaxMonsterAmount);

	DOREPLIFETIME(AMyGameState, PlayerDataList);
}
