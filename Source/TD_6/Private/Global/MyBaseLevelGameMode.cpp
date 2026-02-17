#include "Global/MyBaseLevelGameMode.h"
#include "Global/MyBaseLevelGameState.h"

void AMyBaseLevelGameMode::Logout(AController* Controller)
{
	Super::Logout(Controller);

	GetGameState<AMyBaseLevelGameState>()->RemovePlayerData(Controller);
}
