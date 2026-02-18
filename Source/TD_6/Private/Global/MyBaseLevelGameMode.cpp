#include "Global/MyBaseLevelGameMode.h"
#include "Global/MyBaseLevelGameState.h"
#include "Global/MyGameInstance.h"
#include "GameFramework/PlayerState.h"

void AMyBaseLevelGameMode::PostLogin(APlayerController* Controller)
{
	Super::PostLogin(Controller);

	GetGameState<AMyBaseLevelGameState>()->HandlePlayer(Controller);
}

void AMyBaseLevelGameMode::Logout(AController* Controller)
{
	Super::Logout(Controller);

	GetGameState<AMyBaseLevelGameState>()->RemovePlayer(Controller);
}

UClass* AMyBaseLevelGameMode::GetDefaultPawnClassForController_Implementation(AController* Controller)
{
	FCustomPlayerData Data = GetGameInstance<UMyGameInstance>()->RetrieveServerPlayerDataList()[Controller->GetPlayerState<APlayerState>()->GetUniqueId()];

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
