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

	for (TActorIterator<ATargetPoint> TargetPointIterator(GetWorld()); TargetPointIterator; ++TargetPointIterator)
	{
		if (TargetPointIterator->Tags.Contains(FName("PlayerObjectiveSpawnPoint")))
		{
			PlayerObjectiveSpawnPoints.Add(*TargetPointIterator);
		}
		else if (TargetPointIterator->Tags.Contains(FName("WinningTargetPoint")))
		{
			WinningTargetPointList.Add(*TargetPointIterator);
		}
		else if (TargetPointIterator->Tags.Contains(FName("LoosingTargetPoint")))
		{
			LoosingTargetPointList.Add(*TargetPointIterator);
		}
	}

	for (int i = 0; i < GetGameInstance<UMyGameInstance>()->GetNumberOfPlayers(); i++)
	{
		int RandomSpawnPointIndex = FMath::Rand() % PlayerObjectiveSpawnPoints.Num();

		ATargetPoint* PlayerObjectiveSpawnPoint = PlayerObjectiveSpawnPoints[RandomSpawnPointIndex];

		PlayerObjectiveSpawnPoints.RemoveAt(RandomSpawnPointIndex);

		GetWorld()->SpawnActor<APlayerObjective>(PlayerObjective, PlayerObjectiveSpawnPoint->GetActorLocation(), PlayerObjectiveSpawnPoint->GetActorRotation());
	}
}

void AMyBaseLevelGameState::RegisterPlayerObjectiveCompleted()
{
	CurrentNumberOfCompletedPlayerObjective++;

	if (CurrentNumberOfCompletedPlayerObjective == GetGameInstance<UMyGameInstance>()->GetNumberOfPlayers())
	{
		GetWorld()->GetTimerManager().ClearTimer(GameGlobalTimerHandle);

		TriggerResultScreen(ETeam::PLAYER);
	}
}

void AMyBaseLevelGameState::CountdownTimer()
{
	TimeToWaitBeforeGameStart--;

	if (TimeToWaitBeforeGameStart <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameStartCountdownHandle);

		GetWorld()->GetTimerManager().SetTimer(GameGlobalTimerHandle, this, &AMyBaseLevelGameState::CountdownGlobalTimer, 1.f, true);
	}

	DisplayCountdownToClients();
}

void AMyBaseLevelGameState::DisplayCountdownToClients()
{
	if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		MyPC->DisplayCountdown(TimeToWaitBeforeGameStart);
	}
}

void AMyBaseLevelGameState::CountdownGlobalTimer()
{
	GlobalGameTimer--;

	if (GlobalGameTimer <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameGlobalTimerHandle);

		if (CurrentNumberOfCompletedPlayerObjective >= GetGameInstance<UMyGameInstance>()->GetNumberOfPlayers() - CurrentNumberOfMonsterPlayer)
		{
			TriggerResultScreen(ETeam::PLAYER);
		}
		else
		{
			TriggerResultScreen(ETeam::MONSTER);
		}
	}

	DisplayTimerToClients();
}

void AMyBaseLevelGameState::DisplayTimerToClients()
{
	if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		MyPC->DisplayGlobalTimer(GlobalGameTimer);
	}
}

void AMyBaseLevelGameState::TriggerResultScreen(ETeam WinningTeam)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		FCustomPlayerData PlayerData = GetGameInstance<UMyGameInstance>()->RetrieveServerPlayerData(It->Get()->GetPlayerState<APlayerState>()->GetUniqueId());

		if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(It->Get()))
		{
			FVector PlayerPosition = FVector::Zero();

			if (PlayerData.CurrentTeam == WinningTeam)
			{
				if (!WinningTargetPointList.IsEmpty())
				{
					PlayerPosition = WinningTargetPointList.Pop()->GetActorLocation();
				}
			}
			else
			{
				if (!LoosingTargetPointList.IsEmpty())
				{
					PlayerPosition = LoosingTargetPointList.Pop()->GetActorLocation();
				}
			}

			MyPC->DisplayResultScreen(WinningTeam, PlayerPosition);
		}
	}

	FTimerHandle TestLevel;

	GetWorld()->GetTimerManager().SetTimer(TestLevel, [this]
	{
		GetWorld()->ServerTravel("/Game/Levels/LobbyManagement?Listen");
	}, 2, false);
}

void AMyBaseLevelGameState::DestroyGame_Implementation()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(It->Get()))
		{
			MyPC->DestroySessionOnClient();
		}
	}
}

void AMyBaseLevelGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyBaseLevelGameState, TimeToWaitBeforeGameStart);
}
