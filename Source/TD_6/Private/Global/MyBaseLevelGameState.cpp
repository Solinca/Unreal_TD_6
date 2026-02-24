#include "Global/MyBaseLevelGameState.h"
#include "GameFramework/PlayerState.h"
#include "Player/MyPlayerController.h"
#include "Data/MonsterDataAsset.h"
#include "Net/UnrealNetwork.h"

void AMyBaseLevelGameState::HandlePlayer(AController* Controller)
{
	FCustomPlayerData PlayerData = GetGameInstance<UMyGameInstance>()->RetrieveServerPlayerData(Controller->GetPlayerState<APlayerState>()->GetUniqueId());

	Cast<AMyPlayerController>(Controller)->SetupServer(PlayerData, MonsterDataPerType[PlayerData.MonsterType]);

	Cast<AMyPlayerController>(Controller)->SetupClient(PlayerData, MonsterDataPerType[PlayerData.MonsterType], PlayerWaitingTimeAtStart);
}

void AMyBaseLevelGameState::RemovePlayer(AController* Controller)
{
	if (Controller->IsLocalController())
	{
		DestroyGame();
	}
}

void AMyBaseLevelGameState::PlayerHasLoaded_Implementation()
{
	CurrentLoadedPlayer++;

	if (CurrentLoadedPlayer >= GetGameInstance<UMyGameInstance>()->GetNumberOfPlayers())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(It->Get()))
			{
				MyPC->ToggleWaitingScreenOff();
			}
		}

		GetWorld()->GetTimerManager().SetTimer(GameStartCountdownHandle, this, &AMyBaseLevelGameState::CountdownTimer, 1.f, true);
	}
}

void AMyBaseLevelGameState::CountdownTimer()
{
	TimeToWaitBeforeGameStart--;

	if (TimeToWaitBeforeGameStart <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameStartCountdownHandle);
	}

	DisplayTimerToClients();
}

void AMyBaseLevelGameState::DisplayTimerToClients()
{
	if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		MyPC->DisplayCountdown(TimeToWaitBeforeGameStart);
	}
}

void AMyBaseLevelGameState::DestroyGame_Implementation()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		if (AMyPlayerController* MyLPC = Cast<AMyPlayerController>(It->Get()))
		{
			MyLPC->DestroySessionOnClient();
		}
	}
}

void AMyBaseLevelGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyBaseLevelGameState, TimeToWaitBeforeGameStart);
}
