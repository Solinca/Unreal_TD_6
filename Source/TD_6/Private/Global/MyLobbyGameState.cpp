#include "Global/MyLobbyGameState.h"
#include "Global/MyLobbyGameMode.h"
#include "Global/MyGameInstance.h"
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

	GetGameInstance<UMyGameInstance>()->AddPlayerToServerList(CustomPlayerData);

	DisplayEveryPlayerInLobby();
}

void AMyLobbyGameState::RemovePlayerData(AController* Controller)
{
	if (Controller->IsLocalController())
	{
		DestroyGame();

		return;
	}

	FUniqueNetIdRepl PlayerID = Controller->GetPlayerState<APlayerState>()->GetUniqueId();

	GetGameInstance<UMyGameInstance>()->RemovePlayerFromServerList(PlayerID);

	for (FCustomPlayerData Data : PlayerDataList)
	{
		if (Data.CustomPlayerID == PlayerID)
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
	FUniqueNetIdRepl PlayerID = Controller->GetPlayerState<APlayerState>()->GetUniqueId();

	for (FCustomPlayerData& Data : PlayerDataList)
	{
		if (Data.CustomPlayerID == PlayerID)
		{
			Data.CurrentTeam = NewTeam;

			GetGameInstance<UMyGameInstance>()->SetServerPlayerData(Data);

			if (AMyLobbyPlayerController* MyLPC = Cast<AMyLobbyPlayerController>(Controller))
			{
				MyLPC->UpdatePlayerData(Data);
			}

			break;
		}
	}

	DisplayEveryPlayerInLobby();
}

void AMyLobbyGameState::ChangePlayerMonsterType_Implementation(AController* Controller, EMonsterType MonsterType)
{
	FUniqueNetIdRepl PlayerID = Controller->GetPlayerState<APlayerState>()->GetUniqueId();

	for (FCustomPlayerData& Data : PlayerDataList)
	{
		if (Data.CustomPlayerID == PlayerID && Data.CurrentTeam == ETeam::MONSTER)
		{
			Data.MonsterType = MonsterType;

			GetGameInstance<UMyGameInstance>()->SetServerPlayerData(Data);

			if (AMyLobbyPlayerController* MyLPC = Cast<AMyLobbyPlayerController>(Controller))
			{
				MyLPC->UpdatePlayerData(Data);
			}

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
