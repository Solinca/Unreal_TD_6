#include "Global/MyBaseLevelGameState.h"
#include "GameFramework/PlayerState.h"
#include "Player/MyPlayerController.h"
#include "Player/PlayerObjective.h"
#include "Data/MonsterDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "Engine/TargetPoint.h"
#include "EngineUtils.h"

void AMyBaseLevelGameState::HandlePlayer(AController* Controller)
{
	FCustomPlayerData PlayerData = GetGameInstance<UMyGameInstance>()->RetrieveServerPlayerData(Controller->GetPlayerState<APlayerState>()->GetUniqueId());

	if (PlayerData.CurrentTeam == ETeam::MONSTER)
	{
		CurrentNumberOfMonsterPlayer++;
	}

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
		SetupPlayerObjectives();

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

void AMyBaseLevelGameState::SetupPlayerObjectives()
{
	TArray<ATargetPoint*> PlayerObjectiveSpawnPoints;

	for (TActorIterator<ATargetPoint> PlayerObjectiveIterator(GetWorld()); PlayerObjectiveIterator; ++PlayerObjectiveIterator)
	{
		if (PlayerObjectiveIterator->Tags.Contains(FName("PlayerObjectiveSpawnPoint")))
		{
			PlayerObjectiveSpawnPoints.Add(*PlayerObjectiveIterator);
		}
	}

	for (int i = 0; i < CurrentNumberOfMonsterPlayer + 1; i++)
	{
		int RandomSpawnPointIndex = FMath::Rand() % PlayerObjectiveSpawnPoints.Num();

		ATargetPoint* PlayerObjectiveSpawnPoint = PlayerObjectiveSpawnPoints[RandomSpawnPointIndex];

		PlayerObjectiveSpawnPoints.RemoveAt(RandomSpawnPointIndex);

		GetWorld()->SpawnActor<APlayerObjective>(PlayerObjective, PlayerObjectiveSpawnPoint->GetActorLocation(), PlayerObjectiveSpawnPoint->GetActorRotation());
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
