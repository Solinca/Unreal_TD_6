#include "Global/MyGameState.h"
#include "Player/MyLobbyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void AMyGameState::DisplayEveryPlayerInLobby()
{
	if (AMyLobbyPlayerController* MLPC = Cast<AMyLobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		MLPC->DisplayLobbyInfoOnClient(PlayerDataList, CurrentSessionName, CurrentMaxPlayerConnectionAmount, CurrentMaxMonsterAmount);
	}
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

void AMyGameState::RegisterPlayerData(const FCustomPlayerData& CustomPlayerData)
{
	PlayerDataList.Add(CustomPlayerData);

	DisplayEveryPlayerInLobby();
}

void AMyGameState::RemovePlayerData(AController* Controller)
{
	if (Controller->IsLocalController())
	{
		DestroyGame();

		return;
	}

	for (FCustomPlayerData Data : PlayerDataList)
	{
		if (Data.CustomPlayerID == Controller->GetPlayerState<APlayerState>()->GetUniqueId())
		{
			PlayerDataList.Remove(Data);

			break;
		}
	}

	DisplayEveryPlayerInLobby();
}

void AMyGameState::StartLobbyIfReady_Implementation()
{
	int PlayerCount = 0;

	int MonsterCount = 0;

	for (FCustomPlayerData PlayerData : PlayerDataList)
	{
		if (PlayerData.CurrentTeam == ETeam::PLAYER)
		{
			PlayerCount++;
		}
		else if (PlayerData.CurrentTeam == ETeam::MONSTER)
		{
			MonsterCount++;
		}
	}

	if (PlayerCount > 0 && MonsterCount > 0 && PlayerCount <= (CurrentMaxPlayerConnectionAmount - CurrentMaxMonsterAmount) && MonsterCount <= CurrentMaxMonsterAmount)
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			if (AMyLobbyPlayerController* MyLPC = Cast<AMyLobbyPlayerController>(It->Get()))
			{
				MyLPC->TriggerLobbyAnimation();
			}
		}
	}
}

void AMyGameState::ChangePlayerCurrentTeam_Implementation(AController* Controller, ETeam NewTeam)
{
	for (FCustomPlayerData& Data : PlayerDataList)
	{
		if (Data.CustomPlayerID == Controller->GetPlayerState<APlayerState>()->GetUniqueId())
		{
			Data.CurrentTeam = NewTeam;

			Cast<AMyLobbyPlayerController>(Controller)->UpdatePlayerTeam(NewTeam);

			break;
		}
	}

	DisplayEveryPlayerInLobby();
}

void AMyGameState::DestroyGame_Implementation()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		if (AMyLobbyPlayerController* MyLPC = Cast<AMyLobbyPlayerController>(It->Get()))
		{
			MyLPC->DestroySessionOnClient();
		}
	}
}
