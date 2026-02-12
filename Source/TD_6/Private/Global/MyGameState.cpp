#include "Global/MyGameState.h"
#include "Global/MyPlayerState.h"
#include "Player/MyLobbyPlayerController.h"
#include "Net/UnrealNetwork.h"

// TODO: Comprendre pourquoi DisplayEveryPlayer n'est pas call / répliqué à l'ajout / retrait

void AMyGameState::PlayerJoined_Implementation(AController* Controller)
{
	PlayerList.Add(Controller->GetPlayerState<AMyPlayerState>());

	DisplayEveryPlayer();
}

void AMyGameState::PlayerLeft_Implementation(AController* Controller)
{
	PlayerList.Remove(Controller->GetPlayerState<AMyPlayerState>());

	DisplayEveryPlayer();
}

void AMyGameState::DisplayEveryPlayer_Implementation()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		if (AMyLobbyPlayerController* PC = Cast<AMyLobbyPlayerController>(It->Get()))
		{
			PC->DisplayLobbyInfoOnClient(PlayerList, CurrentSessionName, CurrentMaxPlayerConnectionAmount, CurrentMaxMonsterAmount);
		}
	}
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, PlayerList);
}

void AMyGameState::SetupCurrentSession_Implementation(const FString& SessionName, int MaxConnectionAmount, int MaxMonsterAmount)
{
	CurrentSessionName = SessionName;

	CurrentMaxPlayerConnectionAmount = MaxConnectionAmount;

	CurrentMaxMonsterAmount = MaxMonsterAmount;
}
