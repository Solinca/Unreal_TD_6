#include "Global/MyLobbyGameState.h"
#include "Global/MyLobbyGameMode.h"
#include "Player/MyLobbyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AMyLobbyGameState::DisplayEveryPlayerInLobby()
{
	if (AMyLobbyPlayerController* MLPC = Cast<AMyLobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		MLPC->DisplayLobbyInfoOnClient(PlayerDataList, CurrentSessionName, CurrentMaxPlayerConnectionAmount, CurrentMaxMonsterAmount);
	}
}

void AMyLobbyGameState::SetupCurrentSession_Implementation(const FString& SessionName, int MaxConnectionAmount, int MaxMonsterAmount)
{
	CurrentSessionName = SessionName;

	CurrentMaxPlayerConnectionAmount = MaxConnectionAmount;

	CurrentMaxMonsterAmount = MaxMonsterAmount;

	DisplayEveryPlayerInLobby();
}

void AMyLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyLobbyGameState, CurrentSessionName);

	DOREPLIFETIME(AMyLobbyGameState, CurrentMaxPlayerConnectionAmount);

	DOREPLIFETIME(AMyLobbyGameState, CurrentMaxMonsterAmount);

	DOREPLIFETIME(AMyLobbyGameState, PlayerDataList);
}

void AMyLobbyGameState::RegisterPlayerData(const FCustomPlayerData& CustomPlayerData)
{
	PlayerDataList.Add(CustomPlayerData);

	DisplayEveryPlayerInLobby();
}

void AMyLobbyGameState::RemovePlayerData(AController* Controller)
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

void AMyLobbyGameState::StartLobbyIfReady_Implementation()
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

	Cast<AMyLobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->SetCurrentSessionStatusToLaunched();
}

void AMyLobbyGameState::ChangePlayerCurrentTeam_Implementation(AController* Controller, ETeam NewTeam)
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

void AMyLobbyGameState::DestroyGame_Implementation()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		if (AMyLobbyPlayerController* MyLPC = Cast<AMyLobbyPlayerController>(It->Get()))
		{
			MyLPC->DestroySessionOnClient();
		}
	}
}
