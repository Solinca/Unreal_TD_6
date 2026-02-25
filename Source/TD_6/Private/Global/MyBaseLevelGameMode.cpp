#include "Global/MyBaseLevelGameMode.h"
#include "Global/MyBaseLevelGameState.h"
#include "Global/MyGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

void AMyBaseLevelGameMode::PostLogin(APlayerController* Controller)
{
	Super::PostLogin(Controller);

	GetGameState<AMyBaseLevelGameState>()->HandlePlayer(Controller);
}

void AMyBaseLevelGameMode::Logout(AController* Controller)
{
	Super::Logout(Controller);

	if (!GetWorld() || GetWorld()->bIsTearingDown)
	{
		return;
	}

	GetGameState<AMyBaseLevelGameState>()->RemovePlayer(Controller);
}

AActor* AMyBaseLevelGameMode::ChoosePlayerStart_Implementation(AController* Controller)
{
	FCustomPlayerData Data = GetGameInstance<UMyGameInstance>()->RetrieveServerPlayerData(Controller->GetPlayerState<APlayerState>()->GetUniqueId());

	TArray<APlayerStart*> PlayerStartList;

	TArray<APlayerStart*> MonsterStartList;

	for (TActorIterator<APlayerStart> PlayerStartIterator(GetWorld()); PlayerStartIterator; ++PlayerStartIterator)
	{
		APlayerStart* PlayerStart = *PlayerStartIterator;

		if (PlayerStart)
		{
			if (PlayerStart->PlayerStartTag == FName("PLAYER"))
			{
				PlayerStartList.Add(PlayerStart);
			}
			else if (PlayerStart->PlayerStartTag == FName("MONSTER"))
			{
				MonsterStartList.Add(PlayerStart);
			}
		}
	}

	return Data.CurrentTeam == ETeam::PLAYER ? PlayerStartList[FMath::Rand() % PlayerStartList.Num()] : MonsterStartList[FMath::Rand() % MonsterStartList.Num()];
}

UClass* AMyBaseLevelGameMode::GetDefaultPawnClassForController_Implementation(AController* Controller)
{
	FCustomPlayerData Data = GetGameInstance<UMyGameInstance>()->RetrieveServerPlayerData(Controller->GetPlayerState<APlayerState>()->GetUniqueId());

	if (Data.CurrentTeam == ETeam::PLAYER)
	{
		return PlayerPawnClass;
	}
	else if (Data.CurrentTeam == ETeam::MONSTER)
	{
		switch (Data.MonsterType)
		{
			case EMonsterType::BUTCHER:
				return ButcherMonsterPawnClass;

			case EMonsterType::HUNTER:
				return HunterMonsterPawnClass;

			case EMonsterType::SLIME:
				return SlimeMonsterPawnClass;

			case EMonsterType::GHOST:
				return GhostMonsterPawnClass;

			case EMonsterType::PREDATOR:
				return PredatorMonsterPawnClass;

			default:
				return PlayerPawnClass;
		}
	}

	return PlayerPawnClass;
}
